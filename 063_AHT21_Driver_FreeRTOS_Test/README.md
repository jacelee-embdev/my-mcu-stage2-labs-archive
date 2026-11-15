**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AHT21 Temperature and Humidity Driver

This project implements an AHT21 driver and a FreeRTOS sampling task on the STM32F411CEU6. The driver triggers a measurement over software I²C, waits for the Busy bit to clear, reads seven bytes, and checks the CRC. It then converts the two 20-bit raw values from the same frame to temperature and relative humidity.

## Learning notes

This learning stage implements the sensor protocol behind the public interface. The application injects GPIO software IIC, a millisecond clock, and RTOS delay callbacks. The Driver handles initialization, measurement waiting, communication checks, CRC-8 validation, and conversion. Temperature and humidity come from the same frame. Successful reads are logged together; failures report a status code. Each read is followed by a two-second delay, so the full period also includes measurement time. This stage calls the synchronous Driver directly and does not use Handler queues, caching, or asynchronous callbacks. Check power, voltage compatibility, and common ground before connecting.

## Wiring and setup

- Connect SDA to PB13 and SCL to PB14, with a common ground between the sensor and board. The AHT21 uses the 7-bit address `0x38`.
- Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK 5 with ARM Compiler 5, then build and flash through SWD.
- Connect a J-Link and view EasyLogger output on RTT Viewer channel 0. The task waits two seconds after each read. Continuous output was verified on the board.

## Source modules

- `os_init.c` registers `aht21_task`. The task binds the software I²C bus and FreeRTOS callbacks, initializes the sensor, and logs readings.
- `sw_bsp_aht21_driver.c/.h` provide driver construction, status reads, and combined temperature and humidity reads. The driver header enables `OS_SUPPORTING`, and `sw_bsp_aht21_register.h` defines the addresses and measurement command.
- `Z02_BSP/IIC` provides GPIO-based software I²C. The driver's deinitialization, sleep, and wakeup methods are placeholders.

## Archive validation

This archive preserves the original learning stage and run records. This pass checks source, project references, documentation links, and retained inputs only. It does not rebuild, flash, or test hardware; hardware results described here belong to the imported project history.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
