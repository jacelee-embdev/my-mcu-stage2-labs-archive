**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 RTT and EasyLogger Template

This STM32F411CEU6 project integrates SEGGER RTT and EasyLogger as a development baseline for future embedded projects. Reuse its peripheral initialization, debug output, and logging framework when adding application features. It currently runs bare metal, without FreeRTOS, and uses SysTick as the HAL time base.

## Features

- Standard `printf` redirects to RTT channel 0, which EasyLogger also uses.
- Supports multiple log levels and colored output, including uptime, module tags, source paths, line numbers, and function names.
- Centralizes logging initialization in `debug_init()` and currently uses synchronous output.

## Usage

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK-ARM with ARMCC 5, then build and flash the target using J-Link.
2. Open J-Link RTT Viewer, select the target device, SWD interface, and channel 0, then run or reset the board.
3. The main loop prints test messages using `printf`, `SEGGER_RTT_printf`, and EasyLogger approximately every two seconds.

## Further Development

Extend application code in `Core/Src/main.c`. Maintain logging initialization and formats in `Middlewares_User/EasyLogger/debug/debug.c`, configure logging options in `Middlewares_User/EasyLogger/inc/elog_cfg.h`, and find the `printf` redirection in `Core/Src/usart.c`.

Before adding FreeRTOS, use CubeMX's `System Core → SYS → Timebase Source` setting to move the HAL time base from SysTick to another timer, such as TIM1. Then enable FreeRTOS and generate code. Adapt the application logic and logging synchronization for the task model, and check that the existing porting changes remain intact.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
