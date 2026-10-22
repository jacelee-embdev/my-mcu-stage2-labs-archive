**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Manual Relocation Experiment

Based on the STM32F411CEU6 template, this experiment bypasses `__main` and manually copies RW and clears ZI at the start of `main()`. It demonstrates scatter loading and startup while retaining HAL, SEGGER RTT, EasyLogger, and FreeRTOS.

## Configuration

- Keil MDK 5 / ARM Compiler 5 with MicroLIB enabled. Flash starts at `0x08000000` and RAM at `0x20000000`.
- HSI and PLL provide a 100 MHz system clock. TIM1 supplies the HAL time base, while FreeRTOS uses a 1000 Hz tick.

## Implementation

- `MDK-ARM/startup_stm32f411xe.s` imports `main`, following `Reset_Handler → SystemInit → main → RelocateData`.
- `Core/Src/main.c` obtains boundaries from linker symbols, calculates byte counts from address differences, and initializes RW/ZI with `MemoryCopy()` and `MemoryZero()`.
- `g_test_rw` starts at `0x12345678`. ARMCC5's `zero_init` attribute explicitly places `g_test_zi` in ZI.

## Debug verification

Open `MDK-ARM/STM32F411CEU6.uvprojx`, build, and download using J-Link/SWD. Add both test variables and their addresses to Watch:

1. Halt before calling `RelocateData()` and set `g_test_rw` to `0x11111111` and `g_test_zi` to `0x22222222`.
2. Halt before calling `MemoryZero()`. RW should be restored to `0x12345678`, while ZI retains the modified value.
3. Halt after clearing ZI, before `RelocateData()` returns, and confirm ZI is `0`. Obtain addresses from the current MAP and disassembly.

A debug reset does not automatically clear SRAM, so previous results may remain. Modify the values before verification to distinguish initialization from retained data.

## Scope and maintenance

This experiment bypasses library startup and assumes uncompressed RW initial values. The existing scatter file includes the active stack in ZI, so clearing it may disrupt function returns. Verification is limited to RW copying and ZI clearing.

Added C code stays within CubeMX user sections. After regeneration, check `IMPORT main` and the branch target, retaining the relocation call and existing peripheral, logging, and RTOS configuration.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
