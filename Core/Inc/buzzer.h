/**
 ******************************************************************************
 * @file    buzzer.h
 * @brief   ARAN Milestone 6 – Passive/active buzzer driver.
 *
 * Pin assignment (STM32F446RE Nucleo):
 *   PB0  →  Buzzer positive terminal  (GPIO_PIN_0, GPIOB)
 *            Already configured as GPIO_MODE_OUTPUT_PP in MX_GPIO_Init().
 *   GND  →  Buzzer negative terminal
 *
 * Alert patterns (spec Section 13 / 14.4):
 *   GREEN   – silent
 *   YELLOW  – intermittent double-beep every ~2 s
 *   RED     – rapid continuous beeping
 *
 * Usage:
 *   Call BUZZER_Alert(fusion->zone) once per processing window.
 *   The function runs the buzzer pattern BLOCKING within the call; the
 *   total blocking time is kept short (≤ 500 ms) so that the super-loop
 *   cadence (10 s window) is not disturbed.
 ******************************************************************************
 */
#ifndef BUZZER_H
#define BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "fusion.h"

/* ---- Hardware pin definition (matches MX_GPIO_Init) ---- */
#define BUZZER_GPIO_PORT    GPIOB
#define BUZZER_GPIO_PIN     GPIO_PIN_0

/* ---- Public API ---- */

/**
 * @brief  Initialize the buzzer GPIO (call once at startup).
 *         In practice the pin is already initialized by MX_GPIO_Init();
 *         this function just drives it LOW to ensure the buzzer is silent.
 */
void BUZZER_Init(void);

/**
 * @brief  Execute the buzzer alert pattern for the given DSI zone.
 *
 * @param  zone  FUSION_ZONE_GREEN  → silent
 *               FUSION_ZONE_YELLOW → two short beeps (100 ms each)
 *               FUSION_ZONE_RED    → four rapid beeps (80 ms each)
 */
void BUZZER_Alert(Fusion_Zone_t zone);

/**
 * @brief  Force the buzzer OFF immediately (e.g. on reset / Error_Handler).
 */
void BUZZER_Off(void);

#ifdef __cplusplus
}
#endif
#endif /* BUZZER_H */
