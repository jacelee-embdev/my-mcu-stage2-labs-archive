**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS UART Printing

This project records moving basic button and LED behavior into a FreeRTOS thread. STM32CubeMX generates the HAL project, while CMSIS V2 initializes the kernel, creates threads, and starts the scheduler. It provides a small example of the transition from hardware initialization to a scheduled thread loop.

## Runtime Behavior

After GPIO and USART1 initialization, `main.c` calls `MX_FREERTOS_Init()`. The active loop is the default thread in `Core/Src/freertos.c`. Each iteration reads the button, toggles the LED when it is low, prints `TEST!1142`, and yields through `osDelay(1000)`. Holding the button toggles the LED repeatedly; a short press during the wait may be missed.

USART1 uses 115200 baud, 8N1, with `printf` retargeted to UART. Connect the serial adapter's RX to the configured TX pin and share ground with the board.

## Build and Observe

Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx) with Keil MDK and ARM Compiler 5, rebuild, and download through SWD. Observe the periodic serial messages and button-controlled LED. A breakpoint at the default thread entry shows execution after the scheduler starts. Retain the initialization and thread-creation entry points when extending the project, and use thread delay interfaces for waits. Check the button polarity, pin definitions, and serial wiring before running.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
