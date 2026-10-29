# Hardware setup and bring-up checklist

This checklist is based on firmware comments and CubeMX configuration. Verify every pin and peripheral setting against your exact board revision before connecting hardware.

## Connections described by the firmware

| Function | MCU signal | Connect to | Notes |
|---|---|---|---|
| ECG stream RX | USART2 RX | ECG replay source / ST-Link VCP path | Four-byte binary float samples; verify sender format and baud rate |
| Alert TX | USART1 TX (PA9 per comments) | ESP32 UART RX (GPIO16 per comments) | Newline-terminated `BT,...` packets |
| Optional ESP32 response | USART1 RX (PA10 per comments) | ESP32 UART TX (GPIO17 per comments) | Not consumed by current bridge module |
| OLED SCL/SDA | I²C1 PB6/PB7 | SSD1306 module | Address constant uses STM32 HAL shifted format |
| Buzzer | PB0 | Buzzer input | Use a suitable driver if current exceeds GPIO limits |
| Common reference | GND | Connected modules | Common ground required for UART/I²C |

## Before powering up

- [ ] Verify MCU board and alternate-function mapping in `ARAN.ioc`.
- [ ] Verify USART1/USART2 baud rates in generated initialization code.
- [ ] Confirm the sender uses 250 Hz float32 little-endian values, not CSV text.
- [ ] Use suitable electrical isolation/protection for any ECG source. Never connect a person to a prototype circuit without appropriate medical-grade isolation and qualified review.
- [ ] Check OLED I²C address and pull-ups.
- [ ] Ensure the buzzer is electrically compatible with PB0.
- [ ] Test reception with a synthetic known waveform before recorded data.
- [ ] Capture UART logs and check overflow flags during sustained replay.

## Limitations

Firmware comments are not a verified schematic. No schematic, bill of materials, ESP32 firmware, or sensor front-end design was included in the supplied archive.
