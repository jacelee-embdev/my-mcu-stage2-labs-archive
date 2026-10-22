**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 APP1 OTA Update Experiment

## Project Description

##### Introduction

APP1 is the initial application in project 044, running on the STM32F411CEU6 with HAL and FreeRTOS. It receives encrypted APP2 firmware and requests an update while the application remains operational. The APP downloads the package and commits update metadata; the Bootloader handles decryption, backup, installation, and launch. See the [project 044 overview](../../README.md) for the complete experiment.

##### Main Features

- Run the application test task and OTA task under FreeRTOS, printing status logs and toggling the PC13 LED.
- Receive encrypted packages through USART1 DMA double buffering and YMODEM, coordinating writes to external Flash region A through a queue and semaphore.
- Save EEPROM state `0x11` before downloading. After verifying writes and length, save the package length and state `0x22`, send the completion response, then request a system reset to the Bootloader after the update command and key confirmation.

**APP1 and APP2 share the same hardware configuration, protocol, and OTA flow. Only the log message and delay in the `Tasks/APP/app.c` test task differ:**

| Project | Log message | Loop delay / PC13 toggle interval |
| --- | --- | --- |
| APP1 | `APP1 running...` | 500 ms |
| APP2 | `APP2 running...` | 200 ms |

The FreeRTOS tick rate is 1000 Hz. PC13 toggles once per loop; the table shows the interval between level changes.

##### Repository Structure

```text
02_APP1/
├── Core/              # HAL initialization, DMA, and interrupts
├── BSP/               # UART reception, W25Q Flash, AT24C02 EEPROM
├── Tasks/APP/         # Log and LED test task
├── Tasks/SWC_OTA/     # OTA state handling and Flash write task
├── SysSupport/        # Logging and RTOS resource initialization
├── Middlewares/       # FreeRTOS
├── Middlewares_User/  # YMODEM, EasyLogger, SEGGER RTT
└── MDK-ARM/           # Keil project
```

## Usage Guide

##### Environment and Dependencies

- Hardware: STM32F411CEU6, W25Q series SPI Flash, AT24C02 EEPROM, a key, a PC13 indicator LED, a USB-to-serial adapter, and J-Link.
- Software: Keil MDK (ARM Compiler 5.06 update 7), HAL, FreeRTOS V10.3.1, SecureCRT, and a serial utility that sends HEX bytes. J-Link RTT Viewer V8.12g was used for log observation. Libraries and middleware are included.

##### Hardware Connection and Configuration

| Interface | Pins and settings |
| --- | --- |
| USART1 | PA9/TX → adapter RX, PA10/RX ← adapter TX; 115200, 8N1, no flow control |
| W25Q Flash | PA4/CS, PA5/SCK, PA6/MISO, PA7/MOSI; SPI mode 0 |
| AT24C02 | PB8/SCL, PB9/SDA; software I²C |
| Key / LED | PA0 with pull-up, grounded when pressed; PC13 indicator LED |
| Debug and power | J-Link over SWD; common ground and compatible 3.3 V logic levels |

The APP runs at `0x08008000` and is launched by the companion Bootloader. External A is `0x000000–0x00FFFF` and B is `0x010000–0x01FFFF`, each 64 KiB. The APP downloads into A; the Bootloader performs decryption and installation.

##### Build, Run, and Observe

1. Open and build `MDK-ARM/STM32F411CEU6.uvprojx`, keeping the APP link address at `0x08008000`. Both `APP1.bin` and `APP1_EncryptionBIN.mxxx` are in the [firmware directory](../04_BIN_EncryptionBIN/). New packages must match the Bootloader's AES-256 CBC header format, key, and IV, be no larger than 64 KiB, and be aligned to 16 bytes.
2. For the initial test, follow the [Bootloader guide](../01_Bootloader/README.md) to program the Bootloader, receive the encrypted APP1 package, and launch this application.
3. Send HEX `11 22 33` to the running APP. After receiving `C`, switch to SecureCRT and send an encrypted package with YMODEM. This test sent `APP2_EncryptionBIN.mxxx` from APP1. Close the previous tool's serial connection before switching tools.
4. Wait for HEX `44 55 66` (displayed as `DUf` in ASCII) and `OTA ready: size=39440, EEPROM state=0x22`. The package and metadata are confirmed; the current APP continues running, and the new APP is not installed yet.
5. Send HEX `77 88 99` and press the key within about 20 seconds. After the APP requests a system reset, the Bootloader decrypts A into B, backs up a valid old APP into A, then installs and launches the new APP. A key timeout returns to waiting for a download.
6. Check RTT logs and the PC13 LED. If logs stop after an APP launch, reopen RTT Viewer to search for the control block.

**Board regression results on 2026-10-06, after code formatting and comment cleanup:**

| Observation | Result |
| --- | --- |
| APP1 running | `APP1 running...` appeared about every 500 ms and the LED blinked |
| APP1 downloads APP2 | Transfer showed 38 KB, about 11 seconds, and 0 Errors. Received `44 55 66`; the log confirmed 39,440 package bytes and EEPROM state `0x22` |
| Update to APP2 | After the update command and key confirmation, `APP2 running...` appeared about every 200 ms |

The normal APP1 → APP2 update passed regression. APP2 uses the same flow as APP1 to receive subsequent firmware; a reverse update was not separately tested in this run.

##### Porting Notes

Update HAL peripherals and DMA configuration, RTOS interrupt priorities, the APP link address and vector table, and the Flash regions, EEPROM metadata, and command bytes shared with the Bootloader. The packaging tool and Bootloader must use the same package format, key, and IV. The tested platform is STM32F411CEU6.

## Author's Notes

##### Project-Related Reflections

None.

##### Other Reflections

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
