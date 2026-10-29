# Codebase audit — supplied snapshot

**Scope:** Static review of the supplied STM32CubeIDE project, application C modules, generated X-CUBE-AI files, CubeMX configuration, and build artifacts. No clean rebuild, hardware test, ECG replay, or model-accuracy evaluation was performed. These are code-level risks; not every finding has been reproduced on hardware.

## Summary

The project has a useful embedded-AI structure and clearly separated modules, but the current snapshot should be presented as a prototype rather than a validated real-time driver-readiness system. The largest concerns are signal-processing correctness, ISR/main concurrency, and blocking telemetry.

## High-priority findings

### 1. Moving-window integration may omit the first sample
**File:** `Core/Src/pan_tompkins.c`

The moving-window integration update appears to add the new sample and subtract `in[0]` on the iteration where the first full window is formed. This can produce a sum for samples 1..N-1 while dividing by N, biasing the integrated signal and potentially changing QRS detection. Verify the update convention and add a deterministic unit test against a reference moving average.

### 2. Ring-buffer counter is shared by ISR and foreground code
**Files:** `Core/Src/circular_buffer.c`, `Core/Inc/circular_buffer.h`

The interrupt handler increments `count` while foreground code decrements it. `volatile` does not make read-modify-write operations atomic. A producer increment and consumer decrement can overwrite each other, resulting in incorrect full/empty state. Prefer a proper single-producer/single-consumer design where each side owns one index, or use a carefully scoped critical section. Add wraparound and stress tests.

### 3. Blocking per-sample telemetry can undermine continuous acquisition
**Files:** `Core/Src/main.c`, `Core/Src/ecg_uart.c`

Each 2,500-sample window emits thousands of raw and filtered sample lines using blocking UART transmission, in addition to other telemetry. At typical UART rates, this can consume a substantial fraction of or exceed the 10-second acquisition window. While foreground code transmits, RX continues in the ISR and the queue can fill. Use asynchronous TX with a bounded queue, decimate telemetry, or disable per-sample logging in normal mode; measure sustained throughput and overflow counts.

### 4. CNN preprocessing and output need reference validation
**Files:** `Core/Src/dsp_preprocessing.c`, `Core/Src/cnn.c`

The firmware applies notch/band-pass filtering and per-window normalization before quantization for the INT8 network. Generated metadata provides quantization parameters, but the original model/training pipeline and representative test vectors are absent. Compare input tensors and predictions against the reference model on fixed data before claiming model accuracy.

### 5. Driver Safety Index thresholds are heuristic
**File:** `Core/Src/fusion.c`

The fusion thresholds are not derived from validated labeled data. Describe the score as a prototype heuristic, not a validated fatigue or driver-safety assessment. Document the formula and evaluate false positives/negatives before safety-related interpretation.

## Medium-priority findings

### 6. Input stream has no framing or resynchronization
**File:** `Core/Src/ecg_uart.c`

The receive callback groups every four bytes into a float without a packet header, checksum, timestamp, or sequence number. A dropped/inserted byte can corrupt subsequent float alignment. Consider a framed binary protocol with a sync marker, sequence counter, and CRC.

### 7. Sampling rate is assumed, not measured
**Files:** `Core/Src/window_manager.h`, `Core/Src/pan_tompkins.h`, `Core/Src/hrv.c`, DSP coefficients

The code assumes 250 Hz but does not timestamp or enforce incoming samples. Rate mismatch affects filters and interval calculations. Add timestamps or monitor/enforce source rate.

### 8. Validity/error handling needs review
**Files:** `Core/Src/main.c`, `Core/Src/cnn.c`, `Core/Src/fusion.c`, `Core/Src/oled.c`

Confirm fusion and UI explicitly handle invalid CNN/HRV outputs. Add explicit invalid-data state and safe degraded behavior. Check UART re-arming and telemetry return values.

### 9. UART comments and wiring descriptions disagree
**Files:** `Core/Src/ecg_uart.c`, `Core/Inc/ecg_uart.h`, `Core/Src/main.c`

Some comments describe USART1 as the visualization TX path, while initialization passes USART2 for ECG UART reporting. The Bluetooth alert module separately uses USART1. Update comments and protocol documentation to match intended wiring.

### 10. Blocking buzzer patterns add foreground latency
**File:** `Core/Src/buzzer.c`

The buzzer alert uses `HAL_Delay()`, adding foreground blocking. A timer-driven non-blocking pattern would improve scheduling.

## Repository and reproducibility gaps

- No project-wide license was supplied in the original project; retain upstream ST/CMSIS/X-CUBE-AI license terms.
- No automated unit tests, host-side replay script, CI workflow, or documented model-training pipeline were included.
- The original `.tflite` model, ESP32 bridge firmware, and external dashboard were not in the supplied archive.
- Compiled build products were excluded from the curated snapshot. A clean build has not been run.
- A machine-specific generated-model path was removed from the curated documentation.

## Recommended order of work

1. Fix and test the moving-window integrator.
2. Correct ring-buffer synchronization.
3. Remove blocking telemetry from the real-time path and measure throughput.
4. Add host-side tests for filter response, R-peak detection, RR/HRV, fusion boundaries, and quantization.
5. Compare embedded CNN outputs against the reference model using fixed test vectors.
6. Propagate validity flags and define safe behavior on invalid data/overflow.
7. Record build, memory, inference-latency, and sustained-throughput results.
8. Make resume claims only when supported by repeatable measurements.
