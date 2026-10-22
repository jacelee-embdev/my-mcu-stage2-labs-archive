**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# Application AES Key and IV Programmer

## Project Description

##### Introduction

This project writes one application AES-256 key and initialization vector (IV) to an external device's user EEPROM over I2C, then verifies the readback. These application data are intended for later AES decryption and are separate from the device authentication key.

##### Main Features

- Initialize the chip, read its serial number, and verify authentication and response data.
- Write a 64-byte page containing a 16-byte IV, a 32-byte key, CRC8, its inverse, and 14 reserved bytes; page 0 is the default.
- Read back the page, check its CRC, compare all bytes, and report results through RTT.

##### Repository Structure

```text
Bsp/                    Software I2C and platform adapters
Tasks/Key_Programmer/   Provisioning flow and key configuration
Core/                   Initialization and application entry
MDK-ARM/               Keil project
```

## Usage Guide

##### Environment and Dependencies

The matching board and peripheral, project drivers and static library, Keil MDK 5 (ARM Compiler 5), J-Link, and RTT Viewer.

##### Hardware Connection and Configuration

Connect the debugger through SWD. Wire I2C according to the BSP configuration, share ground, and provide pull-ups. Logs use RTT channel 0.

Configure the key, IV, page, and password in [key_programmer_config.h](Tasks/Key_Programmer/key_programmer_config.h). `KEY_PROGRAMMER_IV_BYTES` contains the 16-byte IV; `KEY_PROGRAMMER_AES_KEY_BYTES` contains the 32-byte key. The default `KEY_PROGRAMMER_DIAGNOSTIC_MODE=0` enables programming. Setting it to 1 runs function diagnostics without EEPROM writes; it does not provide a read-only key retention check.

##### Build, Run, and Observe

1. Open the Keil project in `MDK-ARM/`, build, flash, and run.
2. Startup calls `KeyProgrammer_Init()` and `KeyProgrammer_Run()`. After initialization and authentication pass, it writes page 0 and reads it back.
3. `Readback CRC and 64-byte comparison PASSED` followed by `SUCCESS` confirms the current CRC, inverse, and full-page checks. The latest board log passed these checks.

Page offsets 0-15 hold the IV, 16-47 the key, 48 the CRC8, 49 its inverse, and 50-63 zeros. `Write PASSED` covers the write stage; the final readback check completes the current programming verification.

Every startup retries programming and overwrites page 0. Another `SUCCESS` after reset does not prove retention. This project has no separate read-only key retention entry point yet. Project 02's range write test also overwrites this page.

##### Porting Notes

Adapt GPIO, timing, and interfaces when porting. Check header search paths and static library linking, and ensure the peripheral supports the user EEPROM interface being used.

## Author's Notes

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
