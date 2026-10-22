**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## Overview

This STM32F4 Standard Peripheral Library (SPL) bootloader runs at
`0x08000000` and provides application handover and peripheral verification.

## Features

- Validates and transfers control to the application at `0x08008000`.
- Reads the PA0 button and controls the PC13 LED.
- Provides polled USART1 communication on PA9/PA10 at 115200 baud.
- Erases internal Flash sectors and programs 32-bit words.
- Provides microsecond, millisecond, and second delays using DWT and SysTick.
- Sends logs through EasyLogger and SEGGER RTT.

## Flash Layout

| Region | Start | Size | Address range |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| Application | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## Build and Flash

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` in Keil.
2. Build and download through J-Link/SWD.
3. Select **Erase Sectors** in Flash Download settings.

## Project Layout

```text
Core/                    Entry point, GPIO, Flash, and USART
SYSTEM/delay/            DWT and SysTick delay module
Drivers/                 CMSIS and STM32F4 SPL
Middlewares/             EasyLogger and SEGGER RTT
Tasks/Bootloader/        Application validation and handover
Tasks/Debug/             EasyLogger initialization
MDK-ARM/                 Keil project files
```

## Verification

Verified on hardware: the button and LED, USART1 `A/B` output, SEGGER RTT
logging, Flash sector erase, and `0x00000055` word programming operate correctly.

The current `main.c` is in peripheral-verification mode with application handover
disabled. Remove the sector 3 erase/write test before enabling the handover to
avoid corrupting application data.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
