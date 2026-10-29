/**
 ******************************************************************************
 * @file    circular_buffer.h
 * @brief   Generic lock-free (single-producer/single-consumer) circular
 *          buffer used to hold incoming ECG samples received via UART
 *          interrupt before they are consumed by the window manager.
 *
 * ARAN : Milestone 1 - ECG Signal Acquisition and Validation
 ******************************************************************************
 */

#ifndef INC_CIRCULAR_BUFFER_H_
#define INC_CIRCULAR_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

/* Must be a power of two to allow cheap masking instead of modulo */
#define CB_CAPACITY   4096U

typedef struct {
    float    buffer[CB_CAPACITY];
    volatile uint32_t head;      /* next write index (producer: ISR)      */
    volatile uint32_t tail;      /* next read index  (consumer: main/task)*/
    volatile uint32_t count;     /* number of samples currently stored    */
    volatile uint32_t overflow_count; /* number of samples dropped        */
} CircularBuffer_t;

/**
 * @brief Initialize / reset the circular buffer.
 */
void CB_Init(CircularBuffer_t *cb);

/**
 * @brief Push one sample into the buffer. Called from the UART RX ISR.
 * @return true if the sample was stored, false if the buffer was full
 *         (sample dropped, overflow_count incremented).
 */
bool CB_Push(CircularBuffer_t *cb, float sample);

/**
 * @brief Pop one sample from the buffer. Called from main loop / task context.
 * @return true if a sample was retrieved, false if buffer was empty.
 */
bool CB_Pop(CircularBuffer_t *cb, float *out_sample);

/**
 * @brief Number of samples currently available to read.
 */
uint32_t CB_Available(const CircularBuffer_t *cb);

/**
 * @brief Returns true if the buffer has overflowed since last check and
 *        clears the flag.
 */
bool CB_HasOverflowed(CircularBuffer_t *cb);

#endif /* INC_CIRCULAR_BUFFER_H_ */
