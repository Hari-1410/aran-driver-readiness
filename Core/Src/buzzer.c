/**
 ******************************************************************************
 * @file    buzzer.c
 * @brief   ARAN Milestone 6 – Buzzer alert patterns (GPIO toggle).
 ******************************************************************************
 */

#include "buzzer.h"

/* =========================================================================
 * Private helpers
 * ========================================================================= */

static void buzzer_on(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_SET);
}

static void buzzer_off(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_RESET);
}

static void beep(uint32_t on_ms, uint32_t off_ms)
{
    buzzer_on();
    HAL_Delay(on_ms);
    buzzer_off();
    HAL_Delay(off_ms);
}

/* =========================================================================
 * Public API
 * ========================================================================= */

void BUZZER_Init(void)
{
    buzzer_off();   /* ensure silent at startup */
}

void BUZZER_Alert(Fusion_Zone_t zone)
{
    switch (zone)
    {
        case FUSION_ZONE_GREEN:
            /* No sound */
            break;

        case FUSION_ZONE_YELLOW:
            /* Two gentle beeps (100 ms on / 120 ms off) – total ~440 ms */
            beep(100U, 120U);
            beep(100U,   0U);
            break;

        case FUSION_ZONE_RED:
            /* Four rapid beeps (80 ms on / 80 ms off) – total ~480 ms */
            beep(80U, 80U);
            beep(80U, 80U);
            beep(80U, 80U);
            beep(80U,  0U);
            break;

        default:
            break;
    }
}

void BUZZER_Off(void)
{
    buzzer_off();
}
