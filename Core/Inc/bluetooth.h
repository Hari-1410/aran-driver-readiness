/**
 ******************************************************************************
 * @file    bluetooth.h
 * @brief   ARAN Milestone 6 – Bluetooth alert bridge via ESP32.
 *
 * Architecture
 * ─────────────
 * STM32 USART1 (PA9 TX / PA10 RX, 115200-8N1)
 *          ↓  UART  ↑
 * ESP32 UART2 (GPIO17 TX / GPIO16 RX)
 *          ↓  Bluetooth Classic SPP
 * Mobile phone / tablet
 *
 * Wire connections (3.3 V logic – both STM32 and ESP32 are 3.3 V):
 *   STM32 PA9  (USART1 TX) → ESP32 GPIO16 (UART2 RX)
 *   STM32 PA10 (USART1 RX) → ESP32 GPIO17 (UART2 TX)  [optional for ACK]
 *   STM32 GND               → ESP32 GND
 *   ESP32 powered from its own USB or 3.3 V rail.
 *
 * Protocol (STM32 → ESP32, newline-terminated ASCII):
 *   "BT,<zone>,<dsi>,<hr>,<prob>\n"
 *   zone  : G | Y | R | -
 *   dsi   : float (0–100)
 *   hr    : float (bpm)
 *   prob  : float (0–1)
 *
 * The ESP32 firmware (carma_esp32_bt.ino) forwards this line verbatim over
 * Bluetooth SPP and optionally plays a Text-to-Speech audio alert.
 ******************************************************************************
 */
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "fusion.h"
#include "hrv.h"
#include "cnn.h"

/* ---- Public API ---- */

/**
 * @brief  Initialize the Bluetooth bridge UART (USART1).
 * @param  huart  Pointer to the HAL UART handle for USART1 (pass &huart1).
 */
void BT_Init(UART_HandleTypeDef *huart);

/**
 * @brief  Transmit a status packet to the ESP32 / Bluetooth link.
 *
 * Sends: "BT,<G|Y|R|->,<dsi>,<hr>,<prob>\n"
 *
 * @param  fusion  Latest Fusion result.
 * @param  hrv     Latest HRV result (provides heart rate).
 * @param  cnn     Latest CNN result (provides fatigue probability).
 */
void BT_SendAlert(const Fusion_Result_t *fusion,
                  const HRV_t           *hrv,
                  const CNN_Result_t    *cnn);

#ifdef __cplusplus
}
#endif
#endif /* BLUETOOTH_H */
