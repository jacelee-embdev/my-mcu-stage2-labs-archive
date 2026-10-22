**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 OTA and Watchdog First-Start Confirmation

This project adds first-start confirmation for a new APP to an encrypted OTA workflow, using **IWDG (independent watchdog)**. Bootloader installs APP1 first; APP1 then receives an encrypted APP2 package and requests an update.

## Projects

| Project | Purpose | Link Address |
|---|---|---|
| [Bootloader](03_Firmware/01_Bootloader/README.md) | Download, decrypt, back up, install, and confirm startup | `0x08000000` |
| [APP1](03_Firmware/02_APP1/README.md) | Initial demonstration firmware with application OTA | `0x08008000` |
| [APP2](03_Firmware/03_APP2/README.md) | Verify the update while retaining the same OTA functionality | `0x08008000` |

The [firmware folder](03_Firmware/04_BIN_EncryptionBIN/) contains BIN files and encrypted `.mxxx` packages. Bootloader reserves 32 KiB, and the APP run region is 96 KiB. External flash regions A/B are each 64 KiB at `0x000000–0x00FFFF` / `0x010000–0x01FFFF`.

## Environment and Wiring

STM32F411CEU6, W25Q SPI flash, AT24C02 EEPROM, USB-to-UART, and J-Link; use Keil MDK, a compatible AES packaging tool, a YMODEM sender, and RTT Viewer.

| Interface | Configuration |
|---|---|
| USART1 | PA9 TX / PA10 RX; 115200 baud, 8N1, no flow control |
| SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| EEPROM software I2C | PB8 SCL / PB9 SDA |
| LED / key | PC13 / PA0; active-low key |

Use 3.3 V logic and a common ground, cross-connect UART TX/RX, and retain I2C pull-ups.

## Build and Update

1. Build both APPs at `0x08008000`, then package their latest BINs as `APP1_EncryptionBIN.mxxx` and `APP2_EncryptionBIN.mxxx`. Match Bootloader's AES-256 CBC format, key, and IV; encrypted packages must fit in 64 KiB.
2. For a fresh installation, clear the state at EEPROM address `0x00` to `0x00`, erase MCU internal flash, and flash Bootloader. Press the key to enter download mode and send the encrypted APP1 package through YMODEM after receiving `C`. MCU chip erase does not clear EEPROM or external flash.
3. In APP1, send HEX `11 22 33`; after receiving `C`, send the encrypted APP2 package through YMODEM. Wait for HEX `44 55 66` and the `OTA ready` log.
4. Send HEX `77 88 99` and press the key within about 20 seconds. Bootloader installs the firmware and performs first-start confirmation. Close the previous serial connection before switching tools.

YMODEM retains per-frame CRC-16/XMODEM checks. The RTT control block may change when switching between Bootloader and an APP, requiring reconnection.

## States and Resets

EEPROM address `0x00` stores the update state:

| State | Meaning |
|---|---|
| `0x00` | Normal startup |
| `0x11` | Download in progress or incomplete |
| `0x22` | Encrypted package downloaded; installation pending |
| `0x33` | New APP installed; first-start confirmation pending |
| `0x44` | First-start confirmation in progress |

After installation, write `0x33` and request a software reset. Bootloader writes `0x44`, enables IWDG, and jumps to the APP. After peripheral initialization, the APP writes `0x00` and waits for a watchdog reset; normal startup follows.

A successful fresh installation includes **2 automatic resets**, and an APP-initiated update includes **3 resets**, excluding power-on and additional manual resets. A remaining `0x44` currently causes only a state clear and software reset; automatic restoration of the old APP across resets is not implemented.

## Differences Between APP1 and APP2

The functional differences are confined to the `app_task` thread in the lowercase file `Tasks/APP/app.c`. All other application logic remains the same.

| Project | Status Log | PC13 LED Toggle Interval |
|---|---|---|
| APP1 | `APP1 running...` | 500 ticks, approximately 500 ms |
| APP2 | `APP2 running...` | 200 ticks, approximately 200 ms |

Both use HAL, FreeRTOS, and a 1 kHz tick, sharing the same OTA, CRC, and first-start confirmation flow.

Hardware logs from 2026-10-07 confirmed normal operation after the initial APP1 installation and the APP1 → APP2 update, with the expected startup counts. The first trial's `0x44` log and reset causes have not yet been fully captured.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
