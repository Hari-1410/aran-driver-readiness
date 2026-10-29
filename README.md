# ARAN — AI-Based Personalized Driver Readiness Monitoring System

**ARAN** is an AI-based personalized driver-readiness monitoring prototype developed for TATA InnoVent 2027. It is an STM32-based embedded signal-processing and edge-inference system that processes ECG samples in fixed windows, estimates heart-rate/HRV features, runs a quantized neural network, combines the outputs into a Driver Safety Index (DSI), and presents status through an OLED, buzzer, and UART-connected ESP32 Bluetooth bridge.

> **Research/engineering prototype — not a medical device or a validated driver-safety system.** The code has not been clinically validated, and the repository still has integration and real-time issues documented in [`docs/CODEBASE_AUDIT.md`](docs/CODEBASE_AUDIT.md). Do not use its output to make medical or driving decisions.

## Competition context

**TATA InnoVent 2027** — project branding reflects the competition context provided by the project team.

## What is in this repository

- STM32CubeIDE project for **STM32F446RE** / Cortex-M4F.
- Interrupt-driven UART reception and fixed-size ECG windowing.
- CMSIS-DSP notch/band-pass preprocessing and per-window normalization.
- Pan–Tompkins-style QRS/R-peak detection and RR-derived HRV features.
- X-CUBE-AI generated network integration for an INT8 model (`network.*`, `network_data.*`).
- Heuristic decision fusion / DSI zone calculation.
- SSD1306 OLED, GPIO buzzer, and UART packet output intended for an ESP32 Bluetooth SPP bridge.

## Processing pipeline

```text
ECG sample stream (binary float32, UART2)
                 │
                 ▼
       UART RX interrupt + ring buffer
                 │
                 ▼
      2,500-sample window (10 s @ 250 Hz)
                 │
                 ▼
     50 Hz notch → 0.5–40 Hz band-pass
                 │
                 ▼
          Per-window z-score
                 │
          ┌──────┴─────────┐
          ▼                ▼
   Pan–Tompkins QRS     INT8 CNN inference
          │                │
          ▼                │
     RR / HRV features     │
          └──────┬─────────┘
                 ▼
       Heuristic decision fusion / DSI
                 │
        ┌────────┼───────────┐
        ▼        ▼           ▼
       OLED    Buzzer    UART alert → ESP32*
```

`*` The ESP32 bridge firmware and host/mobile dashboard are **not included** in the uploaded codebase.

## Hardware / software assumptions

| Item | Configuration in project |
|---|---|
| MCU / board | STM32F446RE (project startup and linker scripts target STM32F446RETx) |
| Clock | CubeMX configuration indicates 180 MHz system clock; verify board clock source and generated clock setup before flashing |
| ECG input | USART2 / ST-Link VCP; binary 32-bit little-endian IEEE-754 float samples |
| Nominal sample rate | 250 Hz (expected from sender; the firmware does not timestamp or enforce this rate) |
| Window | 2,500 samples (nominally 10 seconds) |
| DSP | CMSIS-DSP biquad filters and math functions |
| ML runtime | ST X-CUBE-AI generated `network` implementation; INT8 input/output |
| Display | SSD1306 128×64 OLED, I²C1, configured address 0x3C (7-bit) |
| Alert bridge | USART1 to an ESP32 UART bridge (external firmware required) |
| Buzzer | GPIO PB0 |
| IDE | STM32CubeIDE / CubeMX project metadata included |

Pin assignments and UART settings must be checked against the actual Nucleo board and sender configuration. See [`docs/HARDWARE_SETUP.md`](docs/HARDWARE_SETUP.md).

## Open in STM32CubeIDE

1. Install STM32CubeIDE and the STM32F4 device support package.
2. Install/enable the compatible **X-CUBE-AI** package for the generated AI runtime in this project.
3. Clone/download this repository and use **File → Import → Existing Projects into Workspace**. Select the repository root.
4. Inspect `ARAN.ioc`, linker scripts, include paths, and the X-CUBE-AI middleware/runtime version. Regenerate code only after backing up changes in user-code sections.
5. Build the `Debug` configuration. Resolve any toolchain/runtime version mismatch before flashing.
6. Connect the ECG source to USART2 using the exact binary format described below.

The ZIP supplied for this audit contained a previously built `Debug/ARAN.elf`, but generated build products were intentionally excluded from this GitHub-ready version. **A clean build was not executed in this environment**, so successful compilation on a fresh installation is not claimed.

## ECG input protocol

The RX ISR reassembles every four incoming bytes into one native `float` using `memcpy`. This assumes the sender transmits IEEE-754 float32 values in little-endian byte order, with no ASCII delimiters or packet framing. The implementation has no checksum, sequence number, timestamp, or robust resynchronization marker. Ensure sender and MCU agree on byte order and rate.

## Telemetry and outputs

- `R,...`: raw samples (as currently emitted by the UART reporting path).
- `F,...`: preprocessed samples.
- `S,...`: status messages.
- `BT,<zone>,<dsi>,<hr>,<prob>`: alert packet on USART1 for the external ESP32 bridge.

The firmware currently sends a large volume of per-sample telemetry synchronously. This is a known real-time bottleneck and can lead to buffer overflow during continuous streaming; see the audit before relying on long runs.

## Repository structure

```text
Core/Inc, Core/Src/       Application modules
Core/Startup/             STM32 startup assembly
Drivers/                  ST HAL, CMSIS core and CMSIS-DSP sources
Middlewares/ST/AI/         ST AI runtime files and license
X-CUBE-AI/App/             Generated network wrapper, weights and metadata
ARAN.ioc                  CubeMX hardware/peripheral configuration
*.ld                       Flash/RAM linker scripts
docs/                      Architecture, hardware setup and code audit
```

## Current status and next steps

This repository is a cleaned, documented snapshot of the supplied project, not a claim that the complete end-to-end system is production-ready. Prioritize: (1) fix and test the moving-window integration indexing, (2) correct ring-buffer concurrency, (3) redesign telemetry so it cannot block acquisition, (4) validate CNN preprocessing/quantization against the training pipeline, and (5) add replay-based regression tests and measured on-target performance. The detailed findings and severity are in [`docs/CODEBASE_AUDIT.md`](docs/CODEBASE_AUDIT.md).

## License and attribution

No project-wide license was supplied, so no blanket open-source license has been invented. Third-party components retain their own license terms; see [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and the license files distributed with the relevant components. Before making the repository public, decide which original project code you have the right to license and add a project license if appropriate.
