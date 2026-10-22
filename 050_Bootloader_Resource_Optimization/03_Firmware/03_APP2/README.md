**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 APP2: SFUD Porting and OTA Application

## Project Description

##### Introduction

An OTA verification application copied from APP1, sharing its HAL, FreeRTOS, and SFUD implementation. The application task in `Tasks/APP/app.c` uses APP2 logs and a 200 ms delay.

##### Main Features

- Receive update packages over USART1 / YMODEM and queue writes to external region A, flushing the final buffer and checking the written length.
- Save update state in EEPROM, accept an update command, and reset into the Bootloader on a button press.
- Output `APP2 running...` through RTT; the application task toggles the LED and logs every 200 ms.

##### Notes for This Stage

This stage optimizes EasyLogger and SFUD resource usage in the Bootloader. APP2 remains the update target, identified by its `APP2 running...` logs and 200 ms application-task delay, and retains its existing logging and SFUD configurations. Hardware validation of the optimized Bootloader found no functional issues; see the [repository overview](../../README.md) for the changes and resource comparison.

##### Repository Structure

```text
Core/                    HAL initialization and entry point
BSP/W25Qxx/              External Flash management interfaces
Middlewares_User/SFUD/   SFUD library, port, and adapter
Tasks/APP/               Application task
Tasks/SWC_OTA/           YMODEM reception and update control
MDK-ARM/                Keil project
```

## Usage Guide

##### Environment and Dependencies

STM32F411CEU6, W25Q, and AT24C02; Keil MDK 5 / ARMCC 5.06 update 7, HAL, FreeRTOS, J-Link / RTT Viewer, serial YMODEM tools, and the matching firmware encryption tool.

##### Hardware Connection and Configuration

| Peripheral | Connection and configuration |
| --- | --- |
| W25Q SPI NOR | PA4: CS; PA5: SCK; PA6: MISO; PA7: MOSI |
| AT24C02 EEPROM | PB8: SCL; PB9: SDA, software I²C |
| USART1 | PA9: TX; PA10: RX, 115200, 8N1 |
| Button / LED | PA0: active-low button; PC13: status LED |

Use 3.3 V logic and a common ground for the board, peripherals, and USB-to-UART adapter.

##### Build, Run, and Observe

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` and build with the APP link address kept at `0x08008000`. Encrypt the generated BIN into a `.mxxx` package and install it through the Bootloader.
2. In this application, send the hexadecimal bytes `11 22 33` over UART, then send the encrypted package for the next APP using YMODEM. After receiving `44 55 66`, send `77 88 99` and press the PA0 button to reset.
3. Observe `OTA ready` and the installed application's running logs in RTT Viewer.

Hardware regression and integration tests passed: initial APP1 installation by the Bootloader → APP2 reception in APP1 → APP2 running after reset.

##### Porting Notes

`Middlewares_User/SFUD/port/sfud_port.c` adapts HAL SPI and bare-metal / FreeRTOS delays; `inc/sfud_cfg.h` configures the device; `BSP/W25Qxx/w25qxx_Handler.c` preserves local business interfaces. Region A is currently 64 KiB with 4 KiB erase units. When changing platforms, review peripheral initialization, partitions, and the RTOS execution context.

## Author's Notes

None.

##### Project-Related Reflections

None.

##### Other Reflections

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
