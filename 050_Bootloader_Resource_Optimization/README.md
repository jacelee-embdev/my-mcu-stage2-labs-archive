**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 OTA: Bootloader Resource Optimization

This stage reduces Bootloader resource usage by trimming EasyLogger and SFUD features to leave room for future functionality. Full builds show Flash usage decreasing from **23.40 KB to 19.98 KB**, and static RAM usage from **22.71 KB to 21.89 KB**, saving **3.42 KB** and **0.82 KB**, respectively. Hardware validation after optimization found no functional issues.

## Firmware Projects

| Project | Implementation and role | Instructions |
| --- | --- | --- |
| 01_Bootloader | SPL bare-metal firmware for reception, decryption, installation, and boot-state management; the target of this optimization | [Bootloader README](03_Firmware/01_Bootloader/README.md) |
| 02_APP1 | HAL / FreeRTOS application for update reception; APP1 logs and a 500 ms application-task delay | [APP1 README](03_Firmware/02_APP1/README.md) |
| 03_APP2 | Update verification application copied from APP1; APP2 logs and a 200 ms application-task delay | [APP2 README](03_Firmware/03_APP2/README.md) |

```text
03_Firmware/
├── 01_Bootloader/          Optimized Bootloader
├── 02_APP1/                Initial application and update reception
├── 03_APP2/                Update target for verification
└── 04_BIN_EncryptionBIN/   Encrypted APP1 / APP2 packages
08_docs/
└── image/                 Resource statistics before and after optimization
```

## Optimization Changes

The build-generated `.map` file and keil-build-viewer statistics were used to identify resource usage by module, then remove features unnecessary for fixed hardware and basic logging. Changes are concentrated in `03_Firmware/01_Bootloader/`; APP1 and APP2 retain their existing middleware configurations.

### EasyLogger

| Setting | Before | After |
| --- | --- | --- |
| Per-line buffer, `ELOG_LINE_BUF_SIZE` | 1024 B | 256 B |
| Color output, `ELOG_COLOR_ENABLE` | Enabled | Disabled |
| Function name, file path, and line number | `ELOG_FMT_USING_FUNC`, `ELOG_FMT_USING_DIR`, and `ELOG_FMT_USING_LINE` enabled | All three macros disabled |
| Format at every severity level | Time, level, tag, file path, line number, and function name | Only `ELOG_FMT_LVL \| ELOG_FMT_TAG`, plus the message |

Configuration is in [elog_cfg.h](03_Firmware/01_Bootloader/Middlewares_User/EasyLogger/inc/elog_cfg.h), and initialization is in [debug.c](03_Firmware/01_Bootloader/Tasks/Debug/debug.c). The color-function call is guarded by `#ifdef ELOG_COLOR_ENABLE` so that disabling color does not leave a reference to the removed function.

Shrinking the line buffer directly saves 768 B of static RAM. Color and format changes also reduce associated data and log strings. Asynchronous and buffered output were already disabled before this stage; synchronous output remains in use. Logging and assertions are retained, with `ELOG_OUTPUT_LVL` still set to `ELOG_LVL_VERBOSE`. The 256 B limit covers the entire line buffer; longer messages are truncated.

### SFUD

The hardware uses a fixed W25Q64 device, so automatic parameter discovery and the generic chip-information table are disabled. Basic read, write, erase, and single-line fast-read operations are retained.

| Setting | Optimized configuration |
| --- | --- |
| `SFUD_USING_SFDP` | Disabled |
| `SFUD_USING_FLASH_INFO_TABLE` | Disabled |
| `SFUD_USING_FAST_READ` | Enabled |
| `SFUD_FLASH_DEVICE_TABLE` | Retains the device index, name, SPI1, and fixed chip parameters |
| Capacity | 8 MiB (`8UL * 1024UL * 1024UL`) |
| Write mode | `SFUD_WM_PAGE_256B`, 256-byte page programming |
| Erase granularity / command | 4096 B / `0x20` |

These settings are in [sfud_cfg.h](03_Firmware/01_Bootloader/Middlewares_User/SFUD/inc/sfud_cfg.h). The device name `W25Q64` is a label. Once both parameter sources are disabled, capacity, write mode, and erase parameters must be explicitly supplied through `.chip` in the device table. Initialization uses fixed parameters; review them when changing the device. The 4 KiB erase granularity matches the existing W25Q management layer.

## Resource Comparison

The following figures cover the **entire Bootloader build**, including middleware, business code, and linked libraries. They are not the combined usage of all three firmware projects. Raw byte counts come from `Program Size` in the full builds before and after optimization; the optimized totals also match the `.map` file. Both statistics show LTO disabled.

| Metric | Before (B) | After (B) | Saved (B) |
| --- | ---: | ---: | ---: |
| Code | 20,580 | 18,696 | 1,884 |
| RO-data | 3,136 | 1,588 | 1,548 |
| RW-data | 244 | 172 | 72 |
| ZI-data | 23,016 | 22,248 | 768 |

| Resource | Before | After | Saved | Reduction |
| --- | ---: | ---: | ---: | ---: |
| Flash | 23,960 B / 23.40 KB | 20,456 B / 19.98 KB | **3,504 B / 3.42 KB** | **14.62%** |
| Static RAM | 23,260 B / 22.71 KB | 22,420 B / 21.89 KB | **840 B / 0.82 KB** | **3.61%** |

KB on this page means 1024 B (KiB). Calculations use:

- **Flash = Code + RO-data + RW-data**, including the initial values of initialized data stored in Flash.
- **Static RAM = RW-data + ZI-data**, which does not measure runtime peak stack usage.

The Flash difference of 3,432 B shown at the bottom of keil-build-viewer corresponds to the change in `Code + RO-data`. Including the 72 B reduction in RW-data initial values gives a total Flash saving of 3,504 B, or 3.42 KB.

### Before Optimization

![Bootloader resource statistics before optimization](08_docs/image/bootloader-resource-before.png)

### After Optimization

![Bootloader resource statistics after optimization](08_docs/image/bootloader-resource-after.png)

## Build and Validation

The setup uses STM32F411CEU6, W25Q64, AT24C02, and Keil MDK 5 / ARMCC 5.06 update 7. Debugging uses J-Link / RTT Viewer; firmware transfer uses a YMODEM-capable serial tool and the matching encryption tool.

1. Open `03_Firmware/01_Bootloader/MDK-ARM/STM32F411CEU6_SPL.uvprojx`, perform a full build, and save the `.map` file and resource statistics.
2. APP1 and APP2 are linked at `0x08008000`. Encrypt their generated BIN files into `.mxxx` packages for initial installation and APP1 → APP2 update verification.
3. Flash the Bootloader and follow each project's README for reception, installation, and startup. Identify the running application through RTT logs.

The optimized full build reports **0 Errors and 0 Warnings**. Hardware functional validation was completed with no issues found.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
