**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Application — YMODEM Reception

## Overview

This STM32 HAL + CMSIS-RTOS2 + FreeRTOS APP receives YMODEM packets into
SRAM using USART1 DMA, IDLE events, and a queue alongside a demonstration task.

**Standalone flashing starts at `0x08000000`; restore `0x08008000` with the
Bootloader. BIN transfers only test communication, without flash writes,
firmware decryption/installation, or full-file retention.**

## Features

- Initializes HAL, the 100 MHz clock, GPIO, DMA, USART1, SPI1, and FreeRTOS.
- `os_init()` creates the receive-length queue, `app_task`, and `swc_ota_task`.
- The demonstration task toggles PC13, runs `ee_test()`, and logs the W25Q ID
  through EasyLogger/RTT, then delays 500 ticks (currently about 500 ms).
- USART1 uses PA9/PA10, 115200, 8N1, and no flow control. Reception uses
  DMA2 Stream2 Channel4 in normal mode and `HAL_UARTEx_ReceiveToIdle_DMA()`.
- The callback queues lengths through `g_YmodemReclength`; data stays in a
  1030-byte global buffer. Handles metadata, 128/1024-byte packets, and completion
  replies, with a 2000 ms receive-notification timeout.
- Prepares reception before replies, stops RX and clears notifications on timeout
  or session exit, then waits 10 ms before the next session.

SecureCRT transferred a 22,668-byte BIN at 100% with 0 Errors; regression after
cleanup passed. Each IDLE event counts as one packet, without fragment
accumulation or CRC16 verification. The tests confirm the receive flow.

## Flash Layout

| Mode | Link and flash address | Configured size |
|---|---:|---:|
| APP with Bootloader | `0x08008000` | `0x18000` (96 KB) |
| Standalone test (current) | `0x08000000` | `0x18000` (96 KB) |

Standalone testing occupies the Bootloader region. In combined mode, the
Bootloader installs the APP and sets VTOR/MSP before handover; the APP
re-enables interrupts at startup.

## Build and Test

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil, check IROM1 starts at
   `0x08000000`, build, and flash through J-Link/SWD.
2. Connect adapter TX/RX to PA10/PA9 with common ground. Set SecureCRT to
   115200, 8N1, no flow control; periodic `C` characters appear after reset.
3. Use **Transfer → Send Ymodem** to send a BIN. After completion,
   further `C` characters indicate that another receive session is waiting.
4. View business logs in RTT Viewer Terminal 0. W25Q uses PA5/PA6/PA7 and
   PA4 chip select (SPI mode 0, prescaler 256); W25Q64 ID is `EF16`.
   EEPROM uses PB8/PB9 and tests its last page, `0xF8–0xFF`.
5. For combined updates, restore `0x08008000`, rebuild an APP of at most 96 KB,
   flash the Bootloader, and send a matching encrypted package.

W25Q read/write/erase APIs remain outside the APP receive flow.
`W25Qx_Init()` still precedes GPIO/SPI initialization; ID reads do not verify
that initialization succeeded.

## Project Layout

```text
Core/                    HAL peripherals, FreeRTOS, and entry point
BSP/UART_Handler/        UART receive-event callback
BSP/W25Qxx/              SPI flash driver
BSP/AT24Cxx/             Software I2C, EEPROM driver, and last-page test
SysSupport/              OS_init queue/tasks, Debug logging setup
Tasks/                   APP demonstration, SWC_OTA reception and SRAM buffer
Middlewares_User/        Ymodem, EasyLogger, and SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
