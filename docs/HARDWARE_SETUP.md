# Hardware setup and bring-up checklist

This is a wiring/configuration checklist based on source comments and the CubeMX project. Confirm every pin and peripheral setting against your exact board revision before connecting hardware.

## Connections described by the firmware

| Function | MCU signal | Connect to | Notes |
|---|---|---|---|
| ECG stream RX | USART2 RX | ECG replay source / ST-Link VCP path | 4-byte binary float samples; confirm sender format and baud rate in `.ioc` / generated init code |
| Alert TX | USART1 TX (PA9 per source comments) | ESP32 UART2 RX (GPIO16 per source comments) | Newline-terminated `BT,...` packets |
| Optional ESP32 response | USART1 RX (PA10 per comments) | ESP32 UART2 TX (GPIO17 per comments) | Not consumed by current bridge module |
| OLED SCL/SDA | I²C1 PB6/PB7 | SSD1306 module | Address constant is `0x3C << 1` for STM32 HAL |
| Buzzer | PB0 | Buzzer input / positive terminal | Confirm drive current; use a transistor/driver if the buzzer exceeds GPIO limits |
| Common reference | GND | All connected modules | Common ground required for UART/I²C |

## Before powering up

- [ ] Verify MCU board and pin alternate functions in `ARAN.ioc`.
- [ ] Verify the actual USART1/USART2 baud rates in generated initialization code.
- [ ] Confirm the sender uses 250 Hz float32 little-endian values, not text CSV.
- [ ] Verify that the ECG input has appropriate electrical isolation/protection for the source. Never connect a person to a prototype circuit without appropriate medical-grade isolation and qualified review.
- [ ] Check OLED I²C address and pull-ups.
- [ ] Ensure the buzzer is electrically compatible with PB0.
- [ ] Test reception with a synthetic, known waveform before using recorded data.
- [ ] Capture UART logs and check overflow flags during a sustained replay.

## Limitations

The code comments are not a substitute for a verified schematic. No schematic, bill of materials, ESP32 firmware, or sensor front-end design was included in the supplied archive.
