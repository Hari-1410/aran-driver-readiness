/**
 ******************************************************************************
 * @file    circular_buffer.c
 * @brief   Implementation of the ECG sample circular buffer.
 *
 * NOTE ON CONCURRENCY:
 *   - CB_Push() is called from the USART2 RX interrupt (producer).
 *   - CB_Pop()  is called from the main super-loop (consumer).
 *   - head/tail/count are declared volatile and each field is only ever
 *     written by ONE side (head/count writes happen in Push, tail/count
 *     writes happen in Pop), so no additional locking is required on a
 *     single-core Cortex-M4 as long as 32-bit accesses stay atomic.
 ******************************************************************************
 */

#include "circular_buffer.h"
#include <string.h>

#define CB_MASK (CB_CAPACITY - 1U)

void CB_Init(CircularBuffer_t *cb)
{
    memset((void *)cb->buffer, 0, sizeof(cb->buffer));
    cb->head = 0U;
    cb->tail = 0U;
    cb->count = 0U;
    cb->overflow_count = 0U;
}

bool CB_Push(CircularBuffer_t *cb, float sample)
{
    if (cb->count >= CB_CAPACITY)
    {
        /* Buffer full: drop sample and record the overflow. This should
         * never happen in normal operation at 250 Hz given the window
         * manager drains the buffer well within real-time constraints. */
        cb->overflow_count++;
        return false;
    }

    cb->buffer[cb->head] = sample;
    cb->head = (cb->head + 1U) & CB_MASK;
    cb->count++;
    return true;
}

bool CB_Pop(CircularBuffer_t *cb, float *out_sample)
{
    if (cb->count == 0U)
    {
        return false;
    }

    *out_sample = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1U) & CB_MASK;
    cb->count--;
    return true;
}

uint32_t CB_Available(const CircularBuffer_t *cb)
{
    return cb->count;
}

bool CB_HasOverflowed(CircularBuffer_t *cb)
{
    if (cb->overflow_count > 0U)
    {
        cb->overflow_count = 0U;
        return true;
    }
    return false;
}
