/**
 ******************************************************************************
 * @file    hrv.h
 * @brief   Heart Rate Variability (HRV) feature extraction for ARAN
 *          Milestone 3.
 *
 *   Consumes a PT_Result_t (R-peak index array) and computes the six HRV
 *   metrics required by the Decision Fusion module (Milestone 5):
 *
 *     heartRate  – average beats per minute
 *     meanRR     – mean RR interval in milliseconds
 *     sdnn       – standard deviation of RR intervals (ms)
 *     rmssd      – root mean square of successive RR differences (ms)
 *     nn50       – number of successive RR pairs differing by > 50 ms
 *     pnn50      – nn50 as a percentage of total successive RR pairs
 *
 *   All metrics are computed in floating-point; CMSIS-DSP helpers are
 *   used where they reduce code size (arm_mean_f32, arm_sqrt_f32).
 *
 * ARAN : Milestone 3 - Pan-Tompkins and HRV Feature Extraction
 ******************************************************************************
 */

#ifndef INC_HRV_H_
#define INC_HRV_H_

#include "pan_tompkins.h"   /* for PT_Result_t and PT_MAX_PEAKS */
#include "arm_math.h"
#include <stdint.h>

/**
 * @brief HRV feature vector.  Exposed directly to the Decision Fusion
 *        module; do not change field names or order without updating
 *        the fusion layer accordingly.
 */
typedef struct {
    float32_t heartRate;   /**< average HR in bpm                              */
    float32_t meanRR;      /**< mean RR interval in ms                         */
    float32_t sdnn;        /**< standard deviation of RR intervals (ms)        */
    float32_t rmssd;       /**< RMSSD – root mean square of successive diffs   */
    float32_t nn50;        /**< count of successive pairs with |ΔRR| > 50 ms   */
    float32_t pnn50;       /**< pNN50 – nn50 / total pairs × 100 (%)           */
} HRV_t;

/**
 * @brief Compute HRV features from a set of detected R-peak indices.
 *
 * @param result   Pointer to the PT_Result_t filled by PT_Detect().
 *                  Requires at least 2 peaks; fewer peaks produce zeroed output.
 * @param hrv      Pointer to caller-allocated HRV_t; all fields are written.
 */
void HRV_Compute(const PT_Result_t *result, HRV_t *hrv);

/**
 * @brief Transmit the full HRV feature set over USART1 in the
 *        "H,<field>,<value>" wire format so the Python visualization
 *        script can parse and display each metric separately.
 *
 *        Wire format (one line per metric):
 *          H,HR,<value>\r\n
 *          H,MRR,<value>\r\n
 *          H,SDNN,<value>\r\n
 *          H,RMSSD,<value>\r\n
 *          H,NN50,<value>\r\n
 *          H,PNN50,<value>\r\n
 *
 * @param window_id   Current window number (for log correlation).
 * @param hrv         Pointer to the populated HRV_t structure.
 */
void HRV_SendUART(uint32_t window_id, const HRV_t *hrv);

/**
 * @brief Transmit the R-peak indices and computed RR intervals over
 *        USART1 for real-time visualization / validation against the
 *        Python reference.
 *
 *        Wire format:
 *          P,<window_id>,<peak_index>\r\n       (one line per peak)
 *          I,<window_id>,<rr_interval_ms>\r\n   (one line per interval)
 *
 * @param window_id   Current window number.
 * @param result      Pointer to the PT_Result_t from PT_Detect().
 */
void HRV_SendPeaksAndIntervals(uint32_t window_id, const PT_Result_t *result);

#endif /* INC_HRV_H_ */
