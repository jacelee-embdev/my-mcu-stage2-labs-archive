**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader and Application Demo

## Overview

This project contains an STM32F411CEU6 bootloader and a standalone application.
The SPL bootloader downloads an application BIN image to backup region B
through YMODEM, copies it to run region A, and transfers control to it. The
application uses STM32 HAL, CMSIS-RTOS2, and FreeRTOS.

```text
Power-on/reset
  → Bootloader (0x08000000) checks the update button
  → Receive the image through YMODEM into APP Backup
  → Copy APP Backup to APP Run and verify each programmed word
  → Validate the APP stack, then switch VTOR and MSP
  → APP Reset_Handler (0x08008000)
  → Start FreeRTOS, blink the LED, and output task logs
```

## Flash Layout

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL bootloader,
  YMODEM reception, A/B region copying, read-back verification, and handover.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS application used as
  the target image for YMODEM updates and application handover.

## Build and Run

1. Build `02_APP` to generate a BIN image linked at `0x08008000` with a
   maximum size of 96 KB.
2. Build and download `01_Bootloader` through J-Link/SWD.
3. Reset the board, then select **Transfer → Send Ymodem** in SecureCRT and
   send the application BIN image.
4. The bootloader writes the image to region B, copies it to region A, and
   starts the application.
5. Observe APP operation through the PC13 LED and SEGGER RTT logs.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL bootloader at 0x08000000
└── 02_APP/            HAL + FreeRTOS application at 0x08008000
```

## Protocol Notes

The current YMODEM implementation is receive-oriented and does not yet
calculate and compare the CRC16 trailer of incoming packets.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
