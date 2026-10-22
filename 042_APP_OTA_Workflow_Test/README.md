**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 In-APP Background Update Workflow Validation

## Overview

This STM32F411CEU6 HAL + FreeRTOS APP implements command-triggered YMODEM
reception, external W25Q64 data processing, AT24C02 metadata access, and a
key-triggered soft reset while the business task toggles an LED. The in-APP
background update workflow has passed on-device regression.

**This stage runs only the standalone APP, without Bootloader installation.
The downloaded file is not installed into the internal APP run region;
the original APP runs again after reset.** The companion SPL Bootloader retains
its encrypted update flow: receive ciphertext into W25Q64, decrypt/install, and hand over.

```text
Current standalone test: reset → FreeRTOS APP (0x08000000)
                         ├─ Business task: PC13 LED, about 500 ms per cycle
                         └─ 11 22 33 → C / dual-buffer YMODEM → external processing
                            → 44 55 66 (DUf) / EEPROM metadata → 77 88 99
                            → Press PA0 within about 20 s → reset → original APP
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

The standalone APP currently starts at `0x08000000` with 96 KB configured,
occupying the Bootloader region. Reception uses two 1030-byte SRAM packet buffers
and W25Q64 sector-buffered access, without installing the downloaded file into
internal flash. The companion Bootloader stores ciphertext from W25Q64 offset
`0x000000` and still erases internal Sector 5 during reception.

AT24C02 stores a one-byte OTA state at `0x00` and the four-byte little-endian new
APP file size at `0x01–0x04`. Current APP size at `0x05–0x08` remains reserved.
Startup writes `0x00`, and download writes `0x11`; the update-request state `0x22`
and Bootloader integration are not implemented yet.

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL Bootloader,
  YMODEM reception, W25Q64 ciphertext storage, decryption/installation, and
  APP handover. Unchanged in this stage.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS APP with a business
  task, OTA state machine, and dynamically created worker. Dual buffering,
  queue/semaphore handshakes, completion replies, EEPROM write-ready polling
  and readback, the update command, and key-triggered reset have been validated.

## Build and Run

**Standalone testing flashes a plaintext APP at `0x08000000`. After startup,
it waits for a download command before starting YMODEM. Combined Bootloader
operation requires restoring `0x08008000` and integrating the installation flow.**

1. Open `03_Firmware/02_APP/MDK-ARM/STM32F411CEU6.uvprojx` in Keil, check
   IROM1 starts at `0x08000000`, build, and flash through J-Link/SWD.
2. Connect adapter TX/RX to PA10/PA9 with common ground. Set the serial assistant
   to 115200, 8N1, no flow control, send `11 22 33` in HEX mode, and check for `C`.
3. Close the assistant's port, connect SecureCRT, and use **Transfer → Send Ymodem**
   to send a BIN. Check completion and the `44 55 66` (ASCII `DUf`) reply.
4. In RTT Viewer Terminal 0, check the received size and EEPROM readback:
   `8C 58 00 00` for a 22,668-byte file and state `11`. W25Q uses PA5/PA6/PA7
   and PA4 chip select; EEPROM uses PB8/PB9.
5. Disconnect SecureCRT, reopen the assistant, send `77 88 99` in HEX mode,
   and press PA0 within about 20 seconds. Startup logs reappearing confirm reset.

On 2026-10-04, a 22,668-byte BIN transfer reached 100% with 0 Errors; the completion
reply, EEPROM state/size readback, and key-triggered reset succeeded. Regression
passed after formatting, comments, diagnostic-log cleanup, and version updates.
This validates the in-APP workflow, without validating new firmware installation
or a complete external flash image.

See the [Bootloader documentation](03_Firmware/01_Bootloader/README.md) for its
encrypted package, link-address, and operating requirements. Its existing flow
is not yet connected to this stage's APP metadata.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/       SPL Bootloader starting at 0x08000000
│   └── BSP/W25QXX/      External flash driver and buffered access
└── 02_APP/              HAL + FreeRTOS APP; standalone start is 0x08000000
    ├── BSP/            UART_Handler, W25Qxx Handler, AT24Cxx
    ├── SysSupport/     OS_init queue/task creation, Debug logging setup
    ├── Tasks/          APP business task, SWC_OTA state machine and worker
    └── Middlewares_User/ Ymodem, EasyLogger, SEGGER RTT
```

## Current Implementation Notes

- Receive-notification and processing-completion waits are both 2000 ms.
  Reception is prepared before replies; timeouts/session exits stop reception
  and clear notifications. Receive and worker task stacks are each 512 words.
- The 200 ms delay before the completion reply lets the host exit YMODEM
  processing. Key waiting uses 400 scans, a 50 ms interval, and 10 ms debounce;
  timeout returns to download-command waiting.
- An EEPROM write API returning does not finish the internal write cycle.
  The success branch polls ACK before further access, with a 10 ms window.
  Startup clears the state; metadata is not yet a reset-persistent installation protocol.
- Each APP IDLE event is treated as a complete packet, without fragment
  accumulation. Neither receiver checks packet CRC16; a full external flash
  image readback comparison is also not implemented.
- `ee_test()` and W25Q ID business tests remain commented out. Receive results,
  EEPROM readback, and error/warning logs remain. Related handwritten files are
  version `1.1` (2026-10-04); CubeMX-generated files are excluded from comment/format cleanup.
- Bootloader external flash indices remain in RAM; its existing reception and
  installation must complete in one powered session. See the
  [APP documentation](03_Firmware/02_APP/README.md) for initialization order and other details.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
