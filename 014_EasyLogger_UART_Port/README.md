**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 EasyLogger Porting Demo

## Overview

This project demonstrates how to port and use EasyLogger in a bare-metal STM32F411CEU6 application. The project is generated with STM32CubeMX and built with Keil MDK and ARM Compiler 5. Colored logs at different levels are sent to a PC serial terminal through USART1. Each log can include its level, module tag, system uptime, source file, line number, and function name, making runtime observation and problem diagnosis easier.

## EasyLogger Configuration

- The EasyLogger 2.2.99 sources are stored in `Middlewares/easylogger`. The core sources and STM32 port are included in the Keil project.
- `Middlewares/easylogger/inc` is included in the compiler header search path.
- `elog_port_output()` sends logs through `printf()`, while the standard `printf()` function is redirected to USART1 through `fputc()`.
- `elog_port_get_time()` reads the millisecond uptime with `HAL_GetTick()` and formats it as seconds and milliseconds.
- Log colors, function names, source paths, and line numbers are enabled. The newline sequence is configured as `\r\n`.
- The current project uses immediate synchronous output. Asynchronous and buffered output are disabled.

## Project Layout

```text
Core/Src/main.c                  EasyLogger initialization and log tests
Core/Src/usart.c                 USART1 setup and printf redirection
Middlewares/easylogger/inc/      EasyLogger headers and configuration
Middlewares/easylogger/src/      EasyLogger core sources
Middlewares/easylogger/port/     STM32 platform port
MDK-ARM/STM32F411CEU6.uvprojx    Keil MDK project
```

## Running the Demo

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` with Keil, then build and flash the firmware.
2. Run or reset the board and open a serial terminal with ANSI color support, such as MobaXterm.
3. Select the serial port connected to the board and set the baud rate to 115200.
4. The application outputs Assert, Error, Warn, Info, Debug, and Verbose test logs approximately every two seconds.

Example output:

```text
I/main            [0.012] (../Core/Src/main.c:95 test_log)Hello EasyLogger!
```

## Development Environment

| Item | Configuration |
|---|---|
| MCU / clock | STM32F411CEU6 (Cortex-M4F), 100 MHz |
| Logging library | EasyLogger 2.2.99 |
| IDE / compiler | Keil MDK 5, ARM Compiler 5 |
| Serial port | USART1, PA9/PA10, 115200 bps |
| Programming / debug | J-Link, SWD |

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
