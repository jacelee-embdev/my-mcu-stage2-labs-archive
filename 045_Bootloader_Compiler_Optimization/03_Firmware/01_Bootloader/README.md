**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Encrypted Firmware OTA Bootloader Experiment

## Project Description

##### Introduction

This is the Bootloader subproject of project 044, implementing serial firmware updates on the STM32F411CEU6 with the Standard Peripheral Library. It receives or reads encrypted packages, decrypts them, backs up the old APP, installs the new APP, and launches the application from internal Flash. It works with the APP1 and APP2 projects for the update experiment.

##### Main Features

- Receive encrypted firmware through USART1 and YMODEM into external Flash region A.
- Decrypt firmware from A into B using AES-256 CBC, checking its length and APP startup vectors.
- Back up a valid old APP into A, then install the new APP from B into internal Flash. Initial installation is allowed without an old APP; a valid backup made during the current update enables recovery after installation failure.
- Select the startup path from EEPROM state: `0x00` launches the APP or downloads on a key press; `0x11` first tries the old APP, then downloads again if launch fails; `0x22` installs the package already downloaded.

##### Repository Structure

```text
01_Bootloader/
├── Core/              # Startup, peripheral initialization, internal Flash
├── BSP/               # W25Q external Flash and AT24C02 EEPROM drivers
├── Tasks/Bootloader/  # State handling, decryption, backup, installation, launch
├── Middlewares_User/  # YMODEM, AES, EasyLogger, SEGGER RTT
├── SYSTEM/            # Delay support
├── Drivers/           # CMSIS and STM32F4 Standard Peripheral Library
└── MDK-ARM/           # Keil project
```

## Usage Guide

##### Environment and Dependencies

- Hardware: STM32F411CEU6, W25Q series SPI Flash, AT24C02 EEPROM, a key, a PC13 indicator LED, a USB-to-serial adapter, and J-Link.
- Software: Keil MDK (the project selects ARM Compiler 5.06 update 7), SecureCRT with YMODEM support, and a serial utility that sends HEX bytes. J-Link RTT Viewer V8.12g was used to observe logs. Libraries and middleware are included in the project.

##### Hardware Connection and Configuration

| Interface | Pins and settings |
| --- | --- |
| USART1 | PA9/TX → adapter RX, PA10/RX ← adapter TX; 115200, 8N1, no flow control |
| W25Q Flash | PA4/CS, PA5/SCK, PA6/MISO, PA7/MOSI; SPI mode 0 |
| AT24C02 | PB8/SCL, PB9/SDA; software I²C, device address byte `0xA0` |
| Key / LED | PA0 with pull-up, grounded when pressed; PC13 indicator LED |
| Debug and power | J-Link over SWD; common ground for the board, adapter, and peripherals, with compatible 3.3 V logic levels |

| Storage region | Address range | Purpose |
| --- | --- | --- |
| Internal Bootloader | `0x08000000–0x08007FFF` (32 KiB) | Bootloader code |
| Internal APP | `0x08008000–0x0801FFFF` (96 KiB) | Application execution region |
| External A | `0x000000–0x00FFFF` (64 KiB) | New encrypted package, then optionally overwritten with the old APP backup after decryption |
| External B | `0x010000–0x01FFFF` (64 KiB) | New APP plaintext |

Encrypted packages must be no larger than 64 KiB and aligned to 16 bytes. Their header format, key, and IV must match this project.

##### Build, Run, and Observe

1. Open and build `MDK-ARM/STM32F411CEU6_SPL.uvprojx`. The companion APP is linked at `0x08008000`. Test packages are in [`../04_BIN_EncryptionBIN/`](../04_BIN_EncryptionBIN/).
2. For the initial installation test, separately clear the update flag at EEPROM address `0x00` to `0x00`, then select **Erase Chip** to erase internal Flash and program the Bootloader. Erase Chip does not clear external Flash or EEPROM.
3. After reset, allow about 13 seconds for startup preparation, then press the key to enter download mode. Once repeated `C` characters appear, send the encrypted APP1 package using SecureCRT's YMODEM transfer. The Bootloader automatically decrypts, installs, and launches APP1. With no old APP present, backup is skipped.
4. In APP1, send HEX `11 22 33`. After receiving `C`, switch to SecureCRT and send the encrypted APP2 package. HEX `44 55 66` (displayed as `DUf` in ASCII) confirms the package has been received and is ready; APP2 is not installed yet. Close the previous tool's serial connection before another tool uses the same port.
5. Send HEX `77 88 99` and press the key within about 20 seconds to confirm. After the APP requests a system reset, the Bootloader follows state `0x22`: decrypt A into B, back up internal APP1 into A, then install and launch APP2.
6. Observe RTT logs and the PC13 LED. If logs stop after an APP launch, reopen RTT Viewer to search for the control block again. APP1 logs became visible after reopening it during this test.

**Board regression results on 2026-10-06, after code formatting and comment cleanup:**

| Stage | Observed result |
| --- | --- |
| Bootloader → APP1 | Transfer showed 38 KB, about 11 seconds, and 0 Errors. The decryption log reported 39,420 plaintext bytes and no valid old APP. Finally, `APP1 running...` appeared about every 500 ms and the LED blinked |
| APP1 receives APP2 | Transfer showed 38 KB, about 11 seconds, and 0 Errors. Received `44 55 66`; the log read `OTA ready: size=39440, EEPROM state=0x22` |
| Bootloader → APP2 | The update completed after the command and key confirmation; `APP2 running...` appeared about every 200 ms |

Initial installation and the normal `0x22` update passed regression. State `0x11` interruption recovery and rollback after installation failure were not part of this board test.

##### Porting Notes

When porting, update peripheral pins, clocks, internal Flash sectors and capacity, APP link address and vector table, external A/B regions, and EEPROM metadata layout together. The packaging tool and Bootloader must use the same package format, key, and IV. The tested platform is STM32F411CEU6.

## Author's Notes

##### Project-Related Reflections

None.

##### Other Reflections

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
