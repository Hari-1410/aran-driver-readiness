# Architecture notes

## Runtime sequence

The main application initializes HAL/peripherals, starts ECG UART reception, initializes DSP filters and R-peak detection, initializes the X-CUBE-AI network, and brings up the OLED, alert UART, and buzzer. The super-loop drains the receive queue into a 2,500-sample window. A full window is processed as a batch: preprocessing, QRS detection, HRV extraction, CNN inference, heuristic fusion, UI/alerts, and serial telemetry.

## Module map

| Module | Responsibility |
|---|---|
| `ecg_uart.c` | UART RX interrupt, float reassembly, serial telemetry |
| `circular_buffer.c` | ISR-to-main sample queue |
| `window_manager.c` | Fixed-size ECG windowing |
| `dsp_preprocessing.c` | Notch/band-pass filters and normalization |
| `pan_tompkins.c` | Derivative, squaring, integration, adaptive QRS thresholding |
| `hrv.c` | RR validation and heart rate, SDNN, RMSSD, NN50, pNN50 |
| `cnn.c` | X-CUBE-AI initialization, quantized tensor I/O, inference |
| `fusion.c` | Rule-based fusion and DSI zone |
| `oled.c` | SSD1306 frame buffer and status text |
| `buzzer.c` | Zone-dependent beep patterns |
| `bluetooth.c` | Structured alert packet over USART1 to an external ESP32 |

## Important boundaries

- Fusion thresholds are heuristic, not a validated driver-risk model.
- The OLED, ESP32 bridge, and external dashboard have no end-to-end integration test in the supplied snapshot.
- No host-side ECG replay utility, automated unit-test suite, CI workflow, or model-training pipeline was included.
- The original `.tflite` model and training code were not present; generated network C files and metadata were supplied.
