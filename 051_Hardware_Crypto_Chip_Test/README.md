**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# Peripheral Data Programming and Function Tests

## Project Description

##### Introduction

This repository contains two standalone bare-metal projects. Project 01 writes an application AES key and initialization vector (IV) to user EEPROM. Project 02 tests peripheral functions and storage pages. Both report results through RTT.

##### Main Features

- Application AES-256 key and IV programming, with CRC and full-page readback checks.
- Communication, authentication, encryption/decryption roundtrips, and separate write/read and read-only tests.
- Transport diagnostics distinguishing matching data, failures, unverified results, and SDK range limits.

##### Repository Structure

```text
01_Key_data_burning/                   Application key and IV programming
02_Chip_function_test_demonstration/   Peripheral and storage range tests
LICENSE                               Repository license
```

Details: [Project 01](01_Key_data_burning/README.md) · [Project 02](02_Chip_function_test_demonstration/README.md).

## Usage Guide

##### Environment and Dependencies

The matching boards, peripherals, drivers, and static libraries; Keil MDK 5 (ARM Compiler 5), J-Link, and RTT Viewer.

##### Hardware Connection and Configuration

Connect the debugger through SWD. Follow the selected project's wiring configuration, share ground, and provide I2C pull-ups. Check the test mode and write parameters before running.

##### Build, Run, and Observe

1. Choose project 01 or 02 and open its Keil project in `MDK-ARM/`.
2. Build, flash, and reset. Build and use each project separately.
3. Save complete logs in RTT Viewer Terminal 0 and interpret them using the selected project's instructions.

Project 01 writes page 0 by default, using one 64-byte page, and retries programming on each startup. `SUCCESS` means the current write and readback checks passed.

Project 02 requests pages 0-35, totaling 2304 bytes. The current SDK permits pages 0-29, totaling 1920 bytes. `SDK_LIMIT` for the other six pages means the SDK rejected the request without accessing the chip; it does not establish a hardware capacity boundary. The latest complete write/read and read-only logs matched across all 30 user pages.

Writes overwrite existing data; project 02 includes page 0 used by project 01. To check retention, flash the read-only version, fully remove power, then power up and compare without rewriting. Matching data alone does not identify physical capacity, the exact model, or whether its marking is correct.

##### Porting Notes

Check BSP pins, timing, interfaces, and static library compatibility. Update header search paths and linker settings, and confirm that the peripheral supports the storage interface being used.

## Author's Notes

None.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
