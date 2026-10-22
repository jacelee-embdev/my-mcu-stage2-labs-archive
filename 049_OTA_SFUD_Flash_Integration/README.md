**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 OTA: Driver Portability with SFUD

## Project Description

##### Introduction

This repository contains Bootloader, APP1, and APP2 firmware. SFUD reduces coupling between OTA business logic and the W25Q driver. The Bootloader uses SPL; the applications use HAL and FreeRTOS.

##### Main Features

- The Bootloader receives, decrypts, and installs encrypted firmware, managing EEPROM update state and first-start confirmation.
- The APP receives update packages and writes them to external Flash through SFUD; the Bootloader installs them after reset.
- APP2 is copied from APP1. Their application tasks use APP1 / APP2 logs and 500 / 200 ms delays, respectively.

##### Repository Structure

```text
03_Firmware/
├── 01_Bootloader/          Startup and firmware installation
├── 02_APP1/                Initial application and OTA reception
├── 03_APP2/                Update target for verification
└── 04_BIN_EncryptionBIN/   Encrypted APP1 / APP2 packages
```

## Usage Guide

##### Environment and Dependencies

STM32F411CEU6, W25Q SPI NOR, and AT24C02 EEPROM; Keil MDK 5 / ARMCC 5.06 update 7, J-Link / RTT Viewer, a YMODEM-capable serial tool, and the matching firmware encryption tool.

##### Hardware Connection and Configuration

| Peripheral | Connection and configuration |
| --- | --- |
| W25Q SPI NOR | PA4: CS; PA5: SCK; PA6: MISO; PA7: MOSI |
| AT24C02 EEPROM | PB8: SCL; PB9: SDA, software I²C |
| USART1 | PA9: TX; PA10: RX, 115200, 8N1 |
| Button / LED | PA0: active-low button; PC13: status LED |

Use 3.3 V logic and a common ground for the board, peripherals, and USB-to-UART adapter.

##### Build, Run, and Observe

1. Build [Bootloader](03_Firmware/01_Bootloader/README.md), [APP1](03_Firmware/02_APP1/README.md), and [APP2](03_Firmware/03_APP2/README.md) in that order, following their instructions. The APP link address is `0x08008000`.
2. Encrypt each APP BIN into a `.mxxx` package, flash the Bootloader, and install APP1 over YMODEM.
3. Download the encrypted APP2 package from APP1, trigger the update, reset, and confirm APP2 is running through RTT logs.

The complete flow passed hardware regression and integration tests using rebuilt and newly encrypted APP firmware.

##### Porting Notes

All three projects integrate SFUD in `Middlewares_User/SFUD/`. When changing platforms, adapt `port/sfud_port.c`, `inc/sfud_cfg.h`, and peripheral initialization while retaining local W25Q management interfaces. Review the 64 KiB regions, 4 KiB erase units, and APP link address together.

## Author's Notes

None.

##### Project-Related Reflections

None.

##### Other Reflections

None.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
