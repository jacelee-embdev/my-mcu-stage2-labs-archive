**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AT24C02 Integration

## Overview

This project integrates an AT24C02 EEPROM into an STM32F411CEU6 HAL + FreeRTOS
APP, using software I2C for device ACK detection and read/write testing.
The companion Bootloader uses the STM32F4 Standard Peripheral Library (SPL).
It receives encrypted firmware through YMODEM, stores it in external W25Q64,
then decrypts it into the MCU's internal APP run region and starts the APP.

```text
Standalone test: reset → FreeRTOS APP (0x08000000)
                → ee_test(): detect ACK, test the last page → RTT results
Encrypted update: Bootloader (0x08000000) → PA0 requests YMODEM reception into W25Q64
                  → Decrypt into APP (0x08008000) → Set VTOR/MSP and start APP
```

## Storage Layout

Internal MCU flash layout for the combined Bootloader/APP setup:

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
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS APP with AT24C02;
  the default task calls `ee_test()` about every two seconds.

## Build and Run

**When used with the Bootloader, the APP is linked and flashed at `0x08008000`.
For this standalone test, its start address was temporarily changed to
`0x08000000`, and plaintext firmware was flashed directly to test AT24C02,
avoiding encrypted package creation and transfer through a host tool.**

1. Open `03_Firmware/02_APP/MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
2. Check that IROM1 starts at `0x08000000`, rebuild, and flash through J-Link/SWD.
3. After reset, connect RTT Viewer Terminal 0 to view the EEPROM test results.

Hardware testing confirmed ACK at 7-bit address `0x50` (write address byte `0xA0`).
Only the last page, `0xF8–0xFF`, is tested. Data increments across rounds; in the
captured round, all eight bytes written as `0x18–0x1F` matched on readback,
followed by `EEPROM read/write test successful.`

Standalone testing occupies the original Bootloader region. To restore combined
updates, change the APP address back to `0x08008000`, rebuild a plaintext BIN of
at most 96 KB, and create the matching encrypted package. Reflash the Bootloader,
hold PA0 at reset, and send the package through SecureCRT's
**Transfer → Send Ymodem**. Without an update request, or after installation
failure, the Bootloader waits for another request.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader starting at 0x08000000
│   └── BSP/W25QXX/   External W25Q driver and buffered access interface
└── 02_APP/            HAL + FreeRTOS APP; current standalone start is 0x08000000
    └── BSP/AT24Cxx/  Software I2C, EEPROM driver, and last-page test
```

## Current Implementation Notes

- AT24C02 holds 256 bytes in 8-byte pages. `ee_test()` only overwrites the last
  page, with a 10 ms ACK polling timeout for write completion. Chip ID reads
  are not supported by this device.
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
