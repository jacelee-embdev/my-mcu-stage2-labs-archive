**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader and Application Demo

## Overview

This project contains an STM32F411CEU6 bootloader and a standalone application.
The SPL bootloader provides application handover, GPIO, USART, Flash, delay, and
logging support. The application uses STM32 HAL, CMSIS-RTOS2, and FreeRTOS.

```text
Power-on/reset
  → Bootloader (0x08000000)
  → Validate the application and switch VTOR and the main stack
  → Application Reset_Handler (0x08008000)
  → Start FreeRTOS
```

## Flash Layout

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| Application | 2–7 | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): SPL bootloader,
  peripheral verification, and application handover.
- [02_APP](03_Firmware/02_APP/README.md): HAL + FreeRTOS application with
  button-controlled LED operation and logging.

## Build and Flash

1. Open and build the required subproject in Keil.
2. Select **Erase Sectors** in Flash Download settings.
3. Download through J-Link/SWD and view logs with SEGGER RTT Viewer.

> The bootloader is currently in peripheral-verification mode. Remove the
> sector 3 erase/write test before enabling `bootloader_jump_to_app()`.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL bootloader at 0x08000000
└── 02_APP/            HAL + FreeRTOS application at 0x08008000
```

## Verification

The bootloader button, LED, USART1, SEGGER RTT, Flash, and delay functions have
been verified on hardware. Application handover and FreeRTOS operation have also
been validated.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
