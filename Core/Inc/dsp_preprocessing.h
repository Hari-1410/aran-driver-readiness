/**
 ******************************************************************************
 * @file    dsp_preprocessing.h
 * @brief   Milestone 2 - DSP Preprocessing: Notch -> Bandpass -> Normalization
 *
 *   Implements exactly the pipeline described in Section 5.1/5.2 of the
 *   spec, using CMSIS-DSP biquad cascade filters operating in float32.
 *
 *   Stage 1 - Notch filter   : removes 50 Hz powerline interference.
 *   Stage 2 - Bandpass filter: 0.5 - 40 Hz, 4th-order Butterworth
 *                              (implemented as 4 cascaded biquad sections).
 *   Stage 3 - Normalization  : z-score, i.e. (x - mean) / std, computed
 *                              per-window (matches common ECG training
 *                              pipelines that normalize each window
 *                              independently before feeding the CNN).
 *
 *   IMPORTANT: The exact notch frequency (50 vs 60 Hz), bandpass corner
 *   frequencies, and normalization method (z-score vs min-max) MUST match
 *   whatever was used in the Python training pipeline (Section 5.2 of the
 *   spec: "the embedded firmware must perform exactly the same
 *   preprocessing as the Python training pipeline"). The coefficients
 *   below were generated for fs = 250 Hz using:
 *       scipy.signal.iirnotch(50, Q=30, fs=250)
 *       scipy.signal.butter(4, [0.5, 40], btype='bandpass', fs=250, output='sos')
 *   using python/design_filters.py. If your training pipeline used
 *   different parameters, re-run that script with the correct values and
 *   paste the regenerated coefficients into dsp_preprocessing.c.
 *
 * ARAN : Milestone 2 - DSP Preprocessing
 ******************************************************************************
 */

#ifndef INC_DSP_PREPROCESSING_H_
#define INC_DSP_PREPROCESSING_H_

#include "arm_math.h"
#include <stdint.h>

#define DSP_WINDOW_SIZE   2500U   /* must match WINDOW_SIZE in window_manager.h */

/**
 * @brief Initialize all filter states. Must be called once at startup
 *        before the first call to DSP_Preprocess(), and again any time
 *        you want to reset filter memory (e.g. after a long gap in data).
 */
void DSP_Init(void);

/**
 * @brief Run the full Notch -> Bandpass -> Normalization pipeline on one
 *        complete 2500-sample window.
 *
 * @param input        pointer to WINDOW_SIZE raw ECG samples
 * @param output       pointer to WINDOW_SIZE floats to receive the fully
 *                      preprocessed (filtered + normalized) signal.
 *                      input and output may NOT alias (must be different
 *                      buffers) because intermediate stages are computed
 *                      in-place into scratch buffers internally.
 */
void DSP_Preprocess(const float32_t *input, float32_t *output);

#endif /* INC_DSP_PREPROCESSING_H_ */
