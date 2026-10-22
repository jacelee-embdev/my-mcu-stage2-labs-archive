**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## Overview

This STM32F4 Standard Peripheral Library (SPL) bootloader runs at
`0x08000000`. It receives encrypted firmware through USART1 and YMODEM,
stores it in backup region B, decrypts it into run region A, and transfers
control to the application at `0x08008000`. The current firmware also enables
a repeating SPI1 and W25Q64 erase/write test when no update is requested.

## Features

- Receives encrypted firmware through SecureCRT YMODEM transfer.
- Handles metadata, 128/1024-byte data packets, ACK, NAK, and CA controls.
- Erases backup region B, writes ciphertext, and verifies reads.
- Validates package and APP sizes, then decrypts into A with read-back checks.
- Validates the application stack, then switches VTOR and MSP before handover.
- Provides polled USART1 communication on PA9/PA10 at 115200 baud.
- Uses PA5/PA6/PA7 for SPI1 and PA4 for software chip select; reads the W25Q
  ID and tests erase, write, and read-back operations.
- Provides PA0 button, PC13 LED, and DWT/SysTick delay drivers.
- Sends runtime logs through EasyLogger and SEGGER RTT.

## Flash Layout

| Region | Start | Size | Address range |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

This table describes internal MCU flash. The external W25Q test erases the
4 KB sector at `0x007FF000`.

## Build, Test, and Update

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` in Keil and build it.
2. Download the bootloader through J-Link/SWD. Download it again after a
   full-chip erase.
3. Reset and check RTT: a W25Q64 reports ID `61206` (`0xEF16`). Each test pass
   writes, erases, writes again, and reads back. `write` rises by 2 per pass;
   matching `read` produces `OK`. The loop then waits one second.
4. For encrypted updates, build an APP BIN linked at `0x08008000` and no
   larger than 96 KB. Encrypt it with the expected key, IV, and package format.
5. Hold PA0 while resetting the board, then select **Transfer → Send Ymodem**
   in SecureCRT and send the encrypted file. On success, the bootloader
   decrypts it from B into A and starts the APP.

## Project Layout

```text
Core/                    Entry point, GPIO, Flash, USART, and SPI1
BSP/W25QXX/             W25Q ID, erase/write access, and capacity parameters
SYSTEM/delay/            DWT and SysTick delay module
Drivers/                 CMSIS and STM32F4 SPL
Middlewares_User/Ymodem/      YMODEM receiver and serial helpers
Middlewares_User/Algorithms/AES/  AES decryption
Middlewares_User/             EasyLogger and SEGGER RTT
Tasks/Bootloader/        Firmware decryption, installation, and handover
Tasks/Debug/             EasyLogger initialization
MDK-ARM/                 Keil project files
```

## Protocol Notes

The first encrypted 16-byte block carries the APP plaintext size; the rest
contains 16-byte ciphertext blocks. The YMODEM receiver does not yet calculate
and compare the CRC16 trailer of incoming packets.
The fallback loop remains under `#if 0`; the W25Q test runs under `#if 1`.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
