**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Application

## Overview

This STM32F411CEU6 application is launched by the bootloader and uses STM32
HAL, CMSIS-RTOS2, and FreeRTOS. It is linked into run region A at `0x08008000`,
with a maximum size of 96 KB.

## Features

- Re-enables the global interrupts disabled by the bootloader on entry to `main()`.
- Initializes HAL, the 100 MHz system clock, GPIO, USART1, and FreeRTOS.
- Prints an EasyLogger test message every 100 ms from the default task.
- Toggles the PC13 LED while the active-low PA0 button is pressed, verifying GPIO and task execution.
- Writes complete EasyLogger messages directly to SEGGER RTT Up Buffer 0, including time, level, tag, file, line, and function fields.

## Flash Layout

| Region | Start | Size | Address range |
|---|---:|---:|---:|
| APP Run | `0x08008000` | `0x18000` (96 KB) | `0x08008000 ~ 0x0801FFFF` |

The application vector table is located at `0x08008000`. The bootloader copies
the image from backup region B to this region, points `SCB->VTOR` to it before
handover, and the application re-enables interrupts during startup.

## Build and Flash

1. Flash the companion bootloader first.
2. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
3. Build the application; the generated image must not exceed 96 KB.
4. Send the BIN image through YMODEM, or flash the APP directly through
   J-Link/SWD.
5. Reset the board and view runtime logs in SEGGER RTT Viewer Terminal 0.

## Project Layout

```text
Core/                    HAL, FreeRTOS, GPIO, USART, and application entry
Tasks/Debug/             EasyLogger initialization and formatting
Middlewares_User/        EasyLogger and SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
