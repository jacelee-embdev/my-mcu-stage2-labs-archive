**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AHT21 Connectivity Test and BSP Interfaces

The firmware uses GPIO-based IIC to check whether the AHT21 acknowledges its address. DWT provides delays, and SEGGER RTT and EasyLogger report the result. The imported notes report verified hardware connectivity. This archive pass did not rebuild, flash, or repeat hardware tests.

## Learning notes

This project preserves the interface design stage of the AHT21 driver. Structures and function pointers describe the bus, clock, and optional RTOS delay dependencies. Measurement declarations are not implementations: the source skeleton does not yet provide the complete protocol. The application retains the bare-metal address acknowledgment example to separate connectivity checks from driver development. Measurement waiting, frame parsing, and validation belong to later implementation stages.

## Configuration

- The HSI and PLL provide a 100 MHz core clock. HAL uses SysTick, and blocking delays use DWT.
- SDA uses PB13 and SCL uses PB14. The AHT21 uses 7-bit address `0x38` and write-address byte `0x70`.
- USART1 uses PA9 for TX and PA10 for RX at 115200 baud, 8N1. `printf` and EasyLogger share RTT channel 0.

## Quick start

Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK 5 with ARM Compiler 5, then build and flash through SWD. Connect a J-Link and view channel 0 in RTT Viewer.

At startup, the firmware waits 100 ms before sending the AHT21 write address. It logs `AHT21 ACK received.` on acknowledgment or `AHT21 did not acknowledge.` on failure. The main loop emits a sample log about every two seconds.

## Source modules

- `Core/Src/main.c` runs the address acknowledgment test using `Z02_BSP/IIC` and `Z01_SYS/delay.c`.
- In `Z02_BSP/AHT21/sw_bsp_aht21_driver`, `sw_bsp_aht21_driver.h` declares IIC, millisecond time-base, and optional RTOS delay callbacks, along with interfaces for temperature, humidity, and combined readings.
- `sw_bsp_aht21_register.h` defines the address bytes, measurement command, and command arguments. `sw_bsp_aht21_driver.c` provides the source skeleton.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
