**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# Peripheral Function Test Demo

## Project Description

##### Introduction

This bare-metal project tests peripheral communication, authentication, and data processing, with results displayed through RTT.

##### Main Features

- Software I2C communication and peripheral initialization.
- Authentication, encryption/decryption roundtrips, and write/read or read-only user storage comparisons.
- Transport error tracking with passed, failed, skipped, unverified, and SDK-limited results.

##### Repository Structure

```text
Core/               Initialization and application entry
Drivers/            Platform driver dependencies
Bsp/                Peripheral communication adapters
Tasks/              Test and logging modules
Middlewares_User/   RTT and logging components
MDK-ARM/            Keil project
```

## Usage Guide

##### Environment and Dependencies

The matching development board and peripheral, project drivers and static library, Keil MDK 5 (ARM Compiler 5), J-Link, and RTT Viewer.

##### Hardware Connection and Configuration

Connect the debugger through SWD. Wire the peripheral according to the project's GPIO configuration, share ground, and provide I2C pull-ups. Logs use RTT channel 0.

##### Build, Run, and Observe

1. Include `keros_demo.h`, initialize, and run `keros_demo_test(&report)`. It writes unique `pattern=v2` data to all requested pages before reading them back.
2. Save the complete write/read log, switch to `keros_demo_test_read_only(&report)`, rebuild, and flash.
3. Fully remove power after flashing the read-only version, then power up and save its log without rewriting.
4. Compare the serial numbers, modes, page numbers, and complete `READ` data before checking the summary.

Both entries require `keros_demo_init()` first and use `keros_demo_report_t`. Save complete logs from RTT Viewer Terminal 0. Missing lines or another write between runs can invalidate a full comparison or retention conclusion.

Each page holds 64 bytes. Requests cover pages 0-35: 36 pages and 2304 bytes. The current SDK permits actual access to pages 0-29: 30 user pages and 1920 bytes. It rejects pages 30-35 internally, reporting `SDK_LIMIT` with zero I2C calls and no valid readback. The 36 requests are not the chip's total page count. Although 2 KB equals 2048 bytes, that calculation does not establish 32 accessible pages.

In the latest complete write/read and read-only logs, all 1920 bytes from pages 0-29 matched both the expected patterns and each other. Both runs reported:

```text
EEPROM pages: requested=36 match=30 fail=0 sdk_limit=6
```

`MATCH_UNVERIFIED` means full readback data matched, `SDK_LIMIT` means the library rejected a request, and `FAIL` means an access or data check failed. Software cannot confirm the external power-off step. Matched bytes do not establish physical capacity, an exact model, or whether its marking is correct.

Write/read tests overwrite every writable user page, including page 0 used for project 01's key data. The read-only entry makes no EEPROM writes; initialization, authentication, and reads still send I2C control commands. See the [test module guide](Tasks/Keros_Test/README.md) for integration and report details.

##### Porting Notes

Check BSP pins, timing, and interface adapters when porting. Update header search paths and retain the matching static library in the linker configuration.

## Author's Notes

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
