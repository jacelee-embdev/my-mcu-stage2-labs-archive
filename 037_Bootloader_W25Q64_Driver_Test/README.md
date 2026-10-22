**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader and Application Demo

## Overview

This project contains an STM32F411CEU6 bootloader and a standalone application.
The SPL bootloader downloads encrypted firmware to backup region B through
YMODEM, decrypts it into run region A, and transfers control to it. The
application uses STM32 HAL, CMSIS-RTOS2, and FreeRTOS. The current bootloader
also runs an SPI1 and W25Q64 erase/write regression test.

```text
Power-on/reset
  → Bootloader (0x08000000) initializes SPI1/W25Q and checks PA0
  → Update requested: YMODEM receives ciphertext into APP Backup
  → Decrypt into APP Run, validate the stack, switch VTOR/MSP, start the APP
  → No update: print W25Q ID and repeatedly erase, write, and read back
```

## Flash Layout

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

These regions are internal MCU flash. The W25Q test uses the external chip's
4 KB sector starting at `0x007FF000`.

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL bootloader,
  YMODEM encrypted updates, and SPI1/W25Q erase/write testing.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS application used as
  the target image for encrypted updates and application handover.

## Build and Run

1. Build and download `01_Bootloader` through J-Link/SWD, then connect
   SEGGER RTT Viewer.
2. After reset, check the W25Q64 ID `61206` (`0xEF16`). In the repeating log,
   `write` rises by 2 each pass and should match `read` with an `OK` result.
3. Each pass erases the external W25Q test sector, deleting its prior data,
   then waits one second.
4. For an update, build `02_APP` as a BIN linked at `0x08008000` and no larger
   than 96 KB. Encrypt it with the bootloader's key, IV, and package format.
5. Hold PA0 while resetting the board, then use **Transfer → Send Ymodem** in
   SecureCRT to send the file. The bootloader installs and starts the APP.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL bootloader at 0x08000000
│   └── BSP/W25QXX/   External W25Q flash driver
└── 02_APP/            HAL + FreeRTOS application at 0x08008000
```

## Protocol Notes

The current YMODEM implementation is receive-oriented and does not yet
calculate and compare the CRC16 trailer of incoming packets.
The W25Q test loop is enabled; the original fallback loop is disabled by
conditional compilation.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
