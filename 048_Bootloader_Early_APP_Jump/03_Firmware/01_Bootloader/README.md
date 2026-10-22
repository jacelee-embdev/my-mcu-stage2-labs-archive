**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# project 047 Bootloader

The startup and firmware installation project for [project 047](../../README.md). It uses the STM32F4 Standard Peripheral Library and IWDG (independent watchdog) to confirm the first startup of a new APP.

## Main Features

- Receive encrypted firmware through YMODEM with per-frame CRC-16/XMODEM checks.
- Decrypt the AES-256 CBC package from external flash region A into B, back up a valid old APP, and install the new APP.
- Select normal startup, update, or first-start confirmation from the EEPROM state.

## First-Start Confirmation

1. After installation, write `0x33` and request a software reset.
2. Bootloader reads `0x33`, writes `0x44`, enables IWDG, and jumps to the new APP.
3. After peripheral initialization, the APP reads `0x44`, writes `0x00`, and stops execution while waiting for a watchdog reset.
4. Bootloader reads `0x00` and launches the APP normally without enabling IWDG again.

A remaining `0x44` currently causes only a state clear and software reset. Automatic restoration of the old APP across resets is not implemented.

## Build and Use

- Keil project: `MDK-ARM/STM32F411CEU6_SPL.uvprojx`.
- Bootloader address: `0x08000000`, with 32 KiB reserved; APP address: `0x08008000`.
- Fresh installation: clear the state at EEPROM address `0x00` to `0x00`, erase MCU internal flash, and flash Bootloader. MCU chip erase does not clear EEPROM or external flash.
- Press the key to enter download mode, then send the latest encrypted APP package through YMODEM after receiving `C`.
- For an APP-initiated update, Bootloader uses the package already downloaded by the APP and committed with state `0x22`; no second transfer is needed.

The successful normal flow includes **2 automatic resets** for a fresh installation and **3 resets** for an update initiated from a running APP. Power-on and additional manual resets are excluded.

See the [main project README](../../README.md) for UART settings, wiring, and the full procedure. The RTT control block may change when switching between Bootloader and an APP, requiring reconnection.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
