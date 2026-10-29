/**
 ******************************************************************************
 * @file    cnn.h
 * @brief   CNN inference wrapper for ARAN Milestone 4 (X-CUBE-AI).
 *
 *   This module wraps the X-CUBE-AI generated network API to run the
 *   1D-CNN "Fatigue Probability" model on a 2500-sample normalized ECG
 *   window (output of the Milestone 2 DSP stage), and reports the
 *   result plus timing information over USART1.
 *
 *   ------------------------------------------------------------------
 *   IMPORTANT — READ BEFORE BUILDING
 *   ------------------------------------------------------------------
 *   This file assumes X-CUBE-AI has already generated the following
 *   inside your project, from CubeMX, after you imported your
 *   INT8-quantized .tflite model and clicked "Generate Code":
 *
 *       Middlewares/ST/AI/Inc/...           (already in your include
 *                                            path per the Milestone 1
 *                                            build log)
 *       X-CUBE-AI/App/network.h
 *       X-CUBE-AI/App/network_data.h
 *       X-CUBE-AI/App/network.c
 *       X-CUBE-AI/App/network_data.c
 *
 *   The exact generated symbol names (ai_network_create_and_init,
 *   AI_NETWORK_IN_1_SIZE, etc.) depend on the "Network name" you gave
 *   the model inside CubeMX's X-CUBE-AI panel. "network" is the
 *   CubeMX default and is assumed throughout this file — that's why
 *   you'll see #include "network.h" and prefixes like ai_network_*.
 *
 *   If you named your network something else (e.g. "carma_cnn"), you
 *   must either:
 *     (a) rename it back to "network" in CubeMX and regenerate, or
 *     (b) do a project-wide find/replace of every "network" /
 *         "AI_NETWORK_" token in cnn.c to match your actual generated
 *         names (check X-CUBE-AI/App/network.h for the real symbols).
 *
 *   This wrapper cannot be validated against your specific model
 *   until you've generated those files — treat it as the integration
 *   scaffold the spec (Section 12.8) asks for, not a finished,
 *   pre-tested module.
 ******************************************************************************
 */
#ifndef CNN_H
#define CNN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"

/* Must match the Milestone 2/3 window length */
#define CNN_INPUT_SIZE      2500U

typedef struct
{
    float32_t fatigueProbability;  /* 0.0 (alert) .. 1.0 (drowsy)         */
    float32_t inferenceTimeMs;     /* wall-clock inference time (ms)     */
    bool      valid;               /* true if inference completed OK     */
} CNN_Result_t;

/**
 * @brief Creates and initializes the X-CUBE-AI network + activation
 *        buffers. Call exactly once at startup, after HAL_Init() and
 *        SystemClock_Config(), before the main processing loop.
 * @return true on success, false if network creation/init failed
 *         (check CNN_GetLastError() for the ai_error code in that case).
 */
bool CNN_Init(void);

/**
 * @brief Runs CNN inference on one normalized ECG window.
 * @param normalized_ecg  Pointer to CNN_INPUT_SIZE float32 samples —
 *                        the output of the Milestone 2 Z-score
 *                        normalization stage. Caller retains ownership;
 *                        this function does not modify the buffer.
 * @param result          Output: fatigue probability + inference time.
 *                        Zeroed with valid=false if inference fails.
 */
void CNN_Run(const float32_t *normalized_ecg, CNN_Result_t *result);

/**
 * @brief Transmits the CNN result over USART1 (debug channel) in the
 *        Milestone 4 frame format, one metric per line:
 *          "C,WIN,<window_id>"
 *          "C,PROB,<fatigueProbability>"
 *          "C,TIME,<inferenceTimeMs>"
 *          "C,STATUS,<OK|FAIL>"
 *        Uses ECG_UART_SendStatus() (from ecg_uart.h) exactly like
 *        HRV_SendUART() does in Milestone 3, so the Python
 *        visualization script can parse it the same way.
 */
void CNN_SendUART(uint32_t window_id, const CNN_Result_t *result);

/**
 * @brief Returns the last ai_error struct encountered during
 *        CNN_Init()/CNN_Run(), packed as a single uint32 code — useful
 *        for printing during bring-up. Returns 0 if no error recorded.
 */
uint32_t CNN_GetLastError(void);

#ifdef __cplusplus
}
#endif

#endif /* CNN_H */
