**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Bootloader Startup Optimization

Building on encrypted OTA and IWDG first-start confirmation, this project uses a **retained RAM jump request and a software reset** to jump to the APP early when Bootloader enters `main()` again. Only Bootloader is modified in this task; the APP projects remain unchanged. Hardware validation uses the existing APP1.

## Projects

| Project | Purpose | Link Address |
|---|---|---|
| [Bootloader](03_Firmware/01_Bootloader/README.md) | OTA management, startup confirmation, and an early jump after reset | `0x08000000` |
| [APP1](03_Firmware/02_APP1/README.md) | Existing application used for this validation, with application OTA | `0x08008000` |
| [APP2](03_Firmware/03_APP2/README.md) | Existing OTA demonstration firmware | `0x08008000` |

Bootloader reserves 32 KiB, and the APP run region is 96 KiB. The [firmware folder](03_Firmware/04_BIN_EncryptionBIN/) contains BIN files and encrypted `.mxxx` packages. See the individual project READMEs for the existing OTA procedure.

## Startup Flow and Changes

1. **Initial power-on:** Without a valid jump request, `Core/Src/main.c` initializes peripherals, prints logs, waits about 5 seconds, and calls `OTA_state_mangner()`.
2. **Write the request and reset:** In normal startup state `0x00`, `Tasks/Bootloader/bootloader.c` checks that the download key is not pressed and the APP entry is valid, writes `g_app_jump_request = 0x55AA55AAU`, and calls `System_SoftwareReset()`.
3. **Jump early after reset:** On re-entering `main()`, Bootloader reads the request, clears it to `0`, and calls `bootloader_jump_to_app()` to enter the APP startup code.

`MDK-ARM/bootloader_noinit.sct` reserves the last 16 bytes of SRAM (`0x2001FFF0–0x2001FFFF`) as `UNINIT` for the variable's `NO_INIT` section, preserving the request across a software reset. In Keil Linker settings, uncheck **Use Memory Layout from Target Dialog** and select `./bootloader_noinit.sct`.

The early jump occurs before peripheral initialization in `main()`; `SystemInit()` and C runtime initialization still execute. The initial 5-second wait is retained, while the reset pass skips repeated peripheral initialization and waiting.

## Environment and Wiring

STM32F411CEU6, W25Q SPI flash, AT24C02 EEPROM, USB-to-UART, and J-Link; use Keil MDK (ARM Compiler 5) and RTT Viewer.

| Interface | Configuration |
|---|---|
| USART1 | PA9 TX / PA10 RX; 115200 baud, 8N1, no flow control |
| SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| EEPROM software I2C | PB8 SCL / PB9 SDA |
| LED / key | PC13 / PA0; active-low key |

Use 3.3 V logic and a common ground, cross-connect UART TX/RX, and retain I2C pull-ups.

## Build and Validation

1. Build Bootloader. In Keil download settings, select **Erase Sectors, Program, and Verify** to erase and program only the regions used by Bootloader, preserving the APP at `0x08008000`. Do not select **Erase Full Chip**; no encrypted firmware transfer is needed for this test.
2. Confirm the update state at EEPROM address `0x00` is `0x00`, and leave the download key unpressed. Exit debugging and power-cycle the board; observe startup logs and APP1's blinking PC13 LED.
3. Set a breakpoint at the request check in `main()`. After the first check, press **F5 (Run)**; the program writes the request and performs a software reset. The next breakpoint hit should show `0x55AA55AA`. Continue, then halt execution and confirm the request is `0` and PC is in the APP region, `0x08008000–0x0801FFFF`.

**Validated on 2026-10-08:** Bootloader builds with 0 errors and 0 warnings. Hardware debugging showed the request changing `0 → 0x55AA55AA → 0`, with PC at `0x0800ED12` after halting and APP1's LED blinking normally. Retained RAM and the early jump after reset passed validation.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
