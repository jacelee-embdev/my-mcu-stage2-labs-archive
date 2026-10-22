**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## Overview

This STM32F4 Standard Peripheral Library (SPL) project runs from the beginning of Flash and transfers control to the application at `0x08008000`.

## Features

- Reads and validates the initial main stack pointer from the application vector table.
- Reads the application `Reset_Handler` address from `0x08008004`.
- Disables interrupts and switches `SCB->VTOR` to the application vector table before handover.
- Resets the clock configuration, loads the application stack, and enters the application.
- Sends runtime logs to SEGGER RTT Terminal 0 through EasyLogger.
- Remains in the bootloader and prints diagnostic logs when the application is invalid.

## Flash Layout

| Region | Start | Size | Address range |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | `0x8000` (32 KB) | `0x08000000 ~ 0x08007FFF` |
| Application | `0x08008000` | `0x78000` (480 KB) | `0x08008000 ~ 0x0807FFFF` |

## Build and Flash

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` in Keil.
2. Build and flash the bootloader through J-Link/SWD.
3. Select **Erase Sectors** for Flash Download to preserve the application partition.
4. Flash the application, then reset the board. The bootloader will jump to it automatically.

## Project Layout

```text
Core/                    Startup code and program entry
Drivers/                 CMSIS and STM32F4 SPL
Middlewares/             EasyLogger and SEGGER RTT
Tasks/Bootloader/        Application validation and handover
Tasks/Debug/             EasyLogger initialization
MDK-ARM/                 Keil project files
```

## Verification

Verified on hardware: the bootloader switches the vector table and main stack correctly, then runs the application at `0x08008000`.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
