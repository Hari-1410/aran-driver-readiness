/**
 ******************************************************************************
 * @file    pan_tompkins.c
 * @brief   Pan-Tompkins QRS detector implementation.
 *
 * Pipeline stages (all operate on float32, no dynamic allocation):
 *
 *   Stage 1 – Derivative filter
 *     The classic 5-point finite-difference approximation from the 1985
 *     paper, scaled by 1/(8*T) where T = 1/Fs.  Because the input is
 *     already normalized (z-score, dimensionless) the 1/8T scale factor
 *     is absorbed into the squaring step and the adaptive thresholds, so
 *     we apply only the integer coefficients {-1, -2, 0, 2, 1}·(1/8)
 *     which avoids any artefacts from the large 1/T factor.
 *
 *   Stage 2 – Squaring
 *     y[n] = x[n]^2 (element-wise).
 *
 *   Stage 3 – Moving-window integration
 *     Window length N_mwi = round(0.150 * Fs) = 37 samples (≈ 148 ms).
 *     Implemented as a running-sum with O(1) per sample using an add/
 *     subtract circular buffer rather than a naive O(N) inner loop.
 *
 *   Stage 4 & 5 – Adaptive threshold + peak detection
 *     Dual-threshold scheme from Pan & Tompkins (1985):
 *       SPKI = running estimate of signal (QRS) peak level
 *       NPKI = running estimate of noise peak level
 *       THR1 = NPKI + 0.25*(SPKI - NPKI)   primary threshold
 *       THR2 = 0.5 * THR1                   secondary threshold (T-wave guard)
 *     After a candidate peak exceeds THR1 a refractory blanking period of
 *     200 ms (50 samples @ 250 Hz) is enforced to suppress T-waves and
 *     noise peaks.  A T-wave guard additionally rejects peaks that arrive
 *     within 360 ms of the previous R-peak if their slope is < half the
 *     previous QRS slope.
 *
 * References:
 *   Pan J, Tompkins WJ. "A real-time QRS detection algorithm."
 *   IEEE Trans Biomed Eng. 1985;32(3):230-236.
 ******************************************************************************
 */

#include "pan_tompkins.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>

/* ---------------------------------------------------------------------- */
/* Compile-time constants                                                  */
/* ---------------------------------------------------------------------- */

/* Moving-window integration length: 150 ms @ 250 Hz */
#define PT_MWI_WIN       37U

/* Refractory period: 200 ms @ 250 Hz – no second peak can be accepted   */
#define PT_REFRACTORY    50U

/* T-wave guard window: 360 ms @ 250 Hz                                   */
#define PT_TWAVE_GUARD   90U

/* Learning phase: first 2 seconds of the window used to seed thresholds  */
#define PT_LEARN_SAMPLES (PT_FS * 2U)

/* Adaptive filter coefficients */
#define PT_ALPHA_SIGNAL  0.125f   /* SPKI update weight for true QRS      */
#define PT_ALPHA_NOISE   0.125f   /* NPKI update weight for noise peak    */

/* ---------------------------------------------------------------------- */
/* Static intermediate buffers (all PT_WINDOW_SIZE floats)                */
/* ---------------------------------------------------------------------- */
static float32_t s_deriv[PT_WINDOW_SIZE];
static float32_t s_squared[PT_WINDOW_SIZE];
static float32_t s_mwi[PT_WINDOW_SIZE];

/* ---------------------------------------------------------------------- */
/* Adaptive threshold state (persists across windows for robustness)      */
/* ---------------------------------------------------------------------- */
static float32_t s_spki;   /* signal peak estimate */
static float32_t s_npki;   /* noise  peak estimate */
static float32_t s_thr1;   /* primary threshold    */
static float32_t s_thr2;   /* secondary threshold  */

/* ---------------------------------------------------------------------- */
/* Internal helpers                                                        */
/* ---------------------------------------------------------------------- */

/**
 * @brief Five-point derivative filter.
 *   y[n] = (1/8) * (-x[n-2] - 2x[n-1] + 2x[n+1] + x[n+2])
 * Boundary samples (n < 2, n >= len-2) are zeroed.
 */
static void stage_derivative(const float32_t *in, float32_t *out, uint32_t len)
{
    out[0] = 0.0f;
    out[1] = 0.0f;

    for (uint32_t n = 2U; n < len - 2U; n++)
    {
        out[n] = ((-1.0f * in[n - 2U])
                + (-2.0f * in[n - 1U])
                + ( 2.0f * in[n + 1U])
                + ( 1.0f * in[n + 2U])) * 0.125f;
    }

    out[len - 2U] = 0.0f;
    out[len - 1U] = 0.0f;
}

/**
 * @brief Element-wise squaring.
 */
static void stage_squaring(const float32_t *in, float32_t *out, uint32_t len)
{
    arm_mult_f32((float32_t *)in, (float32_t *)in, out, len);
}

/**
 * @brief Moving-window integrator using a running sum (O(1) per sample).
 * @param win_len  Number of samples in the integration window.
 */
static void stage_mwi(const float32_t *in, float32_t *out,
                       uint32_t len, uint32_t win_len)
{
    float32_t running_sum = 0.0f;
    float32_t inv_win = 1.0f / (float32_t)win_len;

    /* Seed the first win_len-1 samples */
    for (uint32_t i = 0U; i < win_len - 1U && i < len; i++)
    {
        running_sum += in[i];
        out[i] = running_sum * inv_win;
    }

    /* Steady-state: add new, subtract old */
    for (uint32_t i = win_len - 1U; i < len; i++)
    {
        running_sum += in[i];
        running_sum -= in[i - (win_len - 1U)];
        out[i] = running_sum * inv_win;
    }
}

/**
 * @brief Seed SPKI/NPKI from the first PT_LEARN_SAMPLES of the MWI signal.
 *        Uses the max of the learning segment as SPKI and one-third of it
 *        as NPKI, matching the Paper's initialisation recommendation.
 */
static void seed_thresholds(const float32_t *mwi, uint32_t len)
{
    float32_t max_val;
    uint32_t  max_idx;
    uint32_t  n_learn = (PT_LEARN_SAMPLES < len) ? PT_LEARN_SAMPLES : len;

    arm_max_f32((float32_t *)mwi, n_learn, &max_val, &max_idx);

    s_spki = max_val;
    s_npki = max_val * 0.333f;
    s_thr1 = s_npki + 0.25f * (s_spki - s_npki);
    s_thr2 = 0.5f  * s_thr1;
}

/* ---------------------------------------------------------------------- */
/* Public API                                                              */
/* ---------------------------------------------------------------------- */

void PT_Init(void)
{
    s_spki = 0.0f;
    s_npki = 0.0f;
    s_thr1 = 0.0f;
    s_thr2 = 0.0f;
}

void PT_Detect(const float32_t *normalized_ecg, PT_Result_t *result)
{
    /* ------------------------------------------------------------------ */
    /* Stage 1: Derivative                                                  */
    /* ------------------------------------------------------------------ */
    stage_derivative(normalized_ecg, s_deriv, PT_WINDOW_SIZE);

    /* ------------------------------------------------------------------ */
    /* Stage 2: Squaring                                                    */
    /* ------------------------------------------------------------------ */
    stage_squaring(s_deriv, s_squared, PT_WINDOW_SIZE);

    /* ------------------------------------------------------------------ */
    /* Stage 3: Moving-window integration                                  */
    /* ------------------------------------------------------------------ */
    stage_mwi(s_squared, s_mwi, PT_WINDOW_SIZE, PT_MWI_WIN);

    /* ------------------------------------------------------------------ */
    /* Stage 4 + 5: Adaptive threshold and R-peak detection               */
    /* ------------------------------------------------------------------ */
    result->peak_count = 0U;

    /* Seed thresholds from the learning phase if they were zero (first
     * window or after PT_Init()) */
    if (s_spki < 1e-9f)
    {
        seed_thresholds(s_mwi, PT_WINDOW_SIZE);
    }

    /* State variables for the peak-finding pass */
    uint32_t  last_peak_sample   = 0U;   /* sample index of last accepted R */
    float32_t last_qrs_slope     = 0.0f; /* used for T-wave slope guard     */
    bool      in_candidate_region = false;
    float32_t candidate_peak_val  = 0.0f;
    uint32_t  candidate_peak_idx  = 0U;

    for (uint32_t n = 1U; n < PT_WINDOW_SIZE - 1U; n++)
    {
        float32_t cur = s_mwi[n];
        float32_t prv = s_mwi[n - 1U];
        float32_t nxt = s_mwi[n + 1U];

        /* ---- Detect a local maximum ---- */
        bool is_local_max = (cur >= prv) && (cur >= nxt) && (cur > 0.0f);

        if (is_local_max)
        {
            /* Refractory period: ignore peaks within 200 ms of last R */
            uint32_t distance = (last_peak_sample > 0U)
                                 ? (n - last_peak_sample)
                                 : PT_REFRACTORY + 1U;

            if (distance <= PT_REFRACTORY)
            {
                /* Within refractory – skip, but still update noise level */
                s_npki = s_npki + PT_ALPHA_NOISE * (cur - s_npki);
                s_thr1 = s_npki + 0.25f * (s_spki - s_npki);
                s_thr2 = 0.5f * s_thr1;
                continue;
            }

            if (cur >= s_thr1)
            {
                /* ---- T-wave guard ----
                 * If this peak arrives within 360 ms of the previous R and
                 * its slope is less than half the previous QRS slope,
                 * classify it as a T-wave and update noise. */
                bool is_twave = false;
                if (distance < PT_TWAVE_GUARD && last_peak_sample > 0U)
                {
                    float32_t cur_slope = s_deriv[n]; /* already computed */
                    if (cur_slope < 0.0f) cur_slope = -cur_slope;
                    if (last_qrs_slope > 0.0f &&
                        cur_slope < 0.5f * last_qrs_slope)
                    {
                        is_twave = true;
                    }
                }

                if (!is_twave)
                {
                    /* ---- Accepted R-peak ---- */
                    if (result->peak_count < PT_MAX_PEAKS)
                    {
                        result->peak_indices[result->peak_count++] = n;
                    }

                    /* Update signal peak estimate */
                    s_spki = s_spki + PT_ALPHA_SIGNAL * (cur - s_spki);

                    /* Record slope for next T-wave guard */
                    last_qrs_slope = s_deriv[n];
                    if (last_qrs_slope < 0.0f) last_qrs_slope = -last_qrs_slope;

                    last_peak_sample = n;
                }
                else
                {
                    /* T-wave classified as noise */
                    s_npki = s_npki + PT_ALPHA_NOISE * (cur - s_npki);
                }
            }
            else if (cur >= s_thr2)
            {
                /* Below primary threshold but above secondary: could be a
                 * missed QRS during a noisy stretch. In a full searchback
                 * implementation this would trigger a look-back; for this
                 * milestone we classify it as noise to keep the algorithm
                 * simple and avoid false positives. */
                s_npki = s_npki + PT_ALPHA_NOISE * (cur - s_npki);
            }
            else
            {
                /* Pure noise peak */
                s_npki = s_npki + PT_ALPHA_NOISE * (cur - s_npki);
            }

            /* Recompute thresholds after every local maximum */
            s_thr1 = s_npki + 0.25f * (s_spki - s_npki);
            s_thr2 = 0.5f * s_thr1;
        }
    }

    (void)in_candidate_region;   /* suppress unused-variable warning */
    (void)candidate_peak_val;
    (void)candidate_peak_idx;
}
