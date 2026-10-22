**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 HAL + SEGGER RTT + EasyLogger + FreeRTOS + SystemView

A reusable STM32F411CEU6 firmware template combining HAL peripheral initialization, SEGGER RTT, EasyLogger, FreeRTOS, and SEGGER SystemView. It provides centralized task creation, a status task, and SystemView trace integration as a starting point for applications that need scheduling, structured logs, and task execution analysis. CubeMX configuration and the Keil project are included.

## Configuration

- HSI and PLL provide a 100 MHz system clock. TIM1 supplies the HAL time base, and FreeRTOS uses a 1000 Hz tick.
- USART1 uses PA9/TX and PA10/RX at 115200 baud, 8N1. Standard `printf` and EasyLogger use RTT channel 0.
- SystemView uses an automatically allocated RTT channel named `SysView`. Its FreeRTOS adapter is included through `FreeRTOSConfig.h`, with the trace facility enabled.

## Quick start

Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK 5 with ARM Compiler 5, build, and flash through SWD. Connect a J-Link and view logs in RTT Viewer on channel 0. Connect SEGGER SystemView to the target over SWD and start recording from the host. `SystemView_Init()` configures tracing before scheduler startup; recording does not start automatically at initialization.

## Reuse

Create tasks in `Z02_Business/Init/os_init.c` through `OS_Init()`. The status task toggles the PC13 LED and logs every 500 ticks. Board drivers live in `Z01_BSP`, and middleware lives in `Middlewares_User`.

Customize trace descriptions and frequency settings in `Middlewares_User/SystemView/Src/SEGGER_SYSVIEW_Config_FreeRTOS.c`. After CubeMX regeneration, retain TIM1, `OS_Init()`, logging and SystemView initialization, RTT redirection, trace hooks, source groups, and include paths. Keep the generated default task disabled and verify timestamp settings when changing clocks.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
