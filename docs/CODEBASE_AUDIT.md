# Codebase audit — supplied snapshot

**Scope:** static review of the uploaded ARAN STM32CubeIDE project, including application C modules, generated X-CUBE-AI files, CubeMX configuration and build artifacts. No clean rebuild, hardware test, ECG replay, or model-accuracy evaluation was performed. Findings below are code-level risks, not a claim that each has been reproduced on hardware.

## Summary

The project has a useful embedded-AI structure and clearly separated modules, but the current snapshot should be presented as a **prototype** rather than a validated real-time fatigue-monitoring system. The largest concerns are signal-processing correctness, ISR/main concurrency, and blocking telemetry.

## High-priority findings

### 1. Moving-window integration appears to drop the first sample of every full window
**File:** `Core/Src/pan_tompkins.c`, `stage_mwi()`

The seed loop accumulates samples `0` through `win_len - 2`. At `i == win_len - 1`, the steady-state loop adds `in[i]` and also subtracts `in[i - (win_len - 1)]`, which subtracts `in[0]`. The resulting sum contains samples `1..win_len-1` rather than the intended `0..win_len-1`, and the divisor remains `win_len`. Subsequent iterations remain offset. This can bias the integrated signal and alter QRS detection. Fix the initialization/update convention and add a deterministic unit test against a reference moving average before relying on HRV output.

### 2. Ring-buffer `count` is modified by both ISR and foreground code
**Files:** `Core/Src/circular_buffer.c`, `Core/Inc/circular_buffer.h`

The RX ISR increments `count` in `CB_Push()`, while the main loop decrements it in `CB_Pop()`. `volatile` does not make a read-modify-write sequence atomic or make a shared counter safe from interleaving. A producer increment and consumer decrement can overwrite each other, yielding an incorrect count and potentially false-full/false-empty behavior. Prefer a proper single-producer/single-consumer design where each side owns one index and full/empty are derived from indices, or protect shared state with a carefully scoped critical section. Add stress tests around wraparound and simultaneous push/pop.

### 3. Per-sample blocking telemetry can undermine continuous acquisition
**File:** `Core/Src/main.c`; transmit helpers in `Core/Src/ecg_uart.c`

Each 2,500-sample window emits 2,500 raw lines and 2,500 filtered lines using blocking `HAL_UART_Transmit()`, in addition to peak/HRV/CNN/fusion/status lines. At 115,200 baud, ASCII telemetry for 5,000 lines can consume a substantial fraction of or exceed the 10-second acquisition window. While the foreground is transmitting, RX continues in the ISR and the 4,096-sample queue fills; sustained operation can eventually overflow. Use DMA/interrupt-driven TX with a bounded queue, downsample/decimate telemetry, stream samples as they arrive, or disable raw/filtered telemetry in normal mode. Measure end-to-end throughput and overflow count.

### 4. CNN input preprocessing must be verified against training
**Files:** `Core/Src/dsp_preprocessing.c`, `Core/Src/cnn.c`, generated model metadata

The firmware applies a 50 Hz notch, 0.5–40 Hz band-pass and z-score normalization independently to each 2,500-sample window before quantizing for the INT8 network. The generated metadata describes an S8 input with scale approximately `0.148956` and zero-point `-48`, and an S8 output with scale `0.00390625` and zero-point `-128`; the affine quantization formula in `cnn.c` is structurally consistent with those values if the runtime exposes the expected metadata. However, the original model/training pipeline and representative test vectors are absent, so preprocessing parity, probability calibration, and model performance cannot be verified. Compare byte-for-byte input tensors and outputs against the reference model on a fixed dataset.

### 5. DSI fusion thresholds are heuristic and not validated
**File:** `Core/Src/fusion.c`

The source itself notes that the HRV normalization ranges are literature-typical values, not derived from this project's dataset. The resulting DSI/zone should be described as a prototype heuristic, not a validated fatigue or driver-safety score. Document the exact formula, validate against labeled data, and assess false positives/negatives before any safety-related interpretation.

## Medium-priority findings

### 6. Input stream framing is fragile
**File:** `Core/Src/ecg_uart.c`

The RX callback groups every four bytes into a float with no packet header, checksum, timestamp, sequence number or resynchronization marker. A dropped/inserted byte can corrupt all subsequent float alignment until the stream is manually restarted. Consider a framed binary protocol (magic, sequence, payload, CRC) and explicit handling of `HAL_UART_Receive_IT()` failures.

### 7. Sampling rate is assumed, not measured
**Files:** `window_manager.h`, `pan_tompkins.h`, `hrv.c`, DSP coefficients

The code assumes 250 Hz in filter coefficients, sample-to-millisecond conversion and 10-second windows. It does not validate or timestamp incoming samples. Any sender-rate drift or mismatch affects filtering, heart rate and interval calculations. Add timestamps or enforce and monitor the source rate.

### 8. Error and validity handling is inconsistent
**Files:** `Core/Src/main.c`, `Core/Src/cnn.c`, `Core/Src/fusion.c`, `Core/Src/oled.c`

`main()` calls fusion and updates UI/alerts after `CNN_Run()` even when `result.valid` may be false; confirm that fusion explicitly handles invalid CNN/HRV values. `OLED_UpdateDisplay()` assumes non-null pointers and shows numeric fields without an explicit invalid-data state. Add end-to-end validity propagation and a safe degraded mode. Also check return values from UART re-arming and most telemetry transmissions.

### 9. UART module comments/API documentation disagree with actual wiring
**Files:** `Core/Src/ecg_uart.c`, `Core/Inc/ecg_uart.h`, `Core/Src/main.c`

Some comments describe USART1 as the visualization TX path, but `main()` calls `ECG_UART_Init(&huart2, &huart2)`, so ECG UART status/raw/filtered telemetry uses USART2. The Bluetooth alert module separately uses USART1. Update comments and protocol docs to match the intended physical connection.

### 10. Blocking buzzer patterns add foreground latency
**File:** `Core/Src/buzzer.c`

`BUZZER_Alert()` uses `HAL_Delay()` for the full pattern. This adds up to roughly half a second of blocking per processed window. It is less severe than raw telemetry, but a timer-driven non-blocking pattern would improve scheduling and make later asynchronous processing easier.

## Repository / reproducibility gaps

- No project-wide license was supplied; third-party ST/CMSIS/X-CUBE-AI licenses must remain intact.
- No automated unit tests, host-side replay script, CI workflow, or documented model training pipeline were present in the archive.
- The original `.tflite` file, ESP32 bridge firmware, and external dashboard code were not present; only generated network C files and metadata were included.
- A prior `Debug` directory and compiled ELF were in the ZIP, but they were excluded from this repository snapshot. A clean build has not been run here.
- The generated model report included an absolute Windows output path; that machine-specific path was removed from the curated copy.

## Recommended order of work

1. Fix and unit-test the moving-window integrator.
2. Replace shared `count` synchronization with a safe SPSC ring buffer.
3. Remove blocking per-sample output from the real-time path; quantify throughput.
4. Add host-side tests for filter response, R-peak detection, RR/HRV calculations, fusion boundaries and quantization.
5. Compare embedded CNN outputs against the original reference model with fixed test vectors.
6. Propagate `valid` flags and define safe behavior on invalid data / buffer overflow.
7. Add a reproducible build guide, pin map/schematic, demo logs and measured MCU memory/inference/latency results.
8. Only then write resume claims with measured and repeatable evidence.
