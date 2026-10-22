**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Keil Debug Basics

This project records basic STM32 debugging with Keil. Generated with STM32CubeMX, it includes GPIO, USART1, and HAL initialization. A small, repeatable serial-output loop provides a starting point for observing execution, breakpoints, and peripheral state.

## Current Program

After initialization, `Core/Src/main.c` prints `Hello World!` approximately every two seconds using `HAL_Delay(2000)`. The active retarget function in `Core/Src/usart.c` sends characters through USART1 at 115200 baud, 8N1.

The source retains a commented example using `ITM_SendChar()`, while the active output path is UART. Use a serial terminal for the current program. Exploring ITM requires separate verification of the probe, trace clock, and output settings.

## Build and Debug

Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx) with Keil MDK and ARM Compiler 5, rebuild, and download. Connect the configured SWD debugger and place breakpoints in the main loop and retarget function. Use stepping, variable inspection, and register views to inspect the calls. Connect the serial adapter's RX to USART1 TX and share ground with the board. Periodic output pauses at breakpoints; check the output interval again after resuming execution. Check the selected device, flash algorithm, and probe connection before debugging.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
