/**
 ******************************************************************************
 * @file    hrv.c
 * @brief   HRV feature extraction and UART reporting for ARAN M3.
 *
 *   All RR intervals are converted to milliseconds before feature
 *   computation so the output units match the clinical definitions:
 *
 *     RR_ms[i] = (peak_indices[i+1] - peak_indices[i]) * (1000 / Fs)
 *              = (peak_indices[i+1] - peak_indices[i]) * 4.0   [@ 250 Hz]
 *
 *   Before feature computation, raw RR intervals are passed through a
 *   physiological validity filter:
 *     - Reject RR < 300 ms   (> 200 bpm, implausible)
 *     - Reject RR > 2000 ms  (< 30 bpm, implausible / missed-beat gap)
 *     - Reject RR that deviates from the previous *valid* RR by more
 *       than 20% (ectopic beat / detector glitch guard)
 *   This prevents a single missed or spurious peak from corrupting
 *   SDNN/RMSSD, which are both sensitive to outliers.
 *
 *   Features (computed on the VALIDATED RR set):
 *     heartRate = 60000 / mean(RR_ms)                 [bpm]
 *     meanRR    = mean(RR_ms)                          [ms]
 *     sdnn      = std(RR_ms)   (population, ddof=0)    [ms]
 *     rmssd     = sqrt(mean(diff(RR_ms)^2))            [ms]
 *     nn50      = count(|RR_ms[i+1]-RR_ms[i]| > 50)   [count]
 *     pnn50     = 100 * nn50 / (num_intervals - 1)     [%]
 *
 * ARAN : Milestone 3 - Pan-Tompkins and HRV Feature Extraction
 ******************************************************************************
 */

#include "hrv.h"
#include "ecg_uart.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ms per sample at 250 Hz */
#define HRV_MS_PER_SAMPLE  4.0f

/* Maximum RR intervals per window: PT_MAX_PEAKS - 1 */
#define HRV_MAX_RR  (PT_MAX_PEAKS - 1U)

/* ---------------------------------------------------------------------- */
/* RR interval validation thresholds                                       */
/* ---------------------------------------------------------------------- */
#define RR_MIN_MS         300.0f   /* 200 bpm ceiling */
#define RR_MAX_MS        2000.0f   /* 30 bpm floor */
#define RR_OUTLIER_FRAC     0.20f  /* max 20% jump from previous valid RR */
#define HRV_MIN_VALID_RR    2U     /* need >=2 valid RR to compute anything */

/* ---------------------------------------------------------------------- */
/* Static scratch (avoids stack pressure in a resource-constrained MCU)   */
/* ---------------------------------------------------------------------- */
static float32_t s_rr_ms[HRV_MAX_RR];       /* raw RR intervals in ms       */
static float32_t s_rr_clean[HRV_MAX_RR];    /* validated RR intervals       */
static float32_t s_rr_diff[HRV_MAX_RR];     /* successive RR differences    */

/* ---------------------------------------------------------------------- */
/* Internal helpers                                                        */
/* ---------------------------------------------------------------------- */

/**
 * @brief Filters raw RR intervals (ms) into a physiologically valid buffer.
 *
 *        Rejects intervals outside [RR_MIN_MS, RR_MAX_MS] outright, and
 *        rejects intervals that deviate from the previous *accepted*
 *        interval by more than RR_OUTLIER_FRAC. Rejected samples are
 *        simply dropped (not interpolated), so num_diffs downstream is
 *        based on the cleaned sequence, not the raw one.
 *
 * @param raw_rr      Input RR intervals (ms)
 * @param raw_count   Number of raw RR intervals
 * @param clean_rr    Output buffer for valid RR intervals
 * @param clean_count Output: number of valid RR intervals
 */
static void RR_Validate(const float32_t *raw_rr, uint32_t raw_count,
                          float32_t *clean_rr, uint32_t *clean_count)
{
    uint32_t  out_idx = 0U;
    float32_t prev_valid_rr = 0.0f;

    for (uint32_t i = 0U; i < raw_count; i++)
    {
        float32_t rr = raw_rr[i];

        /* Hard physiological bounds */
        if (rr < RR_MIN_MS || rr > RR_MAX_MS)
        {
            continue; /* reject outright */
        }

        /* Relative outlier check against previous *valid* RR */
        if (prev_valid_rr > 0.0f)
        {
            float32_t delta = rr - prev_valid_rr;
            if (delta < 0.0f) delta = -delta;

            if (delta > RR_OUTLIER_FRAC * prev_valid_rr)
            {
                continue; /* ectopic beat / glitch: drop */
            }
        }

        clean_rr[out_idx++] = rr;
        prev_valid_rr = rr;
    }

    *clean_count = out_idx;
}

/* ---------------------------------------------------------------------- */
/* Public API                                                              */
/* ---------------------------------------------------------------------- */

void HRV_Compute(const PT_Result_t *result, HRV_t *hrv)
{
    /* Zero output so any early-return leaves a well-defined struct */
    memset(hrv, 0, sizeof(HRV_t));

    if (result == NULL || result->peak_count < 2U)
    {
        /* Not enough peaks to compute any interval-based metric */
        return;
    }

    uint32_t num_intervals = result->peak_count - 1U;

    /* ------------------------------------------------------------------ */
    /* Build raw RR interval array in milliseconds                         */
    /* ------------------------------------------------------------------ */
    for (uint32_t i = 0U; i < num_intervals; i++)
    {
        float32_t delta_samples = (float32_t)(result->peak_indices[i + 1U]
                                            - result->peak_indices[i]);
        s_rr_ms[i] = delta_samples * HRV_MS_PER_SAMPLE;
    }

    /* ------------------------------------------------------------------ */
    /* Validate RR intervals before any feature computation                */
    /* ------------------------------------------------------------------ */
    uint32_t clean_count = 0U;
    RR_Validate(s_rr_ms, num_intervals, s_rr_clean, &clean_count);

    if (clean_count < HRV_MIN_VALID_RR)
    {
        /* Not enough physiologically valid RR intervals survived
         * filtering; leave hrv zeroed rather than reporting a
         * meaningless (or single-sample) HRV value. */
        return;
    }

    /* From this point on, all computation uses s_rr_clean/clean_count,
     * not the raw RR set. */

    /* ------------------------------------------------------------------ */
    /* Heart Rate and Mean RR                                              */
    /* ------------------------------------------------------------------ */
    float32_t mean_rr;
    arm_mean_f32(s_rr_clean, clean_count, &mean_rr);

    hrv->meanRR    = mean_rr;
    hrv->heartRate = (mean_rr > 0.0f) ? (60000.0f / mean_rr) : 0.0f;

    /* ------------------------------------------------------------------ */
    /* SDNN (population standard deviation)                               */
    /* ------------------------------------------------------------------ */
    float32_t sum_sq_diff = 0.0f;
    for (uint32_t i = 0U; i < clean_count; i++)
    {
        float32_t diff = s_rr_clean[i] - mean_rr;
        sum_sq_diff += diff * diff;
    }
    float32_t variance = sum_sq_diff / (float32_t)clean_count;
    float32_t sdnn_val;
    arm_sqrt_f32(variance, &sdnn_val);
    hrv->sdnn = sdnn_val;

    /* ------------------------------------------------------------------ */
    /* RMSSD and NN50 / pNN50                                              */
    /* ------------------------------------------------------------------ */
    if (clean_count < 2U)
    {
        /* Need at least 2 valid intervals for successive differences.
         * (Already guarded above by HRV_MIN_VALID_RR, kept here as a
         * defensive check in case that constant is ever lowered.) */
        hrv->rmssd = 0.0f;
        hrv->nn50  = 0.0f;
        hrv->pnn50 = 0.0f;
        return;
    }

    uint32_t num_diffs  = clean_count - 1U;
    uint32_t nn50_count = 0U;
    float32_t sum_sq_successive = 0.0f;

    for (uint32_t i = 0U; i < num_diffs; i++)
    {
        float32_t d = s_rr_clean[i + 1U] - s_rr_clean[i];
        s_rr_diff[i] = d;
        sum_sq_successive += d * d;

        /* NN50: absolute difference greater than 50 ms */
        if (d < 0.0f) d = -d;
        if (d > 50.0f)
        {
            nn50_count++;
        }
    }

    float32_t rmssd_val;
    arm_sqrt_f32(sum_sq_successive / (float32_t)num_diffs, &rmssd_val);

    hrv->rmssd = rmssd_val;
    hrv->nn50  = (float32_t)nn50_count;
    hrv->pnn50 = (num_diffs > 0U)
                 ? (100.0f * (float32_t)nn50_count / (float32_t)num_diffs)
                 : 0.0f;
}

void HRV_SendUART(uint32_t window_id, const HRV_t *hrv)
{
    char line[80];

    int len;

    /* One line per metric; prefix "H" distinguishes HRV lines from
     * raw ("R"), filtered ("F"), peaks ("P"), and intervals ("I"). */

    len = snprintf(line, sizeof(line), "H,HR,%.2f\r\n",
                   (double)hrv->heartRate);
    ECG_UART_SendStatus(line);   /* reuse SendStatus for the "S," wrapper */

    /* Use direct H-frame format via a small helper that matches the
     * wire format documented in hrv.h.  We call ECG_UART_SendStatus()
     * with the already-formatted body because the function prepends
     * "S," and appends "\r\n"; to emit the raw H-frame we therefore
     * need to transmit directly.  If your build has a raw-transmit
     * helper you can replace these calls.  For Milestone 3 validation
     * the "S," prefix on HRV lines is acceptable because the Python
     * script parses them from the status channel. */

    /* The approach below emits each metric as a status message with a
     * structured body: "H,<tag>,<value>" so the Python parser can
     * detect the "H," prefix inside the status payload. */
    snprintf(line, sizeof(line), "H,WIN,%lu", (unsigned long)window_id);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "H,HR,%.2f",   (double)hrv->heartRate);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "H,MRR,%.2f",  (double)hrv->meanRR);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "H,SDNN,%.2f", (double)hrv->sdnn);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "H,RMSSD,%.2f",(double)hrv->rmssd);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "H,NN50,%.0f", (double)hrv->nn50);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "H,PNN50,%.2f",(double)hrv->pnn50);
    ECG_UART_SendStatus(line);
}

void HRV_SendPeaksAndIntervals(uint32_t window_id, const PT_Result_t *result)
{
    char line[64];

    if (result == NULL || result->peak_count == 0U)
    {
        return;
    }

    /* Transmit each R-peak index */
    for (uint32_t i = 0U; i < result->peak_count; i++)
    {
        snprintf(line, sizeof(line), "P,%lu,%lu",
                 (unsigned long)window_id,
                 (unsigned long)result->peak_indices[i]);
        ECG_UART_SendStatus(line);
    }

    /* Transmit each RR interval in ms (RAW, pre-validation — this
     * stream is for debugging/plotting against the Python reference,
     * so it intentionally shows the detector's unfiltered output). */
    for (uint32_t i = 0U; i + 1U < result->peak_count; i++)
    {
        float32_t rr_ms = (float32_t)(result->peak_indices[i + 1U]
                                    - result->peak_indices[i])
                          * HRV_MS_PER_SAMPLE;

        snprintf(line, sizeof(line), "I,%lu,%.1f",
                 (unsigned long)window_id,
                 (double)rr_ms);
        ECG_UART_SendStatus(line);
    }
}
