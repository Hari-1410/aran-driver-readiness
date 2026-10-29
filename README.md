# ARAN — AI-Based Personalized Driver Readiness Monitoring System

**TATA InnoVent 2027** · **STM32F446RE** · **Embedded C** · **CMSIS-DSP** · **ST X-CUBE-AI**

ARAN is an embedded ECG-processing and edge-inference prototype intended to explore personalized driver-readiness monitoring. The firmware processes ECG samples in fixed windows, extracts heart-rate/HRV features, runs a quantized neural network, combines model and signal features using a heuristic Driver Safety Index (DSI), and presents status through an OLED, buzzer, and UART alert interface.

> **Prototype only.** This code is not a medical device or a validated driver-safety system. It has not been clinically validated. Do not use its output to make medical or driving decisions.

## Pipeline

```text
ECG float32 stream over USART2
             |
             v
    UART RX + sample queue
             |
             v
   2,500-sample window
     (nominally 10 s @ 250 Hz)
             |
             v
  50 Hz notch + 0.5–40 Hz band-pass
             |
             v
       Window normalization
          /          \
         v            v
  QRS / R peaks    INT8 CNN
         |            |
         v            |
   RR / HRV features  |
          \          /
           v        v
      Heuristic decision fusion / DSI
                 |
          OLED / buzzer / UART
                 |
        ESP32 bridge (external)
```

## Features in the supplied firmware snapshot

- STM32CubeIDE project targeting STM32F446RE.
- Interrupt-driven UART sample reception and fixed-size windowing.
- CMSIS-DSP preprocessing, including notch and band-pass filtering.
- Pan–Tompkins-style QRS detection and RR-derived HRV features.
- ST X-CUBE-AI generated network integration for an INT8 model.
- Heuristic fusion logic and status zones.
- SSD1306 OLED, GPIO buzzer, and UART alert packet output.

The ESP32 bridge firmware, host/mobile dashboard, original model-training pipeline, and original `.tflite` model were not present in the supplied archive.

## Hardware and assumptions

| Item | Current project assumption |
|---|---|
| MCU | STM32F446RE |
| Nominal ECG sample rate | 250 Hz; firmware does not enforce or timestamp it |
| Window | 2,500 samples |
| ECG transport | USART2, four-byte little-endian IEEE-754 float samples |
| Display | SSD1306 128×64 over I²C1; configured address 0x3C |
| Alert bridge | USART1 to an external ESP32 UART bridge |
| Buzzer | GPIO PB0 |
| Toolchain | STM32CubeIDE / CubeMX and compatible X-CUBE-AI runtime |

See [hardware setup](docs/HARDWARE_SETUP.md) and [architecture notes](docs/ARCHITECTURE.md). Confirm pins, baud rates, clock source, and sender format against your exact hardware before flashing.

## Open in STM32CubeIDE

1. Install STM32CubeIDE and the STM32F4 device support package.
2. Install or enable a compatible X-CUBE-AI package for the generated runtime.
3. Clone this repository and import it using **File → Import → Existing Projects into Workspace**.
4. Inspect `ARAN.ioc`, linker scripts, include paths, and middleware versions.
5. Build the Debug configuration and resolve toolchain/runtime mismatches before flashing.
6. Send test data in the expected binary float32 format to USART2.

A clean build has **not** been verified in this environment. Build artifacts should not be used as a substitute for a clean local build.

## Known limitations

- Static review identified a likely moving-window integration indexing issue.
- The ISR/foreground ring-buffer counter is shared and requires a concurrency-safe design.
- Blocking per-sample telemetry can prevent sustained real-time acquisition.
- CNN preprocessing and predictions have not been compared against the original reference model.
- DSI thresholds are heuristic and not validated against labeled driver-readiness data.
- The binary input stream has no packet framing, checksum, or resynchronization marker.

Read the [codebase audit](docs/CODEBASE_AUDIT.md) before relying on the firmware. See [reproducibility checklist](docs/REPRODUCIBILITY.md) for measurements needed before making performance claims.

## Repository layout

- `Core/Inc`, `Core/Src` — application modules
- `Drivers/` — STM32 HAL, CMSIS core and DSP dependencies
- `Middlewares/ST/AI/`, `X-CUBE-AI/App/` — generated AI runtime and network integration
- `ARAN.ioc` — CubeMX configuration
- `docs/` — architecture, setup, audit, and reproducibility notes

## Licensing

See the repository's [LICENSE](LICENSE) and [third-party notices](THIRD_PARTY_NOTICES.md). Vendor components retain their upstream license requirements; verify redistribution rights for all included dependencies and generated code before reuse.
