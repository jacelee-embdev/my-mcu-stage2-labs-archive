**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 SEGGER RTT Demo

## Overview

This project demonstrates SEGGER Real Time Transfer (RTT) on an STM32F411CEU6. The RTT sources are integrated into the Keil project, allowing debug messages to be viewed through J-Link and SWD without using a UART as the output channel.

The demo uses both `SEGGER_RTT_printf()` and a standard `printf()` redirected to RTT. Basic floating-point output is also enabled in `SEGGER_RTT_printf()`.

## RTT Integration

- The RTT sources and configuration are stored in `Middlewares/RTT`.
- `SEGGER_RTT.c` and `SEGGER_RTT_printf.c` are included in the Keil target.
- `Middlewares/RTT` is added to the compiler include path.
- `fputc()` in `Core/Src/usart.c` calls `SEGGER_RTT_PutChar()`, redirecting standard `printf()` output to RTT Up Buffer 0.
- `Core/Src/main.c` periodically outputs test strings and floating-point values.

## Project Layout

```text
Core/Src/main.c                RTT output demo
Core/Src/usart.c               printf redirection
Middlewares/RTT/               SEGGER RTT sources and configuration
MDK-ARM/STM32F411CEU6.uvprojx  Keil MDK project
```

## Quick Start

1. Install Keil MDK-ARM and the SEGGER J-Link Software and Documentation Pack.
2. Open `MDK-ARM/STM32F411CEU6.uvprojx`, then build and flash the firmware.
3. Start J-Link RTT Viewer and select USB, `STM32F411CE`, SWD, and automatic RTT Control Block detection.
4. Run or reset the target. RTT Terminal 0 will display output similar to:

```text
val:1.12
Test !
val:1.140000
```

The messages repeat approximately once per second.

## Environment

| Item | Configuration |
|---|---|
| MCU / clock | STM32F411CEU6 (Cortex-M4F), 100 MHz |
| Firmware | STM32Cube FW_F4 V1.28.3 |
| IDE / compiler | Keil MDK 5, ARM Compiler 5 |
| Debug interface | J-Link, SWD |

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
