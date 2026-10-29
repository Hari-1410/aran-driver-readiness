/**
 ******************************************************************************
 * @file    window_manager.h
 * @brief   Collects samples drained from the circular buffer into
 *          fixed-size 2500-sample (10 s @ 250 Hz) processing windows,
 *          matching the Python training pipeline (Section 5.1 of the spec).
 *
 * ARAN : Milestone 1 - ECG Signal Acquisition and Validation
 ******************************************************************************
 */

#ifndef INC_WINDOW_MANAGER_H_
#define INC_WINDOW_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "circular_buffer.h"

#define WINDOW_SIZE   2500U   /* 10 seconds @ 250 Hz */

typedef enum {
    WINDOW_EMPTY = 0,
    WINDOW_FILLING,
    WINDOW_READY      /* full 2500 samples collected, ready for DSP stage */
} WindowState_t;

typedef struct {
    float          data[WINDOW_SIZE];
    uint32_t       fill_index;    /* how many samples currently written   */
    WindowState_t  state;
    uint32_t       window_id;     /* incremented every time a window completes */
} EcgWindow_t;

/**
 * @brief Initialize the window manager state.
 */
void WM_Init(EcgWindow_t *win);

/**
 * @brief Drain as many samples as are available from the circular buffer
 *        into the current window. Call this periodically from the main
 *        super-loop (Milestone 1 does not yet consume the window contents
 *        beyond forwarding raw samples on USART1; later milestones will
 *        pass win->data to the DSP stage once state == WINDOW_READY).
 *
 * @param cb   circular buffer fed by the USART2 RX ISR
 * @param win  window structure to fill
 * @return true if a window has just become READY on this call
 */
bool WM_Update(CircularBuffer_t *cb, EcgWindow_t *win);

/**
 * @brief Reset the window to start accumulating the next one. Must be
 *        called after a READY window has been consumed downstream.
 */
void WM_Reset(EcgWindow_t *win);

#endif /* INC_WINDOW_MANAGER_H_ */
