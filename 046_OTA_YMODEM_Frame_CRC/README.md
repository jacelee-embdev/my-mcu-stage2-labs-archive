**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 YMODEM CRC and Encrypted Firmware Update Experiment

## Project Description

##### Introduction

This project adds per-frame CRC-16/XMODEM validation to an encrypted firmware update workflow. Bootloader installs APP1, then APP1 receives an APP2 package and requests installation by Bootloader.

##### Main Features

- Validate the full 128/1024-byte YMODEM data area, including padding, with CRC-16/XMODEM (polynomial `0x1021`, initial value `0x0000`); reject mismatches before writing.
- Receive encrypted packages into external region A; Bootloader decrypts into B, backs up a valid old APP and installs the new APP.
- Use EEPROM states `0x00` / `0x11` / `0x22` for normal startup, an unfinished/failed download and a completed package.
- Identify APP1 and APP2 through their logs and PC13 LED timing. File-length checks are retained; a whole-file CRC is not implemented.

##### Repository Structure

```text
00_ProjectManagement/, 00_Reference/   Planning and references
01_FunctionMap/, 02_Hardware/         Function and hardware materials
03_Firmware/
├── 01_Bootloader/                    Download, decrypt, install and launch
├── 02_APP1/                          First application and OTA receiver
├── 03_APP2/                          Update target and observable marker
└── 04_BIN_EncryptionBIN/             Plain BINs and encrypted .mxxx packages
04_Software/, 05_Mechanical/          Supporting project directories
06_Factory/, 07_Tools/                Production and tool directories
```

Details: [Bootloader](03_Firmware/01_Bootloader/README.md), [APP1](03_Firmware/02_APP1/README.md), [APP2](03_Firmware/03_APP2/README.md).

## Usage Guide

##### Environment and Dependencies

STM32F411CEU6 board, W25Q SPI flash, AT24C02 EEPROM, USB-to-UART adapter and J-Link are required. Use Keil MDK 5.38 / ARMCC 5.06 update 7, a YMODEM sender such as SecureCRT, and an AES packaging tool compatible with the Bootloader. The repository includes SPL for Bootloader, HAL/FreeRTOS for APPs, libcrc, EasyLogger and SEGGER RTT; RTT Viewer is used for logs.

##### Hardware Connection and Configuration

Use 3.3 V logic and a common ground; cross-connect UART TX/RX.

| Interface | Configuration |
|---|---|
| USART1 | PA9 TX / PA10 RX; 115200 baud, 8N1, no flow control |
| W25Q SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| AT24C02 software I2C | PB8 SCL / PB9 SDA; retain bus pull-ups |
| Status and input | PC13 LED; PA0 active-low key |

Bootloader starts at `0x08000000` (32 KiB); both APPs use `0x08008000` (96 KiB). External A/B regions are `0x000000–0x00FFFF` and `0x010000–0x01FFFF`, each 64 KiB.

##### Build, Run, and Observe

1. Build both APPs at `0x08008000`, then package their latest BINs into `APP1_EncryptionBIN.mxxx` and `APP2_EncryptionBIN.mxxx` in [the firmware folder](03_Firmware/04_BIN_EncryptionBIN/). Match Bootloader's AES-256 CBC header/key/IV; each package must exceed 16 bytes, be 16-byte aligned and fit in 64 KiB.
2. For a fresh test, clear EEPROM OTA state at `0x00` to `0x00`, erase the MCU's entire internal flash and flash Bootloader. Chip erase does not clear external flash or EEPROM.
3. Wait about 13 seconds for startup preparation, press the key to enter download mode, and send the APP1 encrypted package by YMODEM after receiving `C`. Bootloader decrypts, installs and launches APP1.
4. In APP1, send HEX `11 22 33`; after `C`, send the APP2 encrypted package by YMODEM. Wait for HEX `44 55 66` and `OTA ready … EEPROM state=0x22`.
5. Send HEX `77 88 99` and press the key within about 20 seconds. APP1 resets; Bootloader installs and launches APP2. Close the previous serial connection before switching sender tools.

| Application | Status log | Delay after each LED toggle / log |
|---|---|---|
| APP1 | `App1 running...` | 500 ticks, approximately 500 ms |
| APP2 | `App2 running...` | 200 ticks, approximately 200 ms |

Both use a 1 kHz FreeRTOS tick. Their C/H files differ only in `Tasks/APP/app.c`; the OTA and CRC implementations are shared.

Hardware testing on 2026-10-07 confirmed both Bootloader → APP1 and APP1 → Bootloader → APP2. CRC-mismatch injection and failure rollback were not part of this run.

##### Porting Notes

When porting, update GPIO/clock settings, flash sectors and limits, DMA/RTOS configuration, EEPROM layout and AES package parameters together. Keep APP link and vector addresses aligned with Bootloader; other platforms have not been verified.

## Author's Notes

##### Project-Related Reflections

None

##### Other Reflections

None

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
