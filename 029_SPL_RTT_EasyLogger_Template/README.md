**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 SPL Project Template

This is a basic STM32F411CEU6 project template based on the STM32F4 Standard Peripheral Library (SPL). It integrates EasyLogger and SEGGER RTT and can be used as the starting point for future projects.

The project has been verified by building and running it on hardware. It can be downloaded successfully and reaches `main()`.

## Development Environment

- Keil MDK-ARM 5.38
- Arm Compiler 5.06 update 7
- Keil STM32F4xx DFP 2.17.1
- STM32F4xx SPL (STSW-STM32065 V1.9.0)

## Project Structure

```text
STM32F411CEU6_SPL_Template
├── BSP
├── Core
│   ├── Inc
│   └── Src
├── Drivers
│   ├── CMSIS
│   └── STM32F4xx_StdPeriph_Driver
├── MDK-ARM
├── Middlewares
├── OBJ
├── SYSTEM
├── Tasks
├── .gitignore
├── LICENSE
├── README.md
└── README_zh.md
```

## Usage

1. Open `MDK-ARM/STM32F411CEU6_SPL.uvprojx` with Keil.
2. Build the project.
3. Configure ST-Link and download the program to the target board.
4. View EasyLogger and RTT debug output with SEGGER RTT Viewer.

## Logging Support

- EasyLogger logging
- SEGGER RTT real-time debug output

EasyLogger is connected to SEGGER RTT Terminal 0 through its port layer.

## Verification Status

- Build result: 0 errors, 0 warnings
- Startup file: `startup_stm32f411xe.s`
- Target MCU: STM32F411CEU6
- Verified with ST-Link to reach `main()` and continue running
- EasyLogger and SEGGER RTT build and output paths verified

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
