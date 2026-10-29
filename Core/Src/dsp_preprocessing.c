/**
 ******************************************************************************
 * @file    dsp_preprocessing.c
 * @brief   Implementation of Notch -> Bandpass -> Normalization pipeline.
 *
 *   Coefficients generated for fs = 250 Hz via python/design_filters.py:
 *       Notch:     iirnotch(f0=50 Hz, Q=30)
 *       Bandpass:  butter(order=4, [0.5, 40] Hz, output='sos') -> 4 biquads
 ******************************************************************************
 */

#include "dsp_preprocessing.h"
#include <math.h>

/* ---------------------------------------------------------------------- */
/* Notch filter: 1 biquad stage, 50 Hz, Q = 30, fs = 250 Hz               */
/* CMSIS df2T coefficient order per stage: {b0, b1, b2, -a1, -a2}          */
/* ---------------------------------------------------------------------- */
#define NOTCH_NUM_STAGES 1U

static float32_t notch_coeffs[5U * NOTCH_NUM_STAGES] = {
    0.9794827610f, -0.6053536377f, 0.9794827610f,
    0.6053536377f, -0.9589655220f
};

static float32_t notch_state[4U * NOTCH_NUM_STAGES];
static arm_biquad_cascade_df2T_instance_f32 notch_instance;

/* ---------------------------------------------------------------------- */
/* Bandpass filter: 4th-order Butterworth, 0.5-40 Hz, fs = 250 Hz,        */
/* implemented as 4 cascaded biquad sections.                             */
/* ---------------------------------------------------------------------- */
#define BANDPASS_NUM_STAGES 4U

static float32_t bandpass_coeffs[5U * BANDPASS_NUM_STAGES] = {
    /* stage 1 */  0.0219612634f,  0.0439225269f,  0.0219612634f,  0.6215552807f, -0.1305843533f,
    /* stage 2 */  1.0000000000f,  2.0000000000f,  1.0000000000f,  0.8176588707f, -0.5194597166f,
    /* stage 3 */  1.0000000000f, -2.0000000000f,  1.0000000000f,  1.9765144890f, -0.9766768505f,
    /* stage 4 */  1.0000000000f, -2.0000000000f,  1.0000000000f,  1.9904258739f, -0.9905840603f
};

static float32_t bandpass_state[4U * BANDPASS_NUM_STAGES];
static arm_biquad_cascade_df2T_instance_f32 bandpass_instance;

/* ---------------------------------------------------------------------- */
/* Scratch buffer for intermediate stage output (notch -> bandpass)       */
/* ---------------------------------------------------------------------- */
static float32_t scratch[DSP_WINDOW_SIZE];

void DSP_Init(void)
{
    arm_biquad_cascade_df2T_init_f32(&notch_instance, NOTCH_NUM_STAGES,
                                      notch_coeffs, notch_state);

    arm_biquad_cascade_df2T_init_f32(&bandpass_instance, BANDPASS_NUM_STAGES,
                                      bandpass_coeffs, bandpass_state);
}

/**
 * @brief In-window z-score normalization: (x - mean) / std.
 *        Uses CMSIS-DSP for mean/variance, falls back to a tiny epsilon
 *        guard against divide-by-zero on a flat-line input.
 */
static void normalize_window(const float32_t *in, float32_t *out, uint32_t len)
{
    float32_t mean;
    float32_t variance;
    float32_t std_dev;
    const float32_t EPSILON = 1e-8f;

    arm_mean_f32((float32_t *)in, len, &mean);

    /* Compute variance manually: mean of (x - mean)^2.
     * (CMSIS arm_var_f32 uses the (N-1) denominator convention like
     * numpy's ddof=1; if your training pipeline used population variance
     * (ddof=0), this manual version matches that exactly.) */
    float32_t sum_sq_diff = 0.0f;
    for (uint32_t i = 0U; i < len; i++)
    {
        float32_t diff = in[i] - mean;
        sum_sq_diff += diff * diff;
    }
    variance = sum_sq_diff / (float32_t)len;
    arm_sqrt_f32(variance, &std_dev);

    if (std_dev < EPSILON)
    {
        std_dev = EPSILON; /* avoid divide-by-zero on a flat/dead signal */
    }

    for (uint32_t i = 0U; i < len; i++)
    {
        out[i] = (in[i] - mean) / std_dev;
    }
}

void DSP_Preprocess(const float32_t *input, float32_t *output)
{
    /* Stage 1: Notch filter (50 Hz powerline removal) */
    arm_biquad_cascade_df2T_f32(&notch_instance, (float32_t *)input,
                                 scratch, DSP_WINDOW_SIZE);

    /* Stage 2: Bandpass filter (0.5 - 40 Hz), in place on scratch */
    arm_biquad_cascade_df2T_f32(&bandpass_instance, scratch,
                                 scratch, DSP_WINDOW_SIZE);

    /* Stage 3: Per-window z-score normalization -> final output */
    normalize_window(scratch, output, DSP_WINDOW_SIZE);
}
