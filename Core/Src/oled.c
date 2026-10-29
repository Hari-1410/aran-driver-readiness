/**
 ******************************************************************************
 * @file    oled_display.c
 * @brief   ARAN Milestone 6 – SSD1306 0.96" OLED driver (I2C, 128×64).
 *
 * Design notes
 * ─────────────
 * • All SSD1306 commands are issued via a 2-byte I2C write: the first byte
 *   is the control byte (0x00 = command, 0x40 = data), the second is the
 *   payload.  For a multi-byte data burst a single control byte 0x40 is
 *   followed by the entire frame-buffer in one HAL_I2C_Master_Transmit call.
 *
 * • A 1-KB static frame-buffer (128 × 64 / 8) is maintained in RAM.
 *   Rendering targets this buffer; OLED_Flush() DMA-copies it to the panel.
 *
 * • Character rendering uses a built-in 5×7 monospace font (ASCII 0x20–0x7E).
 *   Each glyph is stored as 5 column bytes (1 bit per pixel, MSB = bottom).
 *   A 1-pixel column spacer is appended automatically → effective width = 6 px.
 *
 * • OLED_UpdateDisplay() composes the four-line status screen and flushes in
 *   one call – safe to invoke once per 10-second processing window.
 ******************************************************************************
 */

#include "oled.h"
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * 5×7 monospace font  (ASCII 0x20 ' ' … 0x7E '~')
 * Each entry: 5 column bytes, bit0 = top row, bit6 = bottom row.
 * ========================================================================= */
static const uint8_t s_font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* ' ' */
    {0x00,0x00,0x5F,0x00,0x00}, /* '!' */
    {0x00,0x07,0x00,0x07,0x00}, /* '"' */
    {0x14,0x7F,0x14,0x7F,0x14}, /* '#' */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* '$' */
    {0x23,0x13,0x08,0x64,0x62}, /* '%' */
    {0x36,0x49,0x55,0x22,0x50}, /* '&' */
    {0x00,0x05,0x03,0x00,0x00}, /* ''' */
    {0x00,0x1C,0x22,0x41,0x00}, /* '(' */
    {0x00,0x41,0x22,0x1C,0x00}, /* ')' */
    {0x08,0x2A,0x1C,0x2A,0x08}, /* '*' */
    {0x08,0x08,0x3E,0x08,0x08}, /* '+' */
    {0x00,0x50,0x30,0x00,0x00}, /* ',' */
    {0x08,0x08,0x08,0x08,0x08}, /* '-' */
    {0x00,0x60,0x60,0x00,0x00}, /* '.' */
    {0x20,0x10,0x08,0x04,0x02}, /* '/' */
    {0x3E,0x51,0x49,0x45,0x3E}, /* '0' */
    {0x00,0x42,0x7F,0x40,0x00}, /* '1' */
    {0x42,0x61,0x51,0x49,0x46}, /* '2' */
    {0x21,0x41,0x45,0x4B,0x31}, /* '3' */
    {0x18,0x14,0x12,0x7F,0x10}, /* '4' */
    {0x27,0x45,0x45,0x45,0x39}, /* '5' */
    {0x3C,0x4A,0x49,0x49,0x30}, /* '6' */
    {0x01,0x71,0x09,0x05,0x03}, /* '7' */
    {0x36,0x49,0x49,0x49,0x36}, /* '8' */
    {0x06,0x49,0x49,0x29,0x1E}, /* '9' */
    {0x00,0x36,0x36,0x00,0x00}, /* ':' */
    {0x00,0x56,0x36,0x00,0x00}, /* ';' */
    {0x08,0x14,0x22,0x41,0x00}, /* '<' */
    {0x14,0x14,0x14,0x14,0x14}, /* '=' */
    {0x00,0x41,0x22,0x14,0x08}, /* '>' */
    {0x02,0x01,0x51,0x09,0x06}, /* '?' */
    {0x32,0x49,0x79,0x41,0x3E}, /* '@' */
    {0x7E,0x11,0x11,0x11,0x7E}, /* 'A' */
    {0x7F,0x49,0x49,0x49,0x36}, /* 'B' */
    {0x3E,0x41,0x41,0x41,0x22}, /* 'C' */
    {0x7F,0x41,0x41,0x22,0x1C}, /* 'D' */
    {0x7F,0x49,0x49,0x49,0x41}, /* 'E' */
    {0x7F,0x09,0x09,0x09,0x01}, /* 'F' */
    {0x3E,0x41,0x49,0x49,0x7A}, /* 'G' */
    {0x7F,0x08,0x08,0x08,0x7F}, /* 'H' */
    {0x00,0x41,0x7F,0x41,0x00}, /* 'I' */
    {0x20,0x40,0x41,0x3F,0x01}, /* 'J' */
    {0x7F,0x08,0x14,0x22,0x41}, /* 'K' */
    {0x7F,0x40,0x40,0x40,0x40}, /* 'L' */
    {0x7F,0x02,0x04,0x02,0x7F}, /* 'M' */
    {0x7F,0x04,0x08,0x10,0x7F}, /* 'N' */
    {0x3E,0x41,0x41,0x41,0x3E}, /* 'O' */
    {0x7F,0x09,0x09,0x09,0x06}, /* 'P' */
    {0x3E,0x41,0x51,0x21,0x5E}, /* 'Q' */
    {0x7F,0x09,0x19,0x29,0x46}, /* 'R' */
    {0x46,0x49,0x49,0x49,0x31}, /* 'S' */
    {0x01,0x01,0x7F,0x01,0x01}, /* 'T' */
    {0x3F,0x40,0x40,0x40,0x3F}, /* 'U' */
    {0x1F,0x20,0x40,0x20,0x1F}, /* 'V' */
    {0x3F,0x40,0x38,0x40,0x3F}, /* 'W' */
    {0x63,0x14,0x08,0x14,0x63}, /* 'X' */
    {0x07,0x08,0x70,0x08,0x07}, /* 'Y' */
    {0x61,0x51,0x49,0x45,0x43}, /* 'Z' */
    {0x00,0x7F,0x41,0x41,0x00}, /* '[' */
    {0x02,0x04,0x08,0x10,0x20}, /* '\' */
    {0x00,0x41,0x41,0x7F,0x00}, /* ']' */
    {0x04,0x02,0x01,0x02,0x04}, /* '^' */
    {0x40,0x40,0x40,0x40,0x40}, /* '_' */
    {0x00,0x01,0x02,0x04,0x00}, /* '`' */
    {0x20,0x54,0x54,0x54,0x78}, /* 'a' */
    {0x7F,0x48,0x44,0x44,0x38}, /* 'b' */
    {0x38,0x44,0x44,0x44,0x20}, /* 'c' */
    {0x38,0x44,0x44,0x48,0x7F}, /* 'd' */
    {0x38,0x54,0x54,0x54,0x18}, /* 'e' */
    {0x08,0x7E,0x09,0x01,0x02}, /* 'f' */
    {0x0C,0x52,0x52,0x52,0x3E}, /* 'g' */
    {0x7F,0x08,0x04,0x04,0x78}, /* 'h' */
    {0x00,0x44,0x7D,0x40,0x00}, /* 'i' */
    {0x20,0x40,0x44,0x3D,0x00}, /* 'j' */
    {0x7F,0x10,0x28,0x44,0x00}, /* 'k' */
    {0x00,0x41,0x7F,0x40,0x00}, /* 'l' */
    {0x7C,0x04,0x18,0x04,0x78}, /* 'm' */
    {0x7C,0x08,0x04,0x04,0x78}, /* 'n' */
    {0x38,0x44,0x44,0x44,0x38}, /* 'o' */
    {0x7C,0x14,0x14,0x14,0x08}, /* 'p' */
    {0x08,0x14,0x14,0x18,0x7C}, /* 'q' */
    {0x7C,0x08,0x04,0x04,0x08}, /* 'r' */
    {0x48,0x54,0x54,0x54,0x20}, /* 's' */
    {0x04,0x3F,0x44,0x40,0x20}, /* 't' */
    {0x3C,0x40,0x40,0x40,0x7C}, /* 'u' */
    {0x1C,0x20,0x40,0x20,0x1C}, /* 'v' */
    {0x3C,0x40,0x30,0x40,0x3C}, /* 'w' */
    {0x44,0x28,0x10,0x28,0x44}, /* 'x' */
    {0x0C,0x50,0x50,0x50,0x3C}, /* 'y' */
    {0x44,0x64,0x54,0x4C,0x44}, /* 'z' */
    {0x00,0x08,0x36,0x41,0x00}, /* '{' */
    {0x00,0x00,0x7F,0x00,0x00}, /* '|' */
    {0x00,0x41,0x36,0x08,0x00}, /* '}' */
    {0x08,0x04,0x08,0x10,0x08}, /* '~' */
};

/* =========================================================================
 * Private state
 * ========================================================================= */
static I2C_HandleTypeDef *s_hi2c  = NULL;
static uint8_t            s_framebuf[SSD1306_PAGES][SSD1306_WIDTH];
static uint8_t            s_tx_buf[SSD1306_WIDTH + 1]; /* data prefix + one row */

/* =========================================================================
 * Private helpers
 * ========================================================================= */

static HAL_StatusTypeDef ssd1306_send_cmd(uint8_t cmd)
{
    uint8_t buf[2] = { 0x00, cmd };   /* Co=0, D/C#=0 → command */
    return HAL_I2C_Master_Transmit(s_hi2c, SSD1306_I2C_ADDR, buf, 2, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef ssd1306_send_data(const uint8_t *data, uint16_t len)
{
    /* Build a single I2C burst: 0x40 (data prefix) followed by payload */
    if (len > SSD1306_WIDTH) { len = SSD1306_WIDTH; }
    s_tx_buf[0] = 0x40;               /* Co=0, D/C#=1 → data */
    memcpy(&s_tx_buf[1], data, len);
    return HAL_I2C_Master_Transmit(s_hi2c, SSD1306_I2C_ADDR,
                                   s_tx_buf, (uint16_t)(len + 1u), HAL_MAX_DELAY);
}

/* =========================================================================
 * Public API
 * ========================================================================= */

HAL_StatusTypeDef OLED_Init(I2C_HandleTypeDef *hi2c)
{
    s_hi2c = hi2c;
    HAL_Delay(10);   /* power-on stabilisation */

    static const uint8_t init_seq[] = {
        SSD1306_CMD_DISPLAY_OFF,
        SSD1306_CMD_SET_DISPLAY_CLK,  0x80,   /* ratio/osc */
        SSD1306_CMD_SET_MUX_RATIO,    0x3F,   /* 64 MUX    */
        SSD1306_CMD_SET_DISPLAY_OFFSET, 0x00,
        SSD1306_CMD_SET_START_LINE,         /* 0x40 | 0 */
        SSD1306_CMD_CHARGE_PUMP,      0x14,   /* enable internal pump */
        SSD1306_CMD_MEM_ADDR_MODE,    0x00,   /* horizontal */
        SSD1306_CMD_SEG_REMAP,                /* col 127→SEG0 */
        SSD1306_CMD_COM_SCAN_DEC,
        SSD1306_CMD_SET_COM_PINS,     0x12,
        SSD1306_CMD_SET_CONTRAST,     0xCF,
        SSD1306_CMD_SET_PRECHARGE,    0xF1,
        SSD1306_CMD_SET_VCOM_DESELECT,0x40,
        SSD1306_CMD_ENTIRE_ON_RESUME,
        SSD1306_CMD_NORMAL_DISPLAY,
        SSD1306_CMD_DISPLAY_ON,
    };

    for (uint8_t i = 0; i < sizeof(init_seq); i++)
    {
        if (ssd1306_send_cmd(init_seq[i]) != HAL_OK)
        {
            return HAL_ERROR;
        }
    }

    OLED_Clear();
    OLED_Flush();
    return HAL_OK;
}

void OLED_Clear(void)
{
    memset(s_framebuf, 0, sizeof(s_framebuf));
}

void OLED_Flush(void)
{
    /* Set full-panel address window once, then burst every page */
    ssd1306_send_cmd(SSD1306_CMD_SET_COL_ADDR);
    ssd1306_send_cmd(0);
    ssd1306_send_cmd(SSD1306_WIDTH - 1);

    ssd1306_send_cmd(SSD1306_CMD_SET_PAGE_ADDR);
    ssd1306_send_cmd(0);
    ssd1306_send_cmd(SSD1306_PAGES - 1);

    for (uint8_t page = 0; page < SSD1306_PAGES; page++)
    {
        ssd1306_send_data(s_framebuf[page], SSD1306_WIDTH);
    }
}

void OLED_DrawString(uint8_t page, uint8_t col, const char *str)
{
    if (!str || page >= SSD1306_PAGES) { return; }

    for (uint8_t ci = 0; str[ci] != '\0'; ci++)
    {
        uint8_t ch = (uint8_t)str[ci];
        if (ch < 0x20 || ch > 0x7E) { ch = 0x20; }   /* replace non-printable */

        const uint8_t *glyph = s_font5x7[ch - 0x20];

        for (uint8_t gx = 0; gx < 5; gx++)
        {
            if (col >= SSD1306_WIDTH) { return; }       /* clip at right edge */
            s_framebuf[page][col++] = glyph[gx];
        }
        /* 1-pixel column spacer */
        if (col < SSD1306_WIDTH) { s_framebuf[page][col++] = 0x00; }
    }
}

void OLED_UpdateDisplay(const HRV_t           *hrv,
                        const CNN_Result_t    *cnn,
                        const Fusion_Result_t *fusion)
{
    char line[22];   /* 128 px / 6 px per char ≈ 21 chars max */

    OLED_Clear();

    /* ── Line 0 (page 0):  HR : <bpm> BPM ── */
    snprintf(line, sizeof(line), "HR : %.0f BPM", hrv->heartRate);
    OLED_DrawString(0, 0, line);

    /* ── Line 1 (page 2):  CNN: <prob> ── */
    snprintf(line, sizeof(line), "CNN: %.3f", cnn->fatigueProbability);
    OLED_DrawString(2, 0, line);

    /* ── Line 2 (page 4):  DSI: <score> ── */
    snprintf(line, sizeof(line), "DSI: %.1f", fusion->safetyIndex);
    OLED_DrawString(4, 0, line);

    /* ── Line 3 (page 6):  STATUS : <zone> ── */
    const char *status;
    switch (fusion->zone)
    {
        case FUSION_ZONE_GREEN:   status = "ALERT";        break;
        case FUSION_ZONE_YELLOW:  status = "EARLY FATIGUE"; break;
        case FUSION_ZONE_RED:     status = "DROWSY!";      break;
        default:                  status = "CALIBRATING";  break;
    }
    snprintf(line, sizeof(line), "ST: %s", status);
    OLED_DrawString(6, 0, line);

    OLED_Flush();
}
