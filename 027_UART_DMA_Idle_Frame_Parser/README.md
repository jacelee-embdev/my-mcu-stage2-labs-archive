**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 UART DMA Receive-to-Idle and Frame Parsing

## Project Overview

This STM32F411CEU6 and FreeRTOS project implements USART1 circular-DMA
reception, idle-event handling, and data-frame parsing.

Features:

- Starts variable-length reception with `HAL_UARTEx_ReceiveToIdle_DMA()`.
- Stores DMA data in the application-owned global buffer
  `g_uart_rx_buffer`.
- Handles DMA half-transfer, transfer-complete, and UART idle events.
- Uses cumulative `head_total` and `tail_total` counters with modulo indexing.
- Verifies at compile time that the DMA buffer size is a non-zero power of two.
- Updates the head in interrupt context and wakes the parser with a task
  notification.
- Validates the header, length, payload, checksum, and tail with a state
  machine.
- Delivers valid packets to the application task through a queue.

Data flow:

```text
USART1 + Circular DMA
        ↓
HT / TC / IDLE callbacks update head_total
        ↓
A task notification wakes packet_parser_task
        ↓
The parser updates tail_total and parses frames
        ↓
Valid packets are queued to app_task
```

## Repository Layout

```text
App/                                  Frame parser and application task
BSP/                                  USART1 DMA receive driver
Middlewares_User/circular_buffer/     Circular-buffer middleware
Core/                                 STM32 initialization and interrupts
MDK-ARM/STM32F411CEU6.uvprojx         Keil project
STM32F411CEU6.ioc                     STM32CubeMX project
```

## Usage

### Development Environment

- STM32F411CEU6 at 100 MHz
- Keil MDK 5 with ARM Compiler 5.06
- STM32CubeMX
- FreeRTOS
- USB-to-UART adapter, J-Link, and RTT Viewer

### UART Configuration

| USB-to-UART | STM32F411 |
|---|---|
| TX | PA10 / USART1_RX |
| GND | GND |

- Baud rate: 115200
- Data format: 8-N-1
- Hardware flow control: disabled
- USART1 RX DMA: Circular mode
- DMA buffer size: 128 bytes
- DMA buffer requirement: non-zero and a power of two

### Frame Format

```text
FF FE | LEN | DATA[LEN] | CHECK | AA
```

- `LEN`: one-byte payload length with an effective range of 1 to 255 bytes.
- `CHECK`: low 8 bits of the accumulated `LEN + DATA` sum.
- Complete frame size: `LEN + 5` bytes.
- `MAX_PAYLOAD_LEN` is currently 256 to reserve payload storage.

Test frame:

```text
FF FE 04 01 02 03 04 0E AA
```

### Build and Run

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
2. Build the project and flash the target.
3. Send the test frame in HEX mode with a serial terminal.
4. View the parsed payload in RTT Viewer.

### Configuration and Porting

- UART, pin, and DMA settings are in `STM32F411CEU6.ioc`.
- Set the buffer size with `BUFFER_SIZE` in `circular_buffer.h`. A zero value
  or a value that is not a power of two triggers a compilation error.
- Set payload storage with `MAX_PAYLOAD_LEN` in `app.h`. Because the protocol
  uses a `uint8_t` `LEN` field, one frame can represent at most 255 payload
  bytes.
- Add the global `g_uart_rx_buffer` directly to the Keil Watch window to
  inspect its contents.
- Frame fields and parser states are implemented in `app.c`.
- UART DMA reception and event callbacks are implemented in
  `bsp_uart_driver.c`.

> Buffer-overflow protection is not implemented. During continuous high-speed
> reception, the parser must consume data before DMA overwrites unread bytes.

## Author's Notes

### Project Notes

- Requirements Analysis and Business Flow

![Requirements Analysis and Business Flow](Docs/Images/01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向.png)

Source file: [01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向.drawio](Docs/01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向.drawio)

- Software Resource Architecture

![Software Resource Architecture](Docs/Images/02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构.png)

Source file: [02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构.drawio](Docs/02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构.drawio)

- Program Analysis

![Program Analysis](Docs/Images/03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析.png)

Source file: [03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析.drawio](Docs/03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析.drawio)

### Other Notes

None.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
