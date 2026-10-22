**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Bootloader Space Optimization

## Project Description

##### Introduction

Building on project 044's encrypted firmware OTA project, this experiment uses Keil Build Viewer to analyze Bootloader resource usage and compares `-O0` with `-O3` to demonstrate space savings from compiler optimization. The source code is unchanged; no source-level space optimization was performed.

This STM32F411CEU6 project contains a Bootloader, APP1, and APP2. It demonstrates initial installation with no APP present, followed by downloading and updating firmware while an application is running. The host sends an encrypted package over serial YMODEM. The APP commits the update state and requests a reset; the Bootloader handles decryption, old APP backup, installation, and launch.

##### Main Features

- The Bootloader supports downloads triggered by a key. The APP supports encrypted firmware reception during operation through USART1 DMA double buffering.
- External Flash A initially stores the new encrypted package, and B stores its decrypted plaintext. After decryption succeeds, a valid old APP is backed up into A before the new APP from B is installed into internal Flash.
- EEPROM stores update state and lengths: `0x00` for normal startup, `0x11` for a download in progress or incomplete, and `0x22` for a complete package ready to install. A valid old APP backup made during the current update enables recovery after installation failure.
- APP1 and APP2 share the same functions, hardware configuration, and OTA flow. Only the test task's log message and delay differ, identifying the application running after an update.

| Project | Test log | Loop delay / PC13 toggle interval | Role |
| --- | --- | --- | --- |
| [Bootloader](03_Firmware/01_Bootloader/README.md) | `Bootloader running...` | — | Standard Peripheral Library; startup, decryption, and installation |
| [APP1](03_Firmware/02_APP1/README.md) | `APP1 running...` | 500 ms | HAL + FreeRTOS; initially installed application |
| [APP2](03_Firmware/02_APP2/README.md) | `APP2 running...` | 200 ms | Same application functions as APP1; update target |

The APP toggles PC13 once per loop; the table shows the interval between level changes.

##### Repository Structure

```text
045_Bootloader_Compiler_Optimization/
├── 00_Reference/      # Reference material
├── 01_FunctionMap/    # Functional breakdown
├── 02_Hardware/       # Hardware material
├── 03_Firmware/
│   ├── 01_Bootloader/       # Bootloader
│   ├── 02_APP1/             # Initial application
│   ├── 02_APP2/             # Update target
│   └── 04_BIN_EncryptionBIN/ # Plaintext firmware and encrypted packages
├── 08_README_docs/    # Resource usage screenshots before and after optimization
└── README.md          # Project overview
```

The [firmware directory](03_Firmware/04_BIN_EncryptionBIN/) contains both unencrypted APP1/APP2 binaries and encrypted packages:

| File | Size (bytes) | Purpose |
| --- | --- | --- |
| `APP1.bin` | 39,420 | APP1 plaintext firmware |
| `APP1_EncryptionBIN.mxxx` | 39,440 | Encrypted package sent to the Bootloader for initial installation |
| `APP2.bin` | 39,420 | APP2 plaintext firmware |
| `APP2_EncryptionBIN.mxxx` | 39,440 | Encrypted package sent during the OTA update from APP1 |

## Usage Guide

##### Environment and Dependencies

- Hardware: STM32F411CEU6, W25Q series SPI Flash, AT24C02 EEPROM, a PA0 key, a PC13 LED, a USB-to-serial adapter, and J-Link.
- Software: Keil MDK (all three projects select ARM Compiler 5.06 update 7), SecureCRT with YMODEM support, and a serial utility that sends HEX bytes. J-Link RTT Viewer V8.12g was used during testing. The Bootloader uses the Standard Peripheral Library; both APPs use HAL and FreeRTOS V10.3.1. Required libraries are included.

##### Hardware Connection and Configuration

| Interface | Pins and settings |
| --- | --- |
| Serial update | USART1: PA9/TX → adapter RX, PA10/RX ← adapter TX; 115200, 8N1, no flow control |
| External Flash | PA4/CS, PA5/SCK, PA6/MISO, PA7/MOSI; SPI mode 0 |
| EEPROM | PB8/SCL, PB9/SDA; software I²C |
| Key / LED | PA0 with pull-up, grounded when pressed; PC13 indicator LED |
| Debug and power | J-Link over SWD; common ground and compatible 3.3 V logic levels |

| Storage region | Address range | Purpose |
| --- | --- | --- |
| Internal Bootloader | `0x08000000–0x08007FFF` (32 KiB) | Bootloader code |
| Internal APP | `0x08008000–0x0801FFFF` (96 KiB) | Execution region shared by APP1 and APP2 |
| External A | `0x000000–0x00FFFF` (64 KiB) | New encrypted package; optionally overwritten with the old APP plaintext backup after decryption |
| External B | `0x010000–0x01FFFF` (64 KiB) | New APP plaintext awaiting installation |

Both APPs are linked at `0x08008000` and installed sequentially into the same execution region. Encrypted packages must be no larger than 64 KiB, aligned to 16 bytes, and match the Bootloader's AES-256 CBC header format, key, and IV.

##### Resource Usage Analysis: Keil Build Viewer

1. **Download the tool**: Open the [Keil Build Viewer releases page](https://github.com/DinoHaw/keil-build-viewer/releases) and download `keil-build-viewer.exe` from a release's **Assets**.
2. **Place it in the project directory**: Put the executable beside the `.uvprojx` project file. For this Bootloader, the directory is `03_Firmware/01_Bootloader/MDK-ARM/`.
3. **Run it after building**: Open **Options for Target → User**, enable **Run #1** under **After Build/Rebuild**, and enter:

   ```text
   .\keil-build-viewer.exe
   ```

4. **Build and view the results**: The tool runs automatically after the build. The **Build Output** window shows per-file Flash and RAM usage, overall usage, and changes since the previous build.

The executable and after-build command are already included in this Bootloader project, so building it directly displays the results.

##### Space Savings from Compiler Optimization: `-O0` versus `-O3`

This experiment uses the same Bootloader source code and compares space usage at two compiler optimization levels.

1. Open the Bootloader project. Under **Options for Target → C/C++ → Optimization**, select **Level 0 (`-O0`)**, run **Rebuild**, and record the usage.
2. Select **Level 3 (`-O3`)**, run **Rebuild** again, and compare the results. Keep all other compiler and linker settings unchanged.

**Compiler optimization disabled: `-O0`**

![Bootloader resource usage at -O0](<08_README_docs/Level 0(-O0)_Resource_Usage.png>)

**Level 3 compiler optimization enabled: `-O3`**

![Bootloader resource usage at -O3](<08_README_docs/Level 3(-O3)_Resource_Usage.png>)

| Metric | `-O0` | `-O3` |
| --- | --- | --- |
| Flash usage (tool display) | About 22.9 KB | About 20.4 KB |
| Usage of the 32 KB Bootloader region | 71.6% | 63.9% |
| RAM usage (tool display) | About 21.8 KB | About 21.8 KB |
| RAM usage percentage | 17.1% | 17.1% |

In this example, `-O3` reduces Flash usage by **2,516 bytes (about 2.46 KiB)** compared with `-O0`, while RAM usage remains essentially unchanged. The savings come from compiler optimization; both screenshots show LTO disabled. No source-level space optimization was performed.

`-O3` does not guarantee the smallest code size for every project. Compare actual build results and verify runtime behavior after changing the optimization level. See the [Keil compiler options](https://www.keil.com/support/man/docs/uv4/uv4_dg_adscc.asp).

##### Build, Run, and Observe

1. Open and build the Keil project in each project's `MDK-ARM/` directory. See the three project READMEs linked above for detailed instructions.
2. For the initial test, separately clear the update flag at EEPROM address `0x00` to `0x00`, then select **Erase Chip** to erase MCU internal Flash and program the Bootloader. No APP is present at this stage. Erase Chip does not clear external Flash or EEPROM.
3. After reset, wait about 13 seconds and press the key to enter Bootloader download mode. Once repeated `C` characters appear, send `APP1_EncryptionBIN.mxxx` over YMODEM. The Bootloader receives ciphertext into A, decrypts it into B, skips backup because no old APP exists, then installs and launches APP1.
4. In APP1, send HEX `11 22 33`. After receiving `C`, send `APP2_EncryptionBIN.mxxx` over YMODEM. Once the package and metadata are confirmed, APP1 saves EEPROM state `0x22` and replies with HEX `44 55 66` (displayed as `DUf` in ASCII). APP1 is still running at this point. Close the previous tool's serial connection before switching tools.
5. Send HEX `77 88 99` and press the key within about 20 seconds. The APP requests a system reset to the Bootloader, which decrypts A → B, backs up internal APP1 → A, installs B → the internal APP region, and launches APP2.
6. Observe PC13 and RTT logs: APP1 prints about every 500 ms, and APP2 prints about every 200 ms. If logs stop after an APP launch, reopen RTT Viewer to search for the control block.

**project 044 board regression results on 2026-10-06, after code formatting and comment cleanup:**

| Stage | Observed result |
| --- | --- |
| Initial APP1 installation | Transfer showed 38 KB, about 11 seconds, and 0 Errors. The decryption log reported 39,420 plaintext bytes and no valid old APP. Finally, `APP1 running...` appeared and the LED blinked |
| APP1 receives APP2 | Transfer showed 38 KB, about 11 seconds, and 0 Errors. Received `44 55 66`; the log read `OTA ready: size=39440, EEPROM state=0x22` |
| Update and launch APP2 | The update completed after the command and key confirmation. The final log was `APP2 running...`, appearing about every 200 ms |

Initial installation and the normal APP1 → APP2 `0x22` update passed regression after code formatting and comment cleanup. State `0x11` interruption recovery, rollback after installation failure, and a reverse update were not part of this board test.

##### Porting Notes

Update the Bootloader and both APPs together: pins, clocks, Flash sectors and capacity, APP link address and vector table, external A/B regions, and EEPROM metadata layout. Keep serial commands, encrypted package format, key, and IV consistent. The tested platform is STM32F411CEU6.

## Author's Notes

##### Project-Related Reflections

None.

##### Other Reflections

None.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
