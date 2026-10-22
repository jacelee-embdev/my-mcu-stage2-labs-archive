**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader and Application Demo

## Overview

This project contains an STM32F411CEU6 Bootloader and a standalone APP.
The Bootloader uses the STM32F4 Standard Peripheral Library (SPL). It
receives encrypted firmware through YMODEM, stores the ciphertext in
external W25Q64 flash, decrypts it into the MCU's internal APP run region,
and starts the APP. The APP uses STM32 HAL, CMSIS-RTOS2, and FreeRTOS.

```text
Power-on/reset
  → Bootloader (0x08000000) initializes USART1, SPI1, and W25Q64
  → PA0 requests an update: YMODEM receives ciphertext into W25Q64
  → Read and decrypt W25Q64 data into internal APP run flash (0x08008000)
  → Validate the APP stack, switch VTOR and MSP, start the FreeRTOS APP
  → No update or failed installation: wait for another PA0 request
```

## Storage Layout

Internal MCU flash:

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP run region | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| Receive-stage erase region | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

The encrypted image is stored sequentially in external W25Q64 starting at
offset `0x000000`. During reception, the MCU internal Flash Sector 5 is
erased; the ciphertext is stored in W25Q64.

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL Bootloader,
  YMODEM reception, W25Q64 ciphertext storage, APP installation, and handover.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS APP used as the
  target image for encrypted updates and application handover.

## Build and Run

1. Build `01_Bootloader` in Keil, download it to the MCU through J-Link/SWD,
   and connect SEGGER RTT Viewer for logs.
2. Build `02_APP` as a BIN linked at `0x08008000` with a plaintext size
   no greater than 96 KB. Encrypt it with the Bootloader's key, IV, and
   package format.
3. Hold PA0 while resetting the board. In SecureCRT, select
   **Transfer → Send Ymodem** and send the encrypted file.
4. The Bootloader reads the ciphertext from W25Q64, decrypts and installs
   it into the APP run region, then starts the APP after verification.
5. If no update is requested or installation fails, the Bootloader waits
   for another PA0 request.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader starting at 0x08000000
│   └── BSP/W25QXX/   External W25Q driver and buffered access interface
└── 02_APP/            HAL + FreeRTOS APP starting at 0x08008000
```

## Current Implementation Notes

- The YMODEM receiver does not yet calculate or compare incoming packet
  CRC16 trailers.
- After receiving the metadata packet, `ymodem.c` erases MCU internal
  Flash Sector 5 and writes the ciphertext to external W25Q64.
- External flash reads rely on read and write indices held in RAM, so
  reception and installation must complete within one powered session.
  See the [Bootloader documentation](03_Firmware/01_Bootloader/README.md)
  for the detailed flow.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
