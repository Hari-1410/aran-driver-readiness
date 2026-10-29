# Reproducibility checklist

Before describing a build or result as reproducible, record: 

- STM32CubeIDE version and compiler/toolchain version.
- STM32CubeMX version and `.ioc` generation version.
- STM32F4 HAL, CMSIS-Core, CMSIS-DSP and X-CUBE-AI versions.
- Exact MCU board, clock source, linker configuration and optimization settings.
- ECG replay source, sample rate, float format, and test recording provenance.
- Original model version, training preprocessing, quantization parameters and reference test vectors.
- Build output size, RAM/flash use, inference latency and sustained UART-overflow measurements.
- Test results for known ECG waveforms and edge cases.

The supplied archive does not provide all of this information, so fill it in during the next hardware validation cycle.
