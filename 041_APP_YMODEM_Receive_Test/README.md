**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 APP YMODEM Reception

## Overview

This project integrates YMODEM reception into the STM32F411CEU6 HAL + FreeRTOS
APP using USART1 DMA, IDLE events, and a queue to receive packets into SRAM.
A demonstration task runs LED, AT24C02 EEPROM read/write tests, and W25Q ID reads
in parallel. The receive task handles protocol replies without retaining the
full file or writing flash.

The companion SPL Bootloader retains its encrypted update flow: receive an
encrypted image through YMODEM, store it in W25Q64, then decrypt/install it
into the internal APP run region and start the APP.

```text
Current standalone test: reset → FreeRTOS APP (0x08000000)
                         ├─ Demo: LED, EEPROM test, W25Q ID / RTT logs
                         └─ RX: USART1 DMA → IDLE event → queue → SRAM parsing/replies
Encrypted update: Bootloader (0x08000000) → PA0 requests reception into W25Q64
                  → Decrypt into APP (0x08008000) → Set VTOR/MSP and start APP
```

## Storage Layout

Internal MCU flash layout for combined Bootloader/APP operation:

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP run region | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| Receive-stage erase region | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

The Bootloader stores ciphertext sequentially from W25Q64 offset `0x000000`
and still erases internal Sector 5 during reception. The standalone APP currently
starts at `0x08000000` with 96 KB configured. Its YMODEM receiver only reuses
a 1030-byte SRAM packet buffer, without using these storage regions.

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL Bootloader,
  YMODEM reception, W25Q64 ciphertext storage, APP installation, and handover.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS APP.
  `os_init()` creates demonstration and receive tasks, reusing W25Qxx/AT24C02
  drivers. YMODEM reception, comment/format cleanup, and on-device regression
  are complete for this stage.

## Build and Run

**Standalone testing flashes a plaintext APP at `0x08000000`; its receive task
automatically waits for YMODEM files. Sending a BIN tests communication and does
not install or execute it. Restore `0x08008000` for combined Bootloader operation.**

1. Open `03_Firmware/02_APP/MDK-ARM/STM32F411CEU6.uvprojx` in Keil, check
   IROM1 starts at `0x08000000`, build, and flash through J-Link/SWD.
2. Connect adapter TX/RX to PA10/PA9 with common ground. Configure SecureCRT
   for 115200, 8N1, no flow control; periodic `C` characters appear after reset.
3. Use **Transfer → Send Ymodem** to send a BIN and check completion/errors.
   Further `C` characters indicate that the next receive session is waiting.
4. View business logs in RTT Viewer Terminal 0. W25Q SCK/MISO/MOSI use
   PA5/PA6/PA7, with PA4 chip select; W25Q64 ID is `EF16`. EEPROM uses
   PB8/PB9; `ee_test()` is enabled and writes/reads the last page, `0xF8–0xFF`.

A 22,668-byte BIN transfer through SecureCRT reached 100% with 0 Errors;
regression after the comment/format update also passed. This validates the
receive flow under the tested conditions.

For combined updates, restore the APP address to `0x08008000`, rebuild an APP
of at most 96 KB, create a matching encrypted package, and reflash the Bootloader.
Hold PA0 at reset and send the encrypted file through YMODEM.
See the Bootloader README for details.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/       SPL Bootloader starting at 0x08000000
│   └── BSP/W25QXX/      External flash driver and buffered access
└── 02_APP/              HAL + FreeRTOS APP; standalone start is 0x08000000
    ├── BSP/            UART_Handler, W25Qxx, AT24Cxx
    ├── SysSupport/     OS_init queue/task creation, Debug logging setup
    ├── Tasks/          APP demonstration task, SWC_OTA receive task
    └── Middlewares_User/ Ymodem, EasyLogger, SEGGER RTT
```

## Current Implementation Notes

- APP reception waits up to 2000 ms and is prepared before replies.
  Timeouts/session exits stop reception and clear notifications. The receive task
  has a 512-word stack and delays 10 ms after each session returns.
- Each APP IDLE event is treated as a complete packet, without accumulating
  fragments. Neither receiver currently verifies packet CRC16; successful
  transfers do not establish byte-for-byte integrity.
- The demonstration task delays about 500 ms after each iteration. EEPROM
  last-page read/write testing is enabled; W25Q testing still only reads its ID.
  See the [APP documentation](03_Firmware/02_APP/README.md) for initialization order.
- APP `ymodem.c` no longer depends on flash reception APIs. The Bootloader's
  same-named file still erases Sector 5 and writes W25Q64; the flows differ.
- Bootloader external flash indices remain in RAM, so reception and installation
  must complete within one powered session. See the
  [Bootloader documentation](03_Firmware/01_Bootloader/README.md) for the full flow.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
