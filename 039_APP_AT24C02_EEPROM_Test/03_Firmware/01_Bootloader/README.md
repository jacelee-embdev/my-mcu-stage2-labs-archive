**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## Overview

This Bootloader uses the STM32F4 Standard Peripheral Library (SPL) and starts
at `0x08000000`. It receives encrypted firmware through USART1 and YMODEM
and stores the ciphertext in external W25Q64 flash. After reception, it reads
the image from W25Q64, decrypts it into the MCU's internal APP run region
(`0x08008000`), verifies the programmed data, and starts the APP. If no update
is requested or installation fails, it waits for another request through PA0.

## Features

- Receives encrypted firmware through SecureCRT YMODEM transfer and handles
  metadata, 128/1024-byte packets, and ACK, NAK, and CA control characters.
- Accesses W25Q64 over SPI1. Incoming data is buffered in 4 KB sectors;
  remaining data is written when the transfer ends.
- Validates the encrypted package and decrypted APP sizes, decrypts the
  image block by block, programs 32-bit words into the internal APP run
  region, and verifies them by reading back.
- Validates the APP initial stack pointer, switches VTOR and MSP, and jumps
  to the APP entry point.
- Uses polled USART1 on PA9/PA10 at 115200 baud; SPI1 uses PA5/PA6/PA7
  and PA4 as the W25Q software chip select.
- Provides the PA0 button, PC13 LED, DWT/SysTick delays, EasyLogger, and
  SEGGER RTT logs.

## Storage Layout

Internal MCU flash:

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP run region | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| Receive-stage erase region | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

The encrypted image is written sequentially to external W25Q64 starting at
offset `0x000000`. Its occupied space depends on the encrypted file length.
During reception, the MCU internal Flash Sector 5 is erased; the ciphertext
is stored in W25Q64.

## Build and Update

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` in Keil, build the project,
   and download the Bootloader through J-Link/SWD.
2. Build an APP BIN linked at `0x08008000` with a plaintext size no greater
   than 96 KB. Create the update file using the Bootloader's key, IV, and
   encrypted package format.
3. Hold PA0 while resetting the board. In SecureCRT, select
   **Transfer → Send Ymodem** and send the encrypted file.
4. After reception, the Bootloader reads the ciphertext from W25Q64,
   decrypts and installs the APP, then starts it after verification.
5. If no update is requested or installation fails, the Bootloader remains
   active and waits for another PA0 request.

## Current Implementation Notes

- The first encrypted 16-byte block carries the APP plaintext size;
  subsequent ciphertext is arranged in 16-byte blocks. The YMODEM receiver
  does not yet calculate or compare packet CRC16 trailers.
- After receiving the metadata packet, `ymodem.c` calls
  `Flash_erase(APP_BACKUP_BASE_ADDR, size)` to erase the corresponding
  internal MCU flash sector; `W25Q64_WriteData()` writes the ciphertext
  to external W25Q64.
- `W25Q64_ReadData()` relies on read and write indices held in RAM, so
  reception and installation must complete within one powered session.

## Project Layout

```text
Core/                            Entry point, GPIO, Flash, USART, and SPI1
BSP/W25QXX/                     W25Q driver and buffered access interface
SYSTEM/delay/                    DWT and SysTick delay module
Drivers/                         CMSIS and STM32F4 SPL
Middlewares_User/Ymodem/         YMODEM receiver and serial helpers
Middlewares_User/Algorithms/AES/ AES decryption
Middlewares_User/                EasyLogger and SEGGER RTT
Tasks/Bootloader/                Firmware installation and APP handover
Tasks/Debug/                     EasyLogger initialization
MDK-ARM/                         Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
