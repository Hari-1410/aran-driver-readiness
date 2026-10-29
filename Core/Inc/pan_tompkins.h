/**
 ******************************************************************************
 * @file    pan_tompkins.h
 * @brief   Pan-Tompkins QRS / R-peak detector for ARAN Milestone 3.
 *
 *   Implements the five-stage pipeline from the original 1985 paper:
 *     1. Derivative filter  – emphasises steep QRS slopes
 *     2. Squaring           – makes all values positive, amplifies large slopes
 *     3. Moving-window integration (150 ms window @ 250 Hz = 37 samples)
 *     4. Adaptive dual-threshold peak detection
 *     5. R-peak confirmation and index recording
 *
 *   Input:  normalized float32 ECG window (2500 samples, 250 Hz)
 *           produced by DSP_Preprocess() in dsp_preprocessing.c.
 *   Output: array of R-peak sample indices + count, stored in PT_Result_t.
 *
 *   All intermediate buffers are statically allocated inside pan_tompkins.c
 *   so no heap allocation is needed.
 *
 * ARAN : Milestone 3 - Pan-Tompkins and HRV Feature Extraction
 ******************************************************************************
 */

#ifndef INC_PAN_TOMPKINS_H_
#define INC_PAN_TOMPKINS_H_

#include "arm_math.h"
#include <stdint.h>


/* Maximum number of R-peaks we can store per 2500-sample window.
 * At 250 Hz and a maximum plausible HR of 220 bpm the inter-peak
 * distance is >= 68 samples, giving at most ~36 peaks per 10 s window.
 * 64 gives comfortable headroom. */
#define PT_MAX_PEAKS   64U

#define PT_WINDOW_SIZE  2500U   /* must match DSP_WINDOW_SIZE / WINDOW_SIZE */
#define PT_FS           250U    /* sampling frequency in Hz                  */

/**
 * @brief Container returned by PT_Detect(); consumed by HRV_Compute().
 */
typedef struct {
    uint32_t peak_indices[PT_MAX_PEAKS];  /**< sample indices of detected R-peaks */
    uint32_t peak_count;                  /**< number of valid entries in peak_indices */
} PT_Result_t;

/**
 * @brief Reset all adaptive-threshold state. Call once at startup and
 *        optionally between recordings if processing is restarted.
 */
void PT_Init(void);

/**
 * @brief Run the complete Pan-Tompkins pipeline on one 2500-sample window.
 *
 * @param normalized_ecg  Pointer to PT_WINDOW_SIZE float32 samples
 *                         (output of DSP_Preprocess).
 * @param result          Pointer to caller-allocated PT_Result_t.  The
 *                         function fills peak_indices[] and peak_count.
 */
void PT_Detect(const float32_t *normalized_ecg, PT_Result_t *result);

#endif /* INC_PAN_TOMPKINS_H_ */
