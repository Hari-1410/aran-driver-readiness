/**
 ******************************************************************************
 * @file    cnn.c
 * @brief   CNN inference wrapper for ARAN Milestone 4 (X-CUBE-AI).
 *
 *   *** REVISION NOTE (fixing constant 0.000 fatigue probability) ***
 *   The previous version of this file assumed the generated network's
 *   input/output tensors were AI_BUFFER_FORMAT_FLOAT. If your model
 *   was exported as a fully INT8-quantized TFLite model (per spec
 *   Section 12.3), X-CUBE-AI's generated I/O tensors are very likely
 *   AI_BUFFER_FORMAT_S8 (or _U8) instead — meaning the firmware was
 *   writing/reading 4-byte floats against buffers the runtime treats
 *   as 1-byte quantized values. ai_network_run() still "succeeds" in
 *   that case (hence STATUS,OK), but the input is garbage and the
 *   output byte you read as a float is not a valid probability —
 *   which is consistent with a constant near-zero reading.
 *
 *   This version checks the ACTUAL format of the generated input/
 *   output buffers at CNN_Init() time and branches accordingly:
 *     - FLOAT format  : behaves exactly like the previous version.
 *     - S8/U8 format  : quantizes the input using the buffer's own
 *                       scale/zero-point, and dequantizes the output
 *                       the same way, per the standard TFLite affine
 *                       quantization formula:
 *                           real_value = scale * (quantized - zero_point)
 *
 *   IMPORTANT: the exact meta-info macro names below
 *   (AI_BUFFER_META_INFO_INTQ / _GET_SCALE / _GET_ZEROPOINT) match the
 *   X-CUBE-AI API used in ST's own generated getting-started templates
 *   as of recent versions. If your ai_platform.h/network.h use
 *   different macro names, the build will fail on those specific
 *   lines — search ai_platform.h for "META_INFO" or "INTQ" and swap
 *   in the matching names; the quantization MATH does not change.
 *
 * ARAN : Milestone 4 - Edge AI CNN Inference
 ******************************************************************************
 */

#include "cnn.h"
#include "ecg_uart.h"
#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------------------- */
/* X-CUBE-AI generated headers                                             */
/* ---------------------------------------------------------------------- */
#include "network.h"
#include "network_data.h"

/* ---------------------------------------------------------------------- */
/* DWT cycle counter helpers (for sub-ms inference timing)                */
/* ---------------------------------------------------------------------- */
#include "stm32f4xx_hal.h"

#define DWT_CYCCNT      (*(volatile uint32_t *)0xE0001004U)
#define DWT_CTRL        (*(volatile uint32_t *)0xE0001000U)
#define DWT_LAR         (*(volatile uint32_t *)0xE0001FB0U)
#define CORE_DEBUG_DEMCR (*(volatile uint32_t *)0xE000EDFCU)

static void dwt_init(void)
{
    CORE_DEBUG_DEMCR |= (1U << 24);
    DWT_LAR = 0xC5ACCE55U;
    DWT_CYCCNT = 0U;
    DWT_CTRL |= 1U;
}

static inline uint32_t dwt_now(void)
{
    return DWT_CYCCNT;
}

static inline float32_t cycles_to_ms(uint32_t cycles)
{
    return ((float32_t)cycles / (float32_t)SystemCoreClock) * 1000.0f;
}

/* ---------------------------------------------------------------------- */
/* X-CUBE-AI network state                                                 */
/* ---------------------------------------------------------------------- */
static ai_handle   s_network = AI_HANDLE_NULL;
static ai_error    s_last_error;
static bool        s_initialized = false;

/* True if the network's input/output tensors are native float32.
 * False means they are quantized (S8/U8) and need manual
 * quantize-in / dequantize-out. Determined at CNN_Init() time from
 * the ACTUAL generated buffer format, not assumed. */
static bool s_input_is_float  = true;
static bool s_output_is_float = true;

/* Quantization parameters, read from the generated buffers at init
 * (only meaningful when the corresponding *_is_float flag is false). */
static float32_t s_in_scale  = 1.0f;
static int32_t   s_in_zp     = 0;
static float32_t s_out_scale = 1.0f;
static int32_t   s_out_zp    = 0;

AI_ALIGNED(4)
static ai_u8 s_activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

/* Raw tensor buffers, sized in BYTES so they're correct whether the
 * network is float32 (4 bytes/element) or int8 (1 byte/element).
 * We reinterpret these through the right pointer type depending on
 * the format detected at init. */
AI_ALIGNED(4)
static ai_u8 s_in_raw[AI_NETWORK_IN_1_SIZE_BYTES];
AI_ALIGNED(4)
static ai_u8 s_out_raw[AI_NETWORK_OUT_1_SIZE_BYTES];

static ai_buffer *s_ai_input;
static ai_buffer *s_ai_output;

/* ---------------------------------------------------------------------- */
/* Public API                                                              */
/* ---------------------------------------------------------------------- */

bool CNN_Init(void)
{
    dwt_init();
    memset(&s_last_error, 0, sizeof(s_last_error));

    const ai_handle acts[] = { s_activations };

    ai_error err = ai_network_create_and_init(&s_network, acts, NULL);
    if (err.type != AI_ERROR_NONE)
    {
        s_last_error  = err;
        s_initialized = false;
        return false;
    }

    s_ai_input  = ai_network_inputs_get(s_network, NULL);
    s_ai_output = ai_network_outputs_get(s_network, NULL);

    if (s_ai_input == NULL || s_ai_output == NULL)
    {
        s_initialized = false;
        return false;
    }

    if (AI_NETWORK_IN_1_SIZE != CNN_INPUT_SIZE)
    {
        s_initialized = false;
        return false;
    }

    /* ---- Detect actual tensor formats (the fix) ---- */
    s_input_is_float  = (AI_BUFFER_FORMAT(&s_ai_input[0])  == AI_BUFFER_FORMAT_FLOAT);
    s_output_is_float = (AI_BUFFER_FORMAT(&s_ai_output[0]) == AI_BUFFER_FORMAT_FLOAT);

    if (!s_input_is_float)
    {
        /* Pull the input tensor's quantization scale/zero-point from
         * its meta-info so we quantize with the values the model was
         * actually trained/exported with, not a guessed default. */
        const ai_buffer_meta_info *meta = s_ai_input[0].meta_info;
        if (meta != NULL && AI_BUFFER_META_INFO_INTQ(meta))
        {
            s_in_scale = AI_BUFFER_META_INFO_INTQ_GET_SCALE(meta, 0);
            s_in_zp    = AI_BUFFER_META_INFO_INTQ_GET_ZEROPOINT(meta, 0);
        }
    }

    if (!s_output_is_float)
    {
        const ai_buffer_meta_info *meta = s_ai_output[0].meta_info;
        if (meta != NULL && AI_BUFFER_META_INFO_INTQ(meta))
        {
            s_out_scale = AI_BUFFER_META_INFO_INTQ_GET_SCALE(meta, 0);
            s_out_zp    = AI_BUFFER_META_INFO_INTQ_GET_ZEROPOINT(meta, 0);
        }
    }

    s_ai_input[0].data  = AI_HANDLE_PTR(s_in_raw);
    s_ai_output[0].data = AI_HANDLE_PTR(s_out_raw);

    s_initialized = true;
    return true;
}

void CNN_Run(const float32_t *normalized_ecg, CNN_Result_t *result)
{
    memset(result, 0, sizeof(CNN_Result_t));

    if (!s_initialized || normalized_ecg == NULL)
    {
        result->valid = false;
        return;
    }

    /* ---- Write input tensor, quantizing if required ---- */
    if (s_input_is_float)
    {
        arm_copy_f32((float32_t *)normalized_ecg, (float32_t *)s_in_raw,
                     CNN_INPUT_SIZE);
    }
    else
    {
        int8_t *in_q = (int8_t *)s_in_raw;
        for (uint32_t i = 0U; i < CNN_INPUT_SIZE; i++)
        {
            /* Affine quantization: q = round(real / scale) + zero_point */
            int32_t q = (int32_t)lrintf(normalized_ecg[i] / s_in_scale) + s_in_zp;
            if (q < -128) q = -128;
            if (q > 127)  q = 127;
            in_q[i] = (int8_t)q;
        }
    }

    uint32_t t_start = dwt_now();
    ai_i32 batches_run = ai_network_run(s_network, s_ai_input, s_ai_output);
    uint32_t t_end = dwt_now();

    if (batches_run != 1)
    {
        s_last_error = ai_network_get_error(s_network);
        result->valid = false;
        return;
    }

    /* ---- Read output tensor, dequantizing if required ---- */
    float32_t prob;
    if (s_output_is_float)
    {
        prob = ((float32_t *)s_out_raw)[0];
    }
    else
    {
        int8_t q = ((int8_t *)s_out_raw)[0];
        prob = s_out_scale * (float32_t)((int32_t)q - s_out_zp);
    }

    if (prob < 0.0f) prob = 0.0f;
    if (prob > 1.0f) prob = 1.0f;

    result->fatigueProbability = prob;
    result->inferenceTimeMs    = cycles_to_ms(t_end - t_start);
    result->valid              = true;
}

void CNN_SendUART(uint32_t window_id, const CNN_Result_t *result)
{
    char line[64];

    snprintf(line, sizeof(line), "C,WIN,%lu", (unsigned long)window_id);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "C,PROB,%.4f",
             (double)result->fatigueProbability);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "C,TIME,%.3f",
             (double)result->inferenceTimeMs);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "C,STATUS,%s",
             result->valid ? "OK" : "FAIL");
    ECG_UART_SendStatus(line);

    /* ---- Temporary debug line — remove once verified ----
     * Confirms which branch was taken and the raw output byte, so you
     * can see directly whether dequantization is actually engaging. */
    snprintf(line, sizeof(line), "C,DBG,informat=%d,outformat=%d,rawout0=%d",
             (int)s_input_is_float, (int)s_output_is_float,
             (int)((int8_t *)s_out_raw)[0]);
    ECG_UART_SendStatus(line);
}

uint32_t CNN_GetLastError(void)
{
    return ((uint32_t)s_last_error.type << 16) | (uint32_t)s_last_error.code;
}
