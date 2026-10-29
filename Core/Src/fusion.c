/**
 ******************************************************************************
 * @file    fusion.c
 * @brief   Decision Fusion + Driver Safety Index (DSI) for ARAN
 *          Milestone 5.
 *
 * ARAN : Milestone 5 - Decision Fusion and Driver Safety Index
 ******************************************************************************
 */

#include "fusion.h"
#include "ecg_uart.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* ---------------------------------------------------------------------- */
/* Tunable HRV normalization ranges                                       */
/* ---------------------------------------------------------------------- */
/* These are literature-typical resting-adult short-window values, NOT
 * derived from your dataset. Retune against your Python reference /
 * recorded baselines once you have ground truth to compare against —
 * these four constants are the only thing that should need to change. */
#define RMSSD_MIN_MS   20.0f   /* at/below this -> hrv contribution = 0 */
#define RMSSD_MAX_MS   60.0f   /* at/above this -> hrv contribution = 1 */
#define SDNN_MIN_MS    20.0f
#define SDNN_MAX_MS    70.0f

/* Fusion formula weights, per spec Section 13.4 */
#define FUSION_W_CNN   0.70f
#define FUSION_W_HRV   0.30f

/* Zone boundaries, per spec Section 13.5 */
#define DSI_GREEN_MIN   70.0f
#define DSI_YELLOW_MIN  40.0f

/* ---------------------------------------------------------------------- */
/* Internal helpers                                                        */
/* ---------------------------------------------------------------------- */

/**
 * @brief Linearly maps value into [0,1] given [min,max], clamped at
 *        both ends. Standard min-max normalization.
 */
static float32_t clamp01_linear(float32_t value, float32_t min, float32_t max)
{
    if (max <= min)
    {
        return 0.0f; /* degenerate range guard */
    }

    float32_t score = (value - min) / (max - min);

    if (score < 0.0f) score = 0.0f;
    if (score > 1.0f) score = 1.0f;

    return score;
}

/**
 * @brief Classifies a Driver Safety Index value into a zone, per
 *        spec Section 13.5 (Green 70-100, Yellow 40-69, Red 0-39).
 */
static Fusion_Zone_t classify_zone(float32_t dsi)
{
    if (dsi >= DSI_GREEN_MIN)
    {
        return FUSION_ZONE_GREEN;
    }
    else if (dsi >= DSI_YELLOW_MIN)
    {
        return FUSION_ZONE_YELLOW;
    }
    else
    {
        return FUSION_ZONE_RED;
    }
}

/* ---------------------------------------------------------------------- */
/* Public API                                                              */
/* ---------------------------------------------------------------------- */

void Fusion_Compute(const HRV_t *hrv, const CNN_Result_t *cnn,
                     Fusion_Result_t *out)
{
    memset(out, 0, sizeof(Fusion_Result_t));

    /* Fail loud, not silent: if either upstream stage didn't produce
     * usable data, report RED / invalid rather than a falsely
     * reassuring default. heartRate == 0 is HRV_Compute()'s own
     * "not enough valid RR intervals" sentinel (see hrv.c). */
    bool cnn_ok = (cnn != NULL) && cnn->valid;
    bool hrv_ok = (hrv != NULL) && (hrv->heartRate > 0.0f);

    if (!cnn_ok || !hrv_ok)
    {
        out->zone     = FUSION_ZONE_RED;
        out->valid    = false;
        return;
    }

    /* ---- Normalized HRV Score: 50/50 blend of RMSSD and SDNN ---- */
    float32_t rmssd_score = clamp01_linear(hrv->rmssd, RMSSD_MIN_MS, RMSSD_MAX_MS);
    float32_t sdnn_score  = clamp01_linear(hrv->sdnn,  SDNN_MIN_MS,  SDNN_MAX_MS);
    float32_t hrv_score   = 0.5f * rmssd_score + 0.5f * sdnn_score;

    /* ---- Weighted Decision Fusion, per spec Section 13.4 ---- */
    float32_t fatigue_prob = cnn->fatigueProbability;
    float32_t dsi = FUSION_W_CNN * (1.0f - fatigue_prob)
                  + FUSION_W_HRV * hrv_score;

    dsi *= 100.0f; /* scale 0-1 -> 0-100 */

    if (dsi < 0.0f)   dsi = 0.0f;
    if (dsi > 100.0f) dsi = 100.0f;

    out->hrvScore       = hrv_score;
    out->cnnProbability = fatigue_prob;
    out->safetyIndex    = dsi;
    out->zone           = classify_zone(dsi);
    out->valid          = true;
}

const char *Fusion_ZoneToString(Fusion_Zone_t zone)
{
    switch (zone)
    {
        case FUSION_ZONE_GREEN:  return "GREEN";
        case FUSION_ZONE_YELLOW: return "YELLOW";
        case FUSION_ZONE_RED:    return "RED";
        default:                 return "UNKNOWN";
    }
}

void Fusion_SendUART(uint32_t window_id, const Fusion_Result_t *result)
{
    char line[64];

    snprintf(line, sizeof(line), "D,WIN,%lu", (unsigned long)window_id);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "D,HRVSCORE,%.4f",
             (double)result->hrvScore);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "D,DSI,%.2f",
             (double)result->safetyIndex);
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "D,ZONE,%s",
             Fusion_ZoneToString(result->zone));
    ECG_UART_SendStatus(line);

    snprintf(line, sizeof(line), "D,VALID,%d", result->valid ? 1 : 0);
    ECG_UART_SendStatus(line);
}
