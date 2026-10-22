**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# project 047 APP2

The target firmware used to verify the APP1 update in [project 047](../../README.md). It is based on an STM32CubeMX-generated HAL and FreeRTOS project and retains the same application OTA functionality as APP1.

## Differences Between APP1 and APP2

The functional differences are confined to the `app_task` thread in the lowercase file `Tasks/APP/app.c`: the status log and PC13 LED toggle interval differ. All other application logic remains the same.

| Project | Status Log | PC13 LED Toggle Interval |
|---|---|---|
| APP1 | `APP1 running...` | 500 ticks, approximately 500 ms |
| APP2 | `APP2 running...` | 200 ticks, approximately 200 ms |

Both use a 1 kHz FreeRTOS tick, link to `0x08008000`, and share the same OTA, CRC, and first-start confirmation flow.

## Build and Verify

1. Build `MDK-ARM/STM32F411CEU6.uvprojx` and package the latest BIN as `APP2_EncryptionBIN.mxxx`.
2. In the running [APP1](../02_APP1/README.md), send HEX `11 22 33`; after receiving `C`, send this encrypted package through YMODEM.
3. After receiving HEX `44 55 66`, send HEX `77 88 99` and press the key within about 20 seconds. Bootloader installs APP2.
4. On its first trial startup, APP2 reads `0x44`, writes `0x00`, and waits for a watchdog reset. FreeRTOS starts normally on the next boot.
5. Confirm `APP2 running...` and an approximately 200 ms PC13 LED toggle interval to verify the switch to APP2.

The normal APP1 → APP2 flow includes 3 resets, excluding additional manual resets. APP2 can receive another firmware version using the same commands. See the [main project README](../../README.md) for the full procedure and wiring.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
