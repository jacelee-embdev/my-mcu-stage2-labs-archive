**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader-to-Application Jump Demo

## Overview

This project demonstrates a complete bootloader-to-application handover on the STM32F411CEU6. The bootloader uses the STM32F4 SPL, while the application uses STM32 HAL, CMSIS-RTOS2, and FreeRTOS. Both projects output logs through EasyLogger and SEGGER RTT.

Boot flow:

```text
Power-on/reset
  → Bootloader (0x08000000)
  → Validate the application stack pointer
  → Disable interrupts and switch VTOR
  → Load the application stack
  → Application Reset_Handler (0x08008000)
  → Re-enable interrupts and start FreeRTOS
```

## Flash Layout

The STM32F411CEU6 provides 512 KB of internal Flash:

| Region | Sectors | Start | Size | Address range |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | `0x8000` (32 KB) | `0x08000000 ~ 0x08007FFF` |
| Application | 2–7 | `0x08008000` | `0x78000` (480 KB) | `0x08008000 ~ 0x0807FFFF` |

## Firmware Projects

- [01_Bootloader](03_Firmware/01_Bootloader/README.md): validates the application, switches the vector table and stack, and transfers control.
- [02_APP](03_Firmware/02_APP/README.md): runs FreeRTOS, prints periodic logs, and controls the PC13 LED with the PA0 button.

## Build and Flash

1. Open and build both subprojects in Keil.
2. Select **Erase Sectors** in Flash Download settings.
3. Flash the bootloader, followed by the application.
4. Reset the board and view logs in SEGGER RTT Viewer Terminal 0.
5. Confirm that the application prints `elog TEST!` periodically and that pressing PA0 makes the PC13 LED blink.

> Do not use a full-chip erase when flashing the two images separately, because the second download would erase the first partition.

## Project Layout

```text
03_Firmware/
├── 01_Bootloader/     SPL bootloader at 0x08000000
└── 02_APP/            HAL + FreeRTOS application at 0x08008000
```

The remaining top-level directories contain project-management, reference, hardware, software, mechanical, factory, and tooling materials.

## Verification

Verified on hardware: the bootloader enters the application successfully, and the application's FreeRTOS task, RTT logging, PA0 button, and PC13 LED all operate correctly.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
