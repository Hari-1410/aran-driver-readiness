/**
 ******************************************************************************
 * @file    ecg_uart.c
 * @brief   Implementation of the dual-UART ECG acquisition driver.
 ******************************************************************************
 */

#include "ecg_uart.h"
#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------------------- */
/* Module-private state                                                   */
/* ---------------------------------------------------------------------- */
static CircularBuffer_t s_ecg_buffer;

static UART_HandleTypeDef *s_huart_rx = NULL;  /* USART2 - ECG replay in  */
static UART_HandleTypeDef *s_huart_tx = NULL;  /* USART1 - visualization  */

/* Byte-wise reassembly state for the 4-byte little-endian float samples  */
static uint8_t  s_rx_byte;              /* single byte received by HAL IT */
static uint8_t  s_rx_assembly[ECG_SAMPLE_SIZE_BYTES];
static uint8_t  s_rx_assembly_idx = 0U;

static volatile bool s_uart_error_flag = false;

/* Running sample counter, used for status reporting */
static volatile uint32_t s_total_samples_received = 0U;

/* ---------------------------------------------------------------------- */
/* Public API                                                              */
/* ---------------------------------------------------------------------- */

void ECG_UART_Init(UART_HandleTypeDef *huart_rx, UART_HandleTypeDef *huart_tx)
{
    s_huart_rx = huart_rx;
    s_huart_tx = huart_tx;

    CB_Init(&s_ecg_buffer);
    s_rx_assembly_idx = 0U;
    s_total_samples_received = 0U;
    s_uart_error_flag = false;

    ECG_UART_SendStatus("ARAN Milestone 1 - UART Init OK");
    ECG_UART_SendStatus("USART2 armed for ECG reception @ 250 Hz");

    /* Arm interrupt-driven single-byte reception on USART2. HAL will call
     * HAL_UART_RxCpltCallback() every time one byte arrives; we reassemble
     * 4 such bytes into one float sample below. */
    HAL_UART_Receive_IT(s_huart_rx, &s_rx_byte, 1U);
}

CircularBuffer_t *ECG_UART_GetBuffer(void)
{
    return &s_ecg_buffer;
}

void ECG_UART_RxCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != s_huart_rx->Instance)
    {
        return; /* not our ECG input UART, ignore */
    }

    s_rx_assembly[s_rx_assembly_idx++] = s_rx_byte;

    if (s_rx_assembly_idx >= ECG_SAMPLE_SIZE_BYTES)
    {
        float sample;
        memcpy(&sample, s_rx_assembly, ECG_SAMPLE_SIZE_BYTES);
        s_rx_assembly_idx = 0U;

        if (!CB_Push(&s_ecg_buffer, sample))
        {
            /* Buffer full - this indicates the main loop is not draining
             * fast enough. Flag it; do NOT block here, we are in an ISR. */
            s_uart_error_flag = true;
        }
        s_total_samples_received++;
    }

    /* Immediately re-arm reception for the next byte so no incoming byte
     * is ever missed between samples (continuous streaming requirement). */
    HAL_UART_Receive_IT(s_huart_rx, &s_rx_byte, 1U);
}

void ECG_UART_SendRawSample(uint32_t window_id, uint32_t sample_index, float value)
{
    char line[64];
    int len = snprintf(line, sizeof(line), "R,%lu,%lu,%.4f\r\n",
                        (unsigned long)window_id,
                        (unsigned long)sample_index,
                        (double)value);

    if (len > 0 && s_huart_tx != NULL)
    {
        HAL_UART_Transmit(s_huart_tx, (uint8_t *)line, (uint16_t)len, 10U);
    }
}

void ECG_UART_SendFilteredSample(uint32_t window_id, uint32_t sample_index, float value)
{
    char line[64];
    int len = snprintf(line, sizeof(line), "F,%lu,%lu,%.4f\r\n",
                        (unsigned long)window_id,
                        (unsigned long)sample_index,
                        (double)value);

    if (len > 0 && s_huart_tx != NULL)
    {
        HAL_UART_Transmit(s_huart_tx, (uint8_t *)line, (uint16_t)len, 10U);
    }
}

void ECG_UART_SendStatus(const char *message)
{
    char line[96];
    int len = snprintf(line, sizeof(line), "S,%s\r\n", message);

    if (len > 0 && s_huart_tx != NULL)
    {
        HAL_UART_Transmit(s_huart_tx, (uint8_t *)line, (uint16_t)len, 10U);
    }
}

bool ECG_UART_HasError(void)
{
    if (s_uart_error_flag)
    {
        s_uart_error_flag = false;
        return true;
    }
    return false;
}

/* ---------------------------------------------------------------------- */
/* HAL error callback - catches framing/overrun/noise errors on either    */
/* UART and flags them for the status line.                               */
/* ---------------------------------------------------------------------- */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (s_huart_rx != NULL && huart->Instance == s_huart_rx->Instance)
    {
        s_uart_error_flag = true;

        /* Clear EVERY possible error flag, not just parity. On F4, an
         * uncleared overrun (ORE) in particular will cause the HAL to
         * keep re-entering this error callback on every subsequent byte,
         * silently killing all future reception. This is almost always
         * the real cause of "receives once, then goes silent forever". */
        __HAL_UART_CLEAR_PEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_IDLEFLAG(huart);

        /* Resync sample framing after an error so a partially-assembled
         * 4-byte float doesn't get corrupted by the glitch. */
        s_rx_assembly_idx = 0U;

        /* Re-arm reception. HAL_UART_Receive_IT() internally re-checks
         * huart->RxState and will restart cleanly even if the previous
         * transfer was aborted by the error. */
        HAL_UART_Receive_IT(s_huart_rx, &s_rx_byte, 1U);
    }
}
