**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# project 047 APP1

The initial demonstration firmware for [project 047](../../README.md), based on an STM32CubeMX-generated HAL and FreeRTOS project. It receives encrypted firmware while running and requests installation by Bootloader.

## Differences Between APP1 and APP2

The functional differences are confined to the `app_task` thread in the lowercase file `Tasks/APP/app.c`: the status log and PC13 LED toggle interval differ. All other application logic remains the same.

| Project | Status Log | PC13 LED Toggle Interval |
|---|---|---|
| APP1 | `APP1 running...` | 500 ticks, approximately 500 ms |
| APP2 | `APP2 running...` | 200 ticks, approximately 200 ms |

Both use a 1 kHz FreeRTOS tick, link to `0x08008000`, and share the same OTA, CRC, and first-start confirmation flow.

## Startup and Update

On the first trial startup, the APP reads EEPROM state `0x44` after peripheral initialization, writes `0x00`, and waits for a watchdog reset. Subsequent startups with state `0x00` launch FreeRTOS normally.

1. Build `MDK-ARM/STM32F411CEU6.uvprojx`, package the latest BIN as `APP1_EncryptionBIN.mxxx`, and install it through [Bootloader](../01_Bootloader/README.md).
2. Build [APP2](../03_APP2/README.md) at the same address and encrypt it as `APP2_EncryptionBIN.mxxx`.
3. In APP1, send HEX `11 22 33`; after receiving `C`, send the APP2 encrypted package through YMODEM.
4. Wait for HEX `44 55 66` and the `OTA ready` log confirming EEPROM state `0x22`.
5. Send HEX `77 88 99` and press the key within about 20 seconds. APP1 requests a software reset, and Bootloader performs installation and first-start confirmation.
6. The normal flow includes 3 resets, ending with `APP2 running...` and an approximately 200 ms LED toggle interval.

Use UART at 115200 baud, 8N1, with no flow control. Close the previous serial connection before switching tools. Firmware files are in `../04_BIN_EncryptionBIN/`; see the [main project README](../../README.md) for wiring.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
