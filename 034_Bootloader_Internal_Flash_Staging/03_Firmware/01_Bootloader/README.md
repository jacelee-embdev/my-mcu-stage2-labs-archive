**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## Overview

This STM32F4 Standard Peripheral Library (SPL) bootloader runs at
`0x08000000`. It receives an application BIN image through USART1 and YMODEM,
writes it to backup region B, copies it to run region A, and transfers control
to the application at `0x08008000`.

## Features

- Receives application BIN images through SecureCRT YMODEM transfer.
- Handles metadata, 128/1024-byte data packets, ACK, NAK, and CA controls.
- Erases backup region B, programs 32-bit words, and verifies reads.
- Copies the image from backup region B to run region A with read-back checks.
- Validates the application stack, then switches VTOR and MSP before handover.
- Provides polled USART1 communication on PA9/PA10 at 115200 baud.
- Provides PA0 button, PC13 LED, and DWT/SysTick delay drivers.
- Sends runtime logs through EasyLogger and SEGGER RTT.

## Flash Layout

| Region | Start | Size | Address range |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

## Build and Update

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` in Keil and build it.
2. Download the bootloader through J-Link/SWD. Download it again after a
   full-chip erase.
3. Reset the board and confirm that RTT Viewer prints
   `Bootloader running...`.
4. In SecureCRT, select **Transfer → Send Ymodem** and send an application BIN
   image linked at `0x08008000` and no larger than 96 KB.
5. The bootloader writes the image to region B, copies it to region A, and
   starts the application.

## Project Layout

```text
Core/                    Entry point, GPIO, Flash, and USART
SYSTEM/delay/            DWT and SysTick delay module
Drivers/                 CMSIS and STM32F4 SPL
Middlewares/Ymodem/      YMODEM receiver and serial helpers
Middlewares/             EasyLogger and SEGGER RTT
Tasks/Bootloader/        B-to-A copy, application validation, and handover
Tasks/Debug/             EasyLogger initialization
MDK-ARM/                 Keil project files
```

## Protocol Notes

This implementation receives firmware only. It transmits only the YMODEM
control bytes required by the receiver, and does not yet calculate and compare
the CRC16 trailer of incoming packets.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
