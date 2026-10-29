/**
 ******************************************************************************
 * @file    ecg_uart.h
 * @brief   Dual-UART driver for ARAN Milestone 1.
 *
 *   USART2 (ST-Link VCP) : ECG sample RECEPTION ONLY, 250 Hz, from the
 *                           Python replay script. No debug traffic allowed
 *                           on this port (spec section 7.6).
 *   USART1 (PA9 -> FTDI)  : Raw ECG + status message TRANSMISSION for the
 *                           PC visualization script.
 *
 * ARAN : Milestone 1 - ECG Signal Acquisition and Validation
 ******************************************************************************
 */

#ifndef INC_ECG_UART_H_
#define INC_ECG_UART_H_

#include "stm32f4xx_hal.h"
#include "circular_buffer.h"
#include <stdint.h>
#include <stdbool.h>

/* ---------------------------------------------------------------------- */
/*  Wire protocol (USART2, PC -> STM32)                                    */
/* ---------------------------------------------------------------------- */
/*  Each ECG sample is sent as a 4-byte little-endian IEEE-754 float,      */
/*  i.e. exactly the layout produced by Python's struct.pack('<f', value). */
#define ECG_SAMPLE_SIZE_BYTES   4U

/* ---------------------------------------------------------------------- */
/*  Wire protocol (USART1, STM32 -> PC)                                    */
/* ---------------------------------------------------------------------- */
/*  Milestone 1 only ever transmits RAW ECG frames:                        */
/*     "R,<window_id>,<sample_index>,<value>\r\n"                          */
/*  Status/debug lines are transmitted as plain ASCII text prefixed with   */
/*  "S,":                                                                  */
/*     "S,<message>\r\n"                                                   */

/**
 * @brief Initialize the ECG UART module: resets the circular buffer,
 *        arms the USART2 byte-wise interrupt reception, and sends an
 *        initial status message over USART1.
 *
 * @param huart_rx  handle to USART2 (ECG replay input)
 * @param huart_tx  handle to USART1 (visualization / debug output)
 */
void ECG_UART_Init(UART_HandleTypeDef *huart_rx, UART_HandleTypeDef *huart_tx);

/**
 * @brief Must be called from HAL_UART_RxCpltCallback() when the interrupt
 *        fires on USART2. Reassembles 4-byte float samples and pushes
 *        completed samples into the circular buffer, then re-arms
 *        reception for the next byte/sample.
 */
void ECG_UART_RxCallback(UART_HandleTypeDef *huart);

/**
 * @brief Retrieve the circular buffer instance owned by this module.
 */
CircularBuffer_t *ECG_UART_GetBuffer(void);

/**
 * @brief Transmit one raw ECG sample over USART1 for visualization.
 *        Blocking transmit (HAL_UART_Transmit) is acceptable here since
 *        the payload is short and USART1 has no reception duties.
 *
 * @param window_id     current window number
 * @param sample_index  index of the sample within the current window
 * @param value         ECG sample value
 */
void ECG_UART_SendRawSample(uint32_t window_id, uint32_t sample_index, float value);

/**
 * @brief Transmit one FILTERED (post Notch->Bandpass->Normalization) ECG
 *        sample, using the "F,<window_id>,<sample_index>,<value>" wire
 *        format so the PC visualization script can distinguish it from
 *        raw samples ("R,...") on the same link (Milestone 2).
 */
void ECG_UART_SendFilteredSample(uint32_t window_id, uint32_t sample_index, float value);

/**
 * @brief Transmit a human-readable status/debug message over USART1.
 *        e.g. "Window 3 READY", "Buffer overflow detected", etc.
 */
void ECG_UART_SendStatus(const char *message);

/**
 * @brief Returns true exactly once if a UART reception error (framing,
 *        overrun, noise) occurred since the last check.
 */
bool ECG_UART_HasError(void);

#endif /* INC_ECG_UART_H_ */
