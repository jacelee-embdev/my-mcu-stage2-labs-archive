**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader AT24C02 Integration

## Overview

This stage ports the AT24C02 driver from HAL to the STM32F4 Standard Peripheral
Library (SPL) Bootloader, adding PB8/PB9 software I²C, device ACK detection,
and a last-page read/write test. Hardware tests passed on 2026-10-04, followed
by formatting and comment updates in the driver and entry-point files.

The existing HAL + FreeRTOS APP retains command-triggered YMODEM, W25Q64
processing, EEPROM metadata, and key-triggered reset. The Bootloader retains
encrypted reception, decryption/installation, and handover. **EEPROM driver
integration is complete; APP metadata is not yet connected to Bootloader
installation. Automatic installation across reset remains to be implemented.**

```text
This stage: Bootloader (0x08000000) → SPL software I²C / EEPROM ACK
            → Write last page 0xF8–0xFF → Poll write completion → Compare readback
Existing encrypted update: PA0 → YMODEM ciphertext to W25Q64
                          → Decrypt/install APP (0x08008000) → Set VTOR/MSP and jump
Existing standalone APP: 11 22 33 → C / YMODEM → W25Q64 / EEPROM metadata
                        → 44 55 66 (DUf) → 77 88 99 → PA0 reset → original APP
```

## Storage Layout

Internal MCU flash for combined Bootloader/APP operation:

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP run region | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| Receive-stage erase region | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

The Bootloader stores ciphertext from W25Q64 offset `0x000000` and still
erases internal Sector 5 during reception. The standalone APP currently starts
at `0x08000000` with 96 KB configured, occupying the Bootloader region;
combined operation requires `0x08008000`. The APP uses two 1030-byte SRAM
packet buffers and does not install the current download into internal flash.

AT24C02 has 256 bytes in 8-byte pages. The APP stores OTA state at `0x00`,
the four-byte little-endian new APP size at `0x01–0x04`, and reserves
`0x05–0x08` for the current APP size. Startup writes `0x00`, download writes
`0x11`, and update-request state `0x22` is not yet written. The Bootloader
test overwrites only `0xF8–0xFF`, without accessing these metadata addresses.

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL Bootloader,
  YMODEM, W25Q64 ciphertext storage, decryption/installation, and handover.
  This stage adds AT24C02: `at24cxx.c/.h` provide device operations and
  `iic.c/.h` provide software I²C. `main.c` detects the device at startup
  and tests the last page if execution reaches the point before the waiting loop.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS APP with concurrent
  business and OTA tasks. Dual buffering, queue/semaphore handshakes, completion
  replies, EEPROM write-ready polling/readback, the update command, and
  key-triggered reset were validated in the previous stage.

## Build and Run

1. Open `03_Firmware/01_Bootloader/MDK-ARM/STM32F411CEU6_SPL.uvprojx` in
   Keil, build, and flash through J-Link/SWD. Its start address is `0x08000000`.
2. EEPROM uses PB8 (SCL)/PB9 (SDA), open-drain outputs, and external pull-ups.
   W25Q64 uses PA5/PA6/PA7 and PA4 chip select; PA9/PA10 UART uses 115200, 8N1.
3. In RTT Viewer Terminal 0, check the ACK at seven-bit address `0x50`,
   each write/read value at `0xF8–0xFF`, and
   `EEPROM read/write test successful.`. `ee_test()` runs one round and
   overwrites the last page.
4. The existing encrypted update is requested by holding PA0 during reset
   and choosing SecureCRT **Send Ymodem**. APP link address, key/IV, and package
   format must match; see the
   [Bootloader documentation](03_Firmware/01_Bootloader/README.md).
5. To repeat the existing standalone APP test, use
   `02_APP/MDK-ARM/STM32F411CEU6.uvprojx` and flash the plaintext APP directly.
   Send HEX `11 22 33`, a YMODEM BIN, then HEX `77 88 99` and press PA0 to
   reset; see the [APP documentation](03_Firmware/02_APP/README.md).

This stage's device ACK, write-completion polling, and last-page read/write
tests passed. Previously, a 22,668-byte BIN reached 100% with 0 Errors;
completion replies, EEPROM state/size readback, and key-triggered reset passed.
The size bytes are `8C 58 00 00`. Neither result establishes a complete
APP/Bootloader installation protocol across reset or full external image verification.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/       SPL Bootloader starting at 0x08000000
│   ├── BSP/AT24Cxx/     SPL software I²C, AT24C02 driver and tests
│   └── BSP/W25QXX/      External flash driver and buffered access
└── 02_APP/              HAL + FreeRTOS APP; standalone start is 0x08000000
    ├── BSP/            UART_Handler, W25Qxx Handler, AT24Cxx
    ├── SysSupport/     OS_init queue/task creation and Debug logging
    ├── Tasks/          APP business task, SWC_OTA state machine and worker
    └── Middlewares_User/ Ymodem, EasyLogger, SEGGER RTT
```

## Current Implementation Notes

- Bootloader GPIO uses SPL open-drain access; writing high releases the bus.
  Write-completion polling uses `SysTickUpdateTime`, incremented every
  millisecond, with a 10 ms window and requires initialized, running SysTick.
  AT24C02 has no readable chip ID; the corresponding message is not a fault.
- This cleanup covers the four driver files and `main.c`: English file headers
  and section separators, Chinese descriptive comments, and formatting.
  Identifiers, macro contents, function logic, and log strings are preserved.
  Versions increase by 0.1: the four driver files are `V1.1` and `main.c` is
  `v1.7`, dated 2026-10-04.
  Existing APP business tests remain commented out; Bootloader `ee_test()` is enabled.
- A successful EEPROM write return means transmission has finished; subsequent
  access must wait for internal write completion. APP startup clears OTA state.
  The Bootloader does not yet consume state/size metadata across reset. External
  flash indices remain in RAM, so existing reception and installation must
  finish within one powered session.
- APP receive and completion waits are 2000 ms with 512-word task stacks.
  The completion reply has a 200 ms delay; the update-command key window is
  about 20 seconds. Each IDLE event is treated as a complete packet without
  fragment accumulation. Neither receiver checks packet CRC16 or verifies a
  full external image. This stage does not change the APP flow.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
