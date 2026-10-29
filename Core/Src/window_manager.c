/**
 ******************************************************************************
 * @file    window_manager.c
 * @brief   Implementation of the 2500-sample ECG window manager.
 ******************************************************************************
 */

#include "window_manager.h"
#include <string.h>

void WM_Init(EcgWindow_t *win)
{
    memset(win->data, 0, sizeof(win->data));
    win->fill_index = 0U;
    win->state = WINDOW_EMPTY;
    win->window_id = 0U;
}

void WM_Reset(EcgWindow_t *win)
{
    win->fill_index = 0U;
    win->state = WINDOW_FILLING;
}

bool WM_Update(CircularBuffer_t *cb, EcgWindow_t *win)
{
    float sample;
    bool window_completed = false;

    if (win->state == WINDOW_EMPTY)
    {
        win->state = WINDOW_FILLING;
    }

    /* Drain everything currently available in the circular buffer.
     * Reception continues via interrupt while we do this, so no
     * samples are lost even while a window is being processed
     * downstream (Section 9.4: "continuous reception while previous
     * windows are processed"). */
    while (win->state == WINDOW_FILLING && CB_Pop(cb, &sample))
    {
        win->data[win->fill_index++] = sample;

        if (win->fill_index >= WINDOW_SIZE)
        {
            win->state = WINDOW_READY;
            win->window_id++;
            window_completed = true;
        }
    }

    return window_completed;
}
