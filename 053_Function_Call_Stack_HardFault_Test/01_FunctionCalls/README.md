**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Function Calls and Stack Inspection

## Project Description

This STM32F411CEU6 experiment uses three nested function calls to inspect
argument passing, local variables, the stack pointer (SP), the link register
(LR), and stack changes as functions return.

The call chain is `main → CallStackDemo_EnterLevel1 → CallStackDemo_EnterLevel2 → CallStackDemo_EnterLevel3`.
Local variables contain recognizable markers. `volatile` and the no-inline
attribute preserve the objects and function boundaries for inspection.

##### Project Structure

```text
Business/          Call-stack demonstration module
Core/              main entry point, clock and peripheral configuration
Drivers/           HAL and CMSIS
MDK-ARM/           Keil project and startup files
Middlewares_User/  SEGGER RTT and EasyLogger
```

## Usage Guide

##### Environment and Connections

- STM32F411CEU6 development board with a 100 MHz system clock.
- Keil MDK 5 with ARMCC 5.06 update 7; HAL and CMSIS sources are included.
- Connect a debugger through SWD, including target reference voltage and ground.
- Optional UART: PA9 → USB-to-UART RX, PA10 → TX, GND → GND;
  use 3.3 V TTL levels, 115200 baud, 8N1.

##### Build, Run, and Observe

1. Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx), select `-O0`, enable debug information, then build, flash, and start debugging.
2. Set a breakpoint on `level3_marker = level3_marker;` inside `CallStackDemo_EnterLevel3()`, then reset and run.
3. Inspect Call Stack, Registers, and Memory. Switch stack frames to examine arguments and local variables.
4. Step out of each function and observe SP, LR, and call-stack changes. After returning to `main()`, its marker becomes `0x00456789`.

| Object | Marker |
|---|---|
| `main_stack_marker` before the call | `0x12345678` |
| `level1_marker` | `0xA1A2A3A4` |
| `level2_marker` | `0xB1B2B3B4` |
| `level3_marker` | `0xC1C2C3C4` |

Arguments may reside in registers or on the stack; check the disassembly
for their actual locations. The call test runs once at startup, followed by
debug messages every two seconds. Reset the board to repeat the test.

##### Porting Notes

Add `Business/call_stack_demo.c` to the target project and configure the header
search path. Adapt the clock, UART, and debugger settings to the target chip,
and verify compiler support for the no-inline attribute. Keep application
logic inside `USER CODE` regions when editing CubeMX-generated `main.c`.

## Author's Notes

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
