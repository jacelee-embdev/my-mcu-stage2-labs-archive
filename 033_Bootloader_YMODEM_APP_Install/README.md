**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader and Application Demo

## Overview

This project contains an STM32F411CEU6 bootloader and a standalone application.
The SPL bootloader receives an application BIN image through YMODEM, programs
it into internal Flash, and transfers control to it. The STM32 HAL,
CMSIS-RTOS2, and FreeRTOS application verifies operation after the update.

```text
Power-on/reset
  → Bootloader (0x08000000) waits for a YMODEM transfer
  → Erase the application region and program the received image
  → Validate the application stack, then switch VTOR and MSP
  → Application Reset_Handler (0x08008000)
  → Start FreeRTOS, blink the LED, and output task logs
```

## Flash Layout

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| Application | 2–7 | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL bootloader,
  YMODEM reception, Flash programming, read-back verification, and handover.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS application used as
  the target image for YMODEM updates and application handover.

## Build and Run

1. Build `02_APP` to generate a BIN image linked at `0x08008000`.
2. Build and download `01_Bootloader` through J-Link/SWD.
3. Reset the board, then select **Transfer → Send Ymodem** in SecureCRT and
   send the application BIN image.
4. After the transfer, the bootloader programs and starts the application.
5. Confirm FreeRTOS operation through the blinking LED and SEGGER RTT logs.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL bootloader at 0x08000000
└── 02_APP/            HAL + FreeRTOS application at 0x08008000
```

## Verification

Hardware regression testing has passed: the SecureCRT YMODEM transfer completes
without errors, the application image is programmed into Flash, and the
bootloader starts the application successfully. The FreeRTOS task, LED blink,
and RTT logs then operate normally. Reconnect RTT Viewer if the application
logs do not appear immediately after the handover.

The current YMODEM implementation is receive-oriented and does not yet
calculate and compare the CRC16 trailer of incoming packets.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
