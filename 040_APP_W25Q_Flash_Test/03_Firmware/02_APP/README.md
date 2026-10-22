**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Application — W25Qxx Test

## Overview

This HAL + CMSIS-RTOS2 + FreeRTOS APP integrates W25Qxx through SPI1.
The current test reads the device ID; the AT24C02 EEPROM software I2C driver
and test code are also retained.

**The APP is linked and flashed at `0x08008000` with the Bootloader. For this
standalone test, its start was temporarily changed to `0x08000000` to flash
plaintext directly, avoiding encrypted firmware creation and host YMODEM transfer.**

## Features

- Initializes HAL, the 100 MHz system clock, GPIO, USART1, SPI1, and FreeRTOS.
- The default task toggles PC13 and calls `W25Qx_Read_ID()` about every two seconds.
- PA5/PA6/PA7 are SCK/MISO/MOSI, and PA4 is software chip select; SPI1 uses mode 0
  with a prescaler of 256.
- EasyLogger/RTT prints `w25q_id`; the expected W25Q64 ID is `EF16`.
- The W25Qxx driver provides reset, parameter discovery, read, page-program,
  and erase APIs; the default task only tests ID reads.
- The retained EEPROM driver uses PB8/PB9; its `ee_test()` last-page test is in `#if 0`.

Currently, `main.c` calls `W25Qx_Init()` before `MX_GPIO_Init()` / `MX_SPI1_Init()`
without checking its return value, so this call does not confirm successful
startup reset or parameter discovery. The default task reads the ID after
peripheral initialization; the current test does not verify data read/write or erase.

## Flash Layout

| Mode | Link and flash address | Configured size |
|---|---:|---:|
| APP with Bootloader | `0x08008000` | `0x18000` (96 KB) |
| Standalone test (current) | `0x08000000` | `0x18000` (96 KB) |

Standalone testing occupies the original Bootloader region. In combined mode,
the Bootloader decrypts the package into `0x08008000` and sets `SCB->VTOR` and
MSP before handover; the APP re-enables interrupts during startup.

## Build and Flash

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
2. For standalone testing, check that IROM1 starts at `0x08000000` and rebuild.
3. Connect W25Q to the SPI1 pins above and flash plaintext through J-Link/SWD.
   Reset and view the `w25q_id` logs in RTT Viewer Terminal 0.
4. For combined mode, restore `0x08008000` and rebuild the APP (at most 96 KB).
   Reflash the Bootloader and send a matching encrypted package through YMODEM.

## Project Layout

```text
Core/                    HAL, FreeRTOS, GPIO, USART, SPI, and application entry
BSP/W25Qxx/              SPI flash driver; default task reads the device ID
BSP/AT24Cxx/             Retained software I2C, EEPROM driver, and last-page test
Tasks/Debug/             EasyLogger initialization and formatting
Middlewares_User/        EasyLogger and SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
