# Architecture notes

## Runtime sequence

`main()` initializes HAL and peripherals, starts ECG UART reception, initializes the DSP filters and R-peak detector, initializes the X-CUBE-AI network, then brings up the OLED, UART alert bridge and buzzer. The super-loop drains the RX ring buffer into a 2,500-sample window. Once full, the window is processed as a batch: DSP preprocessing, QRS detection, HRV extraction, CNN inference, decision fusion, UI/alerts and serial telemetry.

## Module map

| Module | Responsibility |
|---|---|
| `ecg_uart.c` | Byte-wise UART RX interrupt, float reassembly, serial telemetry |
| `circular_buffer.c` | ISR-to-main sample queue |
| `window_manager.c` | Fixed 2,500-sample windowing |
| `dsp_preprocessing.c` | 50 Hz notch, 0.5–40 Hz band-pass, z-score |
| `pan_tompkins.c` | Derivative, squaring, moving-window integration, adaptive QRS thresholding |
| `hrv.c` | RR validation and heart rate, SDNN, RMSSD, NN50, pNN50 |
| `cnn.c` | X-CUBE-AI model initialization, quantized tensor I/O, inference timing |
| `fusion.c` | Rule-based fusion and DSI zone |
| `oled.c` | SSD1306 frame buffer and status text |
| `buzzer.c` | Zone-dependent blocking beep patterns |
| `bluetooth.c` | Sends one structured alert packet to the ESP32 over USART1 |

## Important boundaries

- `fusion.c` uses heuristic thresholds, not a validated driver-risk model.
- The OLED, ESP32 bridge, and external dashboard have no end-to-end integration test in the supplied repository.
- There is no host-side ECG replay utility, automated unit-test suite, CI workflow, or documented dataset/training pipeline in the supplied snapshot.
- The CNN weights are embedded in generated C files; the original `.tflite` model and model-training code are not present in the archive.
