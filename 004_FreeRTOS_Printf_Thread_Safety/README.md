**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Concurrent printf

This project records multiple FreeRTOS threads sharing UART output to explore scheduling, character transmission, and log integrity. It uses STM32CubeMX, HAL, CMSIS V2 kernel startup, and TIM1 as the HAL time base.

## Implementation

`Core/Src/freertos.c` creates three printing loops producing `TEST!1142`, `TEST!1143`, and `TEST!1144`. Their delay calls are commented out, so the threads continuously compete for USART1 output. A serial terminal can reveal interleaved strings.

The retarget function in `Core/Src/usart.c` calls `vTaskSuspendAll()` before sending each character and `xTaskResumeAll()` afterward. This protects one character transmission; it does not establish mutual exclusion for an entire `printf` call or log line. A mutex or dedicated logging thread can be explored when whole-line consistency is required.

## Build and Observe

Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx) with Keil MDK and ARM Compiler 5, rebuild, and download. Configure the serial terminal for 115200 baud, 8N1. Compare the output with the three loops and retarget function, then observe how thread delays change the behavior. Thread-level scheduling APIs belong in thread context; avoid calling this print path directly from an interrupt. Save the output to compare observations under different scheduling conditions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
