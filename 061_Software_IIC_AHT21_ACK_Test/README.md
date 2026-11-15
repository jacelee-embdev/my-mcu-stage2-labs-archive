**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bare-Metal IIC Driver and AHT21 ACK Test

A GPIO-based IIC driver built on STM32 HAL, with DWT cycle-counter delays and AHT21 address acknowledgment testing. Results are logged through SEGGER RTT and EasyLogger. The imported notes report verified hardware connectivity. This archive pass did not rebuild, flash, or repeat hardware tests.

## Learning notes

This learning stage builds IIC primitives from GPIO operations. A bus object stores the SDA and SCL ports and pins. The driver provides start, stop, byte transfer, ACK/NACK, and register access interfaces. The application only probes the address once; it does not trigger measurements or check measurement CRC. Logs use RTT despite the UART configuration. Check module power and voltage compatibility before connecting. An address acknowledgment does not establish measurement validity or sensor accuracy.

## Configuration

- HSI through the PLL provides a 100 MHz core clock. HAL uses SysTick; blocking delays use DWT.
- SDA: PB13; SCL: PB14. The AHT21 7-bit address is `0x38`; its write-address byte is `0x70`.
- USART1 uses PA9 for TX and PA10 for RX, configured for 115200 baud, 8 data bits, no parity, and one stop bit.
- Standard `printf` and EasyLogger share RTT channel 0. Logging is synchronous and supports levels, colors, timestamps, and module tags.

## Quick start

Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK 5 with ARM Compiler 5, build, and flash through SWD. Connect a J-Link and view RTT channel 0.

At startup, the firmware initializes DWT and GPIO, waits 100 ms, then sends the device write address once. It logs `AHT21 ACK received.` on acknowledgment or `AHT21 did not acknowledge.` on failure. The main loop emits sample logs approximately every two seconds. This example tests address acknowledgment.

## Source modules

- `Z01_SYS/delay.c` and `delay.h`: `Delay_Init`, `Delay_Us`, and `Delay_Ms`. Initialize DWT before using the delay functions.
- `Z02_BSP/IIC/iic.c` and `iic.h`: bus configuration, GPIO-based IIC signaling, and register access.
- `Core/Src/main.c`: bus pin assignment and the AHT21 connectivity test.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
