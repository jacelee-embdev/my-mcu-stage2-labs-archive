**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## Overview

This Bootloader uses the STM32F4 Standard Peripheral Library (SPL) and starts
at `0x08000000`. It receives encrypted firmware over USART1 and YMODEM into
external W25Q64 flash, then decrypts it into the internal APP run region
(`0x08008000`) and attempts handover after readback verification. PA0 can
request another update if none was requested or installation failed.
project 043 adds SPL software I²C for AT24C02. Device ACK and last-page read/write
tests passed on hardware; EEPROM metadata is not yet connected to installation.

## Features

- Receives encrypted firmware through SecureCRT YMODEM, handling metadata,
  128/1024-byte packets, and ACK, NAK, and CA control characters.
- Accesses W25Q64 over SPI1 with 4 KB sector buffering and a final partial write.
- Checks package and APP sizes, decrypts block by block, programs 32-bit words
  into the internal APP run region, and verifies them by reading back.
- Checks the APP initial stack pointer, switches VTOR/MSP, and jumps to its entry.
- Uses USART1 on PA9/PA10 at 115200 baud, SPI1 on PA5/PA6/PA7 with PA4 chip
  select, the PA0 update button, and the PC13 LED.
- Accesses AT24C02 through open-drain software I²C on PB8 (SCL)/PB9 (SDA).
  Its seven-bit address is `0x50`; write/read address bytes are `0xA0/0xA1`.
  Detection, read, write, and erase interfaces are provided.
- Provides DWT/SysTick delays, EasyLogger, and SEGGER RTT logs.

## Storage Layout

Internal MCU flash:

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP run region | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| Receive-stage erase region | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

Ciphertext is stored sequentially from W25Q64 offset `0x000000`; reception
still erases internal Sector 5. AT24C02 has 256 bytes in 8-byte pages.
`ee_test()` overwrites only the last page, `0xF8–0xFF`, without accessing
the APP's existing OTA metadata at lower addresses.

## Build and Validate

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` in Keil, build, and flash through
   J-Link/SWD. SCL and SDA require external pull-up resistors.
2. In RTT Viewer Terminal 0, check `EEPROM ACK: addr7=0x50`, write/read values
   at `0xF8–0xFF`, and `EEPROM read/write test successful.`.
   `main()` detects the EEPROM at startup and tests once if execution reaches
   the point before the waiting loop.
3. To validate the existing encrypted update, build an APP BIN linked at
   `0x08008000` with plaintext no larger than 96 KB, then encrypt it using
   the matching key, IV, and package format.
4. Hold PA0 during reset and choose **Transfer → Send Ymodem** in SecureCRT.
   The Bootloader decrypts and installs from W25Q64, then attempts handover
   after verification. Otherwise, it waits for another PA0 request.

On 2026-10-04, device ACK, write-completion polling, and last-page read/write
tests passed after migration. Formatting/comments were then updated;
the four driver files are now `V1.1`, and `main.c` is `v1.7`.

## Current Implementation Notes

- GPIO access uses SPL. `ee_test()` polls write completion within a 10 ms
  window using `SysTickUpdateTime`, incremented every millisecond.
  Call `delay_init()` first and keep SysTick interrupts running. A successful
  write return means that data transmission has finished.
- AT24C02 has no chip ID read command; the message does not indicate
  detection failure. `There is no valid app...` is a waiting-loop message
  and should be assessed separately from EEPROM results.
- The Bootloader does not yet consume the APP's OTA state or file size.
  W25Q64 indices remain in RAM, so existing reception and installation
  must complete within one powered session.
- The first encrypted 16-byte block carries the APP plaintext size.
  Packet CRC16 trailers are not checked. After metadata reception,
  `Flash_erase(APP_BACKUP_BASE_ADDR, size)` still erases internal flash;
  `W25Q64_WriteData()` stores the ciphertext in external flash.

## Project Layout

```text
Core/                            Entry point, GPIO, Flash, USART, and SPI1
BSP/AT24Cxx/                     SPL software I²C, AT24C02 driver and tests
BSP/W25QXX/                      W25Q driver and buffered access
SYSTEM/delay/                    DWT and SysTick delay module
Drivers/                         CMSIS and STM32F4 SPL
Middlewares_User/Ymodem/          YMODEM receiver and serial helpers
Middlewares_User/Algorithms/AES/  AES decryption
Middlewares_User/                EasyLogger and SEGGER RTT
Tasks/Bootloader/                Firmware installation and APP handover
Tasks/Debug/                     EasyLogger initialization
MDK-ARM/                         Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
