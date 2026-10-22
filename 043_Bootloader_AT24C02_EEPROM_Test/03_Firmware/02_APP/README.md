**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Application — In-APP OTA Reception and Soft Reset

## Overview

This STM32 HAL + CMSIS-RTOS2 + FreeRTOS APP receives YMODEM through USART1 DMA,
connecting W25Q64 processing, AT24C02 metadata access, and a key-triggered reset
to exercise the in-APP background update workflow while the business task runs.

**Standalone linking/flashing starts at `0x08000000`, with 96 KB configured.
This stage runs without the Bootloader or internal firmware installation;
reset starts the original APP again.**

## Features

- Initializes HAL, the 100 MHz clock, GPIO, DMA, USART1, SPI1, and FreeRTOS.
  `os_init()` creates the receive-length queue, `app_task`, and `swc_ota_task`.
- The business task toggles PC13 every 500 ticks (about 500 ms).
  `ee_test()` and W25Q ID tests are commented out.
- USART1 uses PA9/PA10, 115200, 8N1, no flow control, DMA2 Stream2 Channel4
  in normal mode, IDLE events, and `g_YmodemReclength_queue` notifications.
- YMODEM handles 128/1024-byte packets with two alternating 1030-byte SRAM
  buffers. `app_data_position_queue` carries size/payload pointers; the worker
  calls `W25Q64_WriteData()` and signals buffer switching and replies.
- After the session, waits 200 ms before sending `44 55 66` (ASCII `DUf`),
  allowing the host to exit YMODEM processing. Then calls
  `W25Q64_WriteData_End()` and releases download resources.
- After `77 88 99`, waits for active-low PA0: 10-tick debounce, 400 scans at
  50-tick intervals (about 20 seconds). A press calls `NVIC_SystemReset()`;
  timeout returns to download waiting. Each state-machine cycle delays 10 ms.

## EEPROM Metadata

AT24C02 uses software I2C on PB8/PB9:

| Data | Start address | Length | Current behavior |
|---|---:|---:|---|
| OTA state | `0x00` | 1 byte | `0x00` at startup, `0x11` during download |
| New APP file size | `0x01` | 4 bytes | Declared size after reception, little-endian |
| Current APP size | `0x05` | 4 bytes | Reserved; not updated |

The success branch checks writes and polls ACK every 1 ms within a 10 ms window
before further access and EasyLogger/RTT readback. A 22,668-byte file is
`0x0000588C`, read as `8C 58 00 00` in increasing address order.
State `0x22` is not written yet; APP startup clears the state.

## Flash Layout

| Mode | Link and flash address | Configured size |
|---|---:|---:|
| Standalone test (current) | `0x08000000` | `0x18000` (96 KB) |
| APP with Bootloader | `0x08008000` | `0x18000` (96 KB) |

Standalone testing occupies the Bootloader region. W25Q64 uses 4 KB sector
buffering; this stage validates the flow, without verifying a complete external
flash image. Combined operation requires restoring the APP address and integrating installation.

## Build and Test

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil, check IROM1 starts at
   `0x08000000`, build, and flash through J-Link/SWD.
2. Connect adapter TX/RX to PA10/PA9 with common ground. Use 115200, 8N1,
   no flow control; send `11 22 33` in HEX mode to start periodic `C` replies.
3. Close the assistant's port, connect SecureCRT with the same settings, and
   select **Transfer → Send Ymodem**. Send a BIN and observe `DUf` afterward.
4. In RTT Viewer Terminal 0, check the received size and EEPROM readback:
   state `11`, and `8C 58 00 00` for a 22,668-byte file. W25Q uses PA5/PA6/PA7
   and PA4 chip select (SPI mode 0, prescaler 256); EEPROM uses PB8/PB9.
5. Disconnect SecureCRT, reopen the assistant, send `77 88 99` in HEX mode,
   and press PA0 within about 20 seconds. Startup logs reappearing confirm reset.

On 2026-10-04, a 22,668-byte transfer reached 100% with 0 Errors; the completion
reply, EEPROM readback, and key reset passed, including regression after cleanup
and version updates. Temporary flow diagnostics were removed; receive results,
EEPROM readback, and error/warning logs remain.

Each IDLE event is one packet, without fragment accumulation or CRC16 checks.
`W25Q64_Init()` still precedes GPIO/SPI initialization; the order is unchanged.

## Project Layout

```text
Core/                    CubeMX peripherals, FreeRTOS, and entry point
BSP/UART_Handler/        UART receive-event callback
BSP/W25Qxx/              SPI flash driver and sector-buffered access
BSP/AT24Cxx/             Software I2C, EEPROM driver, and standalone test API
SysSupport/              OS_init queue/tasks, Debug logging setup
Tasks/                   APP business task, SWC_OTA state machine and worker
Middlewares_User/        Ymodem, EasyLogger, and SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil project files
```

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
