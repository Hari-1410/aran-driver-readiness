/**
 ******************************************************************************
 * @file    fusion.h
 * @brief   Decision Fusion + Driver Safety Index (DSI) for ARAN
 *          Milestone 5.
 *
 *   Combines the Milestone 4 CNN fatigue probability with the
 *   Milestone 3 HRV feature vector into a single Driver Safety Index
 *   (0-100) and a three-zone alert classification (Section 13.5).
 *
 *   ------------------------------------------------------------------
 *   DESIGN NOTE — "Normalized HRV Score" (spec Section 13.4)
 *   ------------------------------------------------------------------
 *   The spec's fusion formula:
 *
 *       DSI = 0.70 * (1 - CNN Probability) + 0.30 * Normalized HRV Score
 *
 *   does not define how the 6-field HRV_t vector collapses into one
 *   "Normalized HRV Score." This implementation uses ONLY RMSSD and
 *   SDNN (each min-max clamped against literature-typical resting
 *   ranges, then averaged 50/50). Heart rate is deliberately excluded
 *   because both bradycardia and tachycardia indicate risk in opposite
 *   directions relative to a "normal" band, which doesn't reduce
 *   cleanly to a single 0-1 "higher is better" score, and the CNN
 *   channel already sees the raw ECG (including rate information).
 *
 *   The four range constants below (RMSSD_MIN/MAX_MS, SDNN_MIN/MAX_MS)
 *   are the only thing you need to retune against your own dataset;
 *   everything else in the fusion math is fixed by the spec.
 *
 *   No Adaptive Personalized Edge Baseline (APEB) yet (Section 13.3) —
 *   Fusion_Compute() takes fixed ranges, matching "not part of the
 *   current prototype." The struct/function boundary is kept clean so
 *   a future APEB module can swap in per-driver ranges without
 *   changing this file's public API.
 *
 * ARAN : Milestone 5 - Decision Fusion and Driver Safety Index
 ******************************************************************************
 */

#ifndef INC_FUSION_H_
#define INC_FUSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"
#include "hrv.h"
#include "cnn.h"
#include <stdint.h>

/**
 * @brief Driver alert zone, per spec Section 13.5.
 */
typedef enum
{
    FUSION_ZONE_GREEN  = 0,  /**< DSI 70-100 : Driver Alert, no warning        */
    FUSION_ZONE_YELLOW = 1,  /**< DSI 40-69  : Early Fatigue, warning + beep   */
    FUSION_ZONE_RED    = 2   /**< DSI 0-39   : Driver Drowsy, continuous alert */
} Fusion_Zone_t;

/**
 * @brief Complete Decision Fusion output for one 10-second window.
 *        Consumed directly by Milestone 6 (OLED / Bluetooth / Buzzer).
 */
typedef struct
{
    float32_t     hrvScore;           /**< Normalized HRV score, 0.0-1.0        */
    float32_t     cnnProbability;     /**< Passed through from CNN_Result_t     */
    float32_t     safetyIndex;        /**< Driver Safety Index, 0.0-100.0       */
    Fusion_Zone_t zone;                /**< GREEN / YELLOW / RED classification */
    bool          valid;               /**< false if inputs were unusable       */
} Fusion_Result_t;

/**
 * @brief Computes the Driver Safety Index from CNN + HRV outputs.
 *
 *        If cnn->valid is false, or hrv has no usable RR intervals
 *        (heartRate == 0, i.e. HRV_Compute() bailed out early), the
 *        result is marked invalid and safetyIndex/zone default to the
 *        safest assumption (RED / 0) so a downstream UI never displays
 *        a stale "all clear" on bad input — it fails loud, not silent.
 *
 * @param hrv     Pointer to the HRV_t populated by HRV_Compute().
 * @param cnn     Pointer to the CNN_Result_t populated by CNN_Run().
 * @param out     Output: caller-allocated Fusion_Result_t, fully written.
 */
void Fusion_Compute(const HRV_t *hrv, const CNN_Result_t *cnn,
                     Fusion_Result_t *out);

/**
 * @brief Returns a short human-readable zone label ("GREEN", "YELLOW",
 *        "RED"), used both for UART reporting and (later) OLED text.
 */
const char *Fusion_ZoneToString(Fusion_Zone_t zone);

/**
 * @brief Transmits the fusion result over USART1 in the same
 *        "S,<body>" status-line style as HRV_SendUART()/CNN_SendUART(),
 *        so the existing Python status parser needs only a new tag
 *        branch ("D,") rather than a new frame type.
 *
 *        Wire format (one line per field):
 *          D,WIN,<window_id>
 *          D,HRVSCORE,<hrvScore>
 *          D,DSI,<safetyIndex>
 *          D,ZONE,<GREEN|YELLOW|RED>
 *          D,VALID,<0|1>
 */
void Fusion_SendUART(uint32_t window_id, const Fusion_Result_t *result);

#endif /* INC_FUSION_H_ */
