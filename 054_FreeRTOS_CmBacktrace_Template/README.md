**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS and CmBacktrace Template

This project integrates FreeRTOS, CmBacktrace, SEGGER RTT, and EasyLogger as a development baseline for future projects. TIM1 provides the HAL time base. Standard `printf`, logs, and fault diagnostics use RTT channel 0, with support for HardFault diagnostics and task stack backtraces.

## Task Organization

- We prefer the more familiar native FreeRTOS APIs, so resource creation is centralized in `Z02_Business/Init/os_init.c` and `os_init.h`. The generated `freertos.c` calls `OS_Init()` in place of default CMSIS-style task creation; future tasks and other RTOS resources should be created here.
- Each task has its own directory with matching `.c/.h` files. `Z02_Business/Status_Task` only tests LED blinking and logging. Remove it when starting application development if no longer needed, along with its include, creation code in `os_init.c`, and Keil references.

## Usage

Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK-ARM with ARMCC 5, build and flash the target, then view channel 0 in J-Link RTT Viewer.

## Regenerating Code with CubeMX

**Back up the project before every configuration change.** Regeneration may restore or overwrite manual changes. Afterwards:

1. Comment out the default task's `osThreadNew(...)` creation statement in `Core/Src/freertos.c` again, keeping the `OS_Init()` call.
2. Comment out the `HardFault_Handler()` implementation in `Core/Src/stm32f4xx_it.c` again so that `cmb_fault.S` provides the fault entry point.
3. Back up the entire `Middlewares/Third_Party/FreeRTOS` directory beforehand. When retaining the same FreeRTOS version, restore that complete backup over the regenerated directory to recover the CmBacktrace stack-depth adaptations, including changes in `Source/tasks.c` and `Source/include/FreeRTOS.h`.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
