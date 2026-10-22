**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Application — AT24C02 Test

## Overview

This HAL + CMSIS-RTOS2 + FreeRTOS APP integrates AT24C02 over software I2C.

**The APP is linked and flashed at `0x08008000` with the Bootloader. For this
standalone test, its start was temporarily changed to `0x08000000` to flash
plaintext directly, avoiding encrypted firmware creation and host YMODEM transfer.**

## Features

- Initializes HAL, the 100 MHz system clock, GPIO, USART1, and FreeRTOS.
- The default task toggles PC13 and calls `ee_test()` about every two seconds.
- Uses PB8 for SCL and PB9 for SDA; checks ACK and prints addresses `0x50` / `0xA0`.
- The EEPROM holds 256 bytes in 8-byte pages; testing only overwrites `0xF8–0xFF`.
- Writes the next incrementing sequence each round, polls ACK, then reads back
  and compares every byte. EasyLogger/RTT prints the values and results.

Hardware testing confirmed ACK and changing data across rounds. All eight bytes
of one round, `0x18–0x1F`, matched on readback:
`EEPROM read/write test successful.` The device does not support chip ID reads.

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
3. Flash plaintext firmware through J-Link/SWD, reset, and view RTT Viewer Terminal 0.
4. For combined mode, restore `0x08008000` and rebuild the APP (at most 96 KB).
   Reflash the Bootloader and send a matching encrypted package through YMODEM.

## Project Layout

```text
Core/                    HAL, FreeRTOS, GPIO, USART, and application entry
BSP/AT24Cxx/             Software I2C, EEPROM driver, and last-page test
Tasks/Debug/             EasyLogger initialization and formatting
Middlewares_User/        EasyLogger and SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
