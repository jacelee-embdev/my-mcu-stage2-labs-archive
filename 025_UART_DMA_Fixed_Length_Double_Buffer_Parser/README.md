**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 UART Double-Buffer Frame Parsing Demo

## Project

##### Overview

This STM32F411CEU6 and FreeRTOS project implements fixed-size UART DMA reception, two alternating circular buffers, and frame parsing.

##### Features

- Alternating DMA receive buffers with queue notifications.
- State-machine validation of headers, length, payload, checksum, and tail.
- Queue delivery of valid payloads to the application task.

##### Layout

```text
App/                                  Frame parser and application task
BSP/                                  UART double-buffer reception
Middlewares_User/circular_buffer/     Circular buffer
Core/                                 STM32 initialization
MDK-ARM/STM32F411CEU6.uvprojx         Keil project
```

## Usage

##### Requirements

- STM32F411CEU6 at 100 MHz
- Keil MDK 5 with ARM Compiler 5.06
- FreeRTOS / CMSIS-RTOS2
- USB-to-UART adapter, J-Link, and RTT Viewer

##### Wiring and Configuration

| USB-to-UART | STM32F411 |
|---|---|
| TX | PA10 / USART1_RX |
| GND | GND |

UART: `115200-8-N-1`, no hardware flow control.

USART1 RX uses DMA normal mode. Each completed transfer is restarted in the alternate receive buffer.

Frame format:

```text
FF FE | LEN | DATA[LEN] | CHECK | AA
```

`LEN` is 1–100. `CHECK` is the low byte of `LEN + DATA`.

Each receive operation reads 9 bytes; each circular buffer holds 108 bytes.

The current DMA receive path advances the write index in fixed 9-byte steps. It does not stop DMA before the write index overtakes the read index, so the consumer must keep up to prevent unread data from being overwritten.

##### Build, Run, and Observe

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil, build, and flash.
2. Send `FF FE 04 01 02 03 04 0E AA` over UART.
3. View parsed payloads in RTT Viewer.

##### Porting

- Open `STM32F411CEU6.ioc` in STM32CubeMX to change the UART, pins, and RX DMA settings, then regenerate the code.
- Change the receive size in `BSP/bsp_uart_driver.c`.
- Keep the buffer size divisible by the receive size.
- Change the frame format and checksum in `App/app.c`.

## Author's Notes

##### Project Notes

None.

##### Other Notes

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
