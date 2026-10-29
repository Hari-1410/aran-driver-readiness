/**
 ******************************************************************************
 * @file    oled_display.h
 * @brief   ARAN Milestone 6 – SSD1306 0.96" OLED display driver.
 *
 * Hardware:  SSD1306 128×64 monochrome OLED connected via I2C1.
 * I2C pins (STM32F446RE Nucleo default):
 *   PB6  → SCK (SCL)
 *   PB7  → SDA
 *   VDD  → 3.3 V
 *   GND  → GND
 * I2C address: 0x3C (SA0 tied LOW, the common default).
 *
 * Provides a 4-line live display:
 *   Line 0:  HR : <bpm> BPM
 *   Line 1:  CNN: <prob>
 *   Line 2:  DSI: <score>
 *   Line 3:  STATUS : ALERT | FATIGUE | DROWSY
 ******************************************************************************
 */
#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "fusion.h"
#include "hrv.h"
#include "cnn.h"

/* ---- I2C address (7-bit, left-shifted by HAL) ---- */
#define SSD1306_I2C_ADDR        (0x3C << 1)   /* 0x78 in 8-bit form */

/* ---- Panel dimensions ---- */
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64
#define SSD1306_PAGES           (SSD1306_HEIGHT / 8)   /* 8 pages */

/* ---- SSD1306 command byte constants ---- */
#define SSD1306_CMD_DISPLAY_OFF         0xAE
#define SSD1306_CMD_DISPLAY_ON          0xAF
#define SSD1306_CMD_SET_CONTRAST        0x81
#define SSD1306_CMD_ENTIRE_ON_RESUME    0xA4
#define SSD1306_CMD_NORMAL_DISPLAY      0xA6
#define SSD1306_CMD_SET_MUX_RATIO       0xA8
#define SSD1306_CMD_SET_DISPLAY_OFFSET  0xD3
#define SSD1306_CMD_SET_START_LINE      0x40
#define SSD1306_CMD_SEG_REMAP           0xA1   /* column 127 → SEG0  */
#define SSD1306_CMD_COM_SCAN_DEC        0xC8   /* reverse COM scan   */
#define SSD1306_CMD_SET_COM_PINS        0xDA
#define SSD1306_CMD_SET_DISPLAY_CLK     0xD5
#define SSD1306_CMD_SET_PRECHARGE       0xD9
#define SSD1306_CMD_SET_VCOM_DESELECT   0xDB
#define SSD1306_CMD_CHARGE_PUMP         0x8D
#define SSD1306_CMD_MEM_ADDR_MODE       0x20
#define SSD1306_CMD_SET_COL_ADDR        0x21
#define SSD1306_CMD_SET_PAGE_ADDR       0x22

/* ---- Public API ---- */

/**
 * @brief  Initialize the SSD1306 OLED panel.
 * @param  hi2c  Pointer to the HAL I2C handle (pass &hi2c1).
 * @retval HAL_OK on success, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef OLED_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Clear the display and the internal frame-buffer.
 */
void OLED_Clear(void);

/**
 * @brief  Flush the internal frame-buffer to the OLED panel.
 */
void OLED_Flush(void);

/**
 * @brief  Draw a null-terminated ASCII string at a given page row and
 *         column pixel position (0-based).
 * @param  page  Page row  [0 .. 7] — each page = 8 pixels tall.
 * @param  col   Column    [0 .. 127].
 * @param  str   ASCII string (printable characters only).
 */
void OLED_DrawString(uint8_t page, uint8_t col, const char *str);

/**
 * @brief  High-level update: refresh all 4 display lines from live data.
 *
 *   Line 0  →  "HR : <heartRate> BPM"
 *   Line 1  →  "CNN: <prob>"
 *   Line 2  →  "DSI: <score>"
 *   Line 3  →  "STATUS: <ALERT|EARLY FATIGUE|DROWSY>"
 *
 * @param  hrv     Pointer to the latest HRV feature structure.
 * @param  cnn     Pointer to the latest CNN result structure.
 * @param  fusion  Pointer to the latest Fusion result structure.
 */
void OLED_UpdateDisplay(const HRV_t           *hrv,
                        const CNN_Result_t    *cnn,
                        const Fusion_Result_t *fusion);

#ifdef __cplusplus
}
#endif
#endif /* OLED_DISPLAY_H */
