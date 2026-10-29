/**
 ******************************************************************************
 * @file    bluetooth.c
 * @brief   ARAN Milestone 6 – Bluetooth alert bridge (USART1 → ESP32).
 ******************************************************************************
 */

#include "bluetooth.h"
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Private state
 * ========================================================================= */
static UART_HandleTypeDef *s_huart = NULL;

/* =========================================================================
 * Public API
 * ========================================================================= */

void BT_Init(UART_HandleTypeDef *huart)
{
    s_huart = huart;
    /* USART1 is already initialized by MX_USART1_UART_Init() in main.c.
     * Nothing extra to do here; the handle just needs to be stored. */
}

void BT_SendAlert(const Fusion_Result_t *fusion,
                  const HRV_t           *hrv,
                  const CNN_Result_t    *cnn)
{
    if (!s_huart) { return; }

    /* Map zone enum to single-character code */
    char zone_ch;
    switch (fusion->zone)
    {
        case FUSION_ZONE_GREEN:  zone_ch = 'G'; break;
        case FUSION_ZONE_YELLOW: zone_ch = 'Y'; break;
        case FUSION_ZONE_RED:    zone_ch = 'R'; break;
        default:                 zone_ch = '-'; break;
    }

    char buf[64];
    int  len = snprintf(buf, sizeof(buf),
                        "BT,%c,%.1f,%.0f,%.3f\n",
                        zone_ch,
                        fusion->safetyIndex,
                        hrv->heartRate,
                        cnn->fatigueProbability);

    if (len > 0 && len < (int)sizeof(buf))
    {
        HAL_UART_Transmit(s_huart, (uint8_t *)buf, (uint16_t)len, HAL_MAX_DELAY);
    }
}
