**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Bootloader: SFUD Porting and OTA Updates

## Project Description

##### Introduction

A bare-metal Bootloader for STM32F411CEU6 using SPL. It accesses W25Q external Flash through SFUD to receive, decrypt, install, and boot encrypted firmware.

##### Main Features

- Receive encrypted firmware over USART1 / YMODEM, using external A/B regions for staging and decryption.
- Install firmware into internal Flash and confirm its first startup using EEPROM state and the watchdog.
- Route W25Q management interfaces through SFUD and restore a backed-up application if installation fails.

##### Repository Structure

```text
Core/                    Startup and SPL peripheral interfaces
BSP/W25QXX/              External Flash management interfaces
Middlewares_User/SFUD/   SFUD library, port, and adapter
Tasks/Bootloader/        OTA state machine and firmware installation
MDK-ARM/                Keil project
```

## Usage Guide

##### Environment and Dependencies

STM32F411CEU6, W25Q, and AT24C02; Keil MDK 5 / ARMCC 5.06 update 7, J-Link, RTT Viewer, a YMODEM-capable serial tool, and the matching firmware encryption tool.

##### Hardware Connection and Configuration

| Peripheral | Connection and configuration |
| --- | --- |
| W25Q SPI NOR | PA4: CS; PA5: SCK; PA6: MISO; PA7: MOSI |
| AT24C02 EEPROM | PB8: SCL; PB9: SDA, software I²C |
| USART1 | PA9: TX; PA10: RX, 115200, 8N1 |
| Button / LED | PA0: active-low button; PC13: status LED |

Use 3.3 V logic and a common ground for the board, peripherals, and USB-to-UART adapter.

##### Build, Run, and Observe

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx`, build, and flash the Bootloader using J-Link.
2. Link the APP at `0x08008000`, generate its BIN, and encrypt it into a `.mxxx` package. Press the button when prompted by the Bootloader and send the APP1 package over YMODEM.
3. Observe installation and APP1 startup in RTT Viewer, then download APP2 from APP1 and trigger the update.

Hardware regression and integration tests passed: initial APP1 installation by the Bootloader → APP2 reception in APP1 → APP2 running after reset.

##### Porting Notes

Hardware adaptation is in `Middlewares_User/SFUD/port/sfud_port.c`, device configuration in `inc/sfud_cfg.h`, and business interfaces in `BSP/W25QXX/w25qxx_Handler.c`. Each external A/B region is 64 KiB; the management layer uses 4 KiB erase units. When changing platforms, review SPI, delays, partitions, and the APP link address together.

## Author's Notes

None.

##### Project-Related Reflections

None.

##### Other Reflections

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
