**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Application

## Overview

This STM32F411CEU6 application is launched by the bootloader. It uses STM32 HAL, CMSIS-RTOS2, and FreeRTOS, and is linked at `0x08008000`.

## Features

- Re-enables the global interrupts disabled by the bootloader on entry to `main()`.
- Initializes HAL, the 100 MHz system clock, GPIO, USART1, and FreeRTOS.
- Prints an EasyLogger test message every 100 ms from the default task.
- Toggles the PC13 LED while the active-low PA0 button is pressed, verifying GPIO and task execution.
- Writes complete EasyLogger messages directly to SEGGER RTT Up Buffer 0, including time, level, tag, file, line, and function fields.

## Flash Layout

| Region | Start | Size | Address range |
|---|---:|---:|---:|
| Application | `0x08008000` | `0x78000` (480 KB) | `0x08008000 ~ 0x0807FFFF` |

The application vector table is located at `0x08008000`. The bootloader points `SCB->VTOR` to this address before handover, and the application re-enables interrupts during startup.

## Build and Flash

1. Flash the companion bootloader first.
2. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
3. Build and flash the application through J-Link/SWD with **Erase Sectors** selected.
4. Reset the board and view the output in SEGGER RTT Viewer Terminal 0.
5. Press the PA0 button and confirm that the PC13 LED blinks.

## Project Layout

```text
Core/                    HAL, FreeRTOS, GPIO, USART, and application entry
Tasks/Debug/             EasyLogger initialization and formatting
Middlewares_User/        EasyLogger and SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil project files
```

## Verification

Verified on hardware: the bootloader enters the application successfully, RTT logs are printed continuously, and the button/LED behavior works correctly.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
