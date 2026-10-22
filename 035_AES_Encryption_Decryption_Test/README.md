**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Software AES Encryption/Decryption Experiment

## Project Description

A software AES encryption experiment based on the STM32F411CEU6. The core consists of two files: `aes.c` / `aes.h` under `Middlewares_User/Algorithms/AES`.

##### Introduction

This project implements the AES algorithm purely in software on the STM32F411CEU6, and verifies correctness by printing the ciphertext, decrypted plaintext, and round-trip check result over the UART.

##### Main Features

- AES encryption and decryption with selectable 128 / 192 / 256-bit key length (`AES_KEY_LENGTH`); AES-128 by default
- Selectable ECB / CBC operating modes (`AES_MODE`); CBC by default, with in-place encryption/decryption support
- Built-in demo `test()`: automatically performs encrypt → decrypt → round-trip comparison after power-up and prints the result
- Includes runtime S-box generation functions (`generate_aes_sbox`, etc.) that can be used to verify the static S-box tables

##### Repository Structure

```
├── Core/                  # HAL initialization and application entry (main.c calls test())
├── Drivers/               # CMSIS and STM32F4 HAL library
├── Middlewares_User/
│   ├── Algorithms/AES/    # AES core: aes.c / aes.h
│   ├── EasyLogger/        # Logging component
│   └── SEGGER_RTT/        # RTT debug output
├── Tasks/Debug/           # Debug module initialization
├── MDK-ARM/               # Keil µVision project
└── STM32F411CEU6.ioc      # CubeMX configuration file
```

## Usage Guide

##### Environment and Dependencies

| Item | Description |
| --- | --- |
| Hardware | STM32F411CEU6 development board (required) |
| Debugger | J-Link (required for flashing) |
| Toolchain | Keil MDK-ARM (µVision) |
| Firmware package | STM32Cube FW_F4 V1.28.3 |
| Serial tool | USB-to-TTL adapter + any serial terminal (required for observing results) |

##### Hardware Connection and Configuration

USART1 prints the results at 115200 8N1:

| Board Pin | Connects To | Description |
| --- | --- | --- |
| PA9 (USART1_TX) | USB-to-TTL RXD | UART transmit |
| PA10 (USART1_RX) | USB-to-TTL TXD | UART receive |
| GND | USB-to-TTL GND | Common ground required |

##### Build, Run, and Observe

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil µVision and build the project.
2. Flash the program to the board via J-Link and reset.
3. Open a serial terminal (115200 8N1). You should see the ciphertext in hex, the decrypted plaintext `JesMicro`, and the round-trip check `AES roundtrip = PASS` printed by `test()`.

##### Porting Notes

- `aes.c` / `aes.h` are hardware-independent. When porting, only handle `#include "main.h"` (provide types such as `uint8_t`) and the EasyLogger reference in `test_sbox()`.
- C51 compatibility macros such as `code` / `xdata` are kept in the source, so the files can be used directly on 8051 platforms.
- The data length must be a multiple of 16 bytes. No padding is implemented; the caller must pad the data.

## Author's Notes

##### Project-Related Reflections

None.

##### Other Reflections

None.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
