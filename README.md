**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# My MCU Stage 2 Labs Archive

An archive of projects from the second stage of my embedded systems learning, covering FreeRTOS, DMA, bootloaders, OTA updates, embedded debugging, and more.

## About This Archive

This repository preserves the projects and notes accumulated during the second stage of my embedded systems learning. It serves as a personal learning archive and code backup for revisiting experiments and reproducing their results.

The learning topics include task scheduling and synchronization, data transfer and buffering, firmware upgrades, encryption, fault diagnosis, linker scripts, and more. Projects are numbered in learning order and retain the implementation and experiment state from that point in the learning process.

Each lab is opened and built separately. Some labs contain related firmware projects, such as a bootloader and its applications. Refer to the lab's documentation for its hardware, toolchain, build sequence, and expected behavior.

## Project Index

Select a project name to open its English documentation.

| No. | Project | Description |
|---:|---|---|
| 001 | [Button_LED_CubeMX_V1](001_Button_LED_CubeMX_V1/README.md) | Poll a button to toggle an LED in a CubeMX project, with periodic UART test output. |
| 002 | [Button_LED_Template_V2](002_Button_LED_Template_V2/README.md) | Poll a button to toggle an LED using a custom HAL template with separate BSP and system drivers. |

## Usage

1. Clone or download this repository, then select a lab from the project index.
2. Read the lab's `README.md` and check its MCU, hardware connections, required components, and development environment.
3. Open the corresponding `.uvprojx` file in Keil MDK and install the compiler and device packs required by that project.
4. Rebuild the project to generate its firmware. If the lab contains related firmware projects, follow the documented build and flashing sequence.
5. Flash the firmware with the configured debugger, then check the serial output, LEDs, or other documented results.

Reproducible build outputs are excluded from version control. When using a lab independently, keep its source files, project configuration, `LICENSE`, and `.gitignore` together. Hardware and toolchain details may vary between labs; use the corresponding documentation and source as the reference.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the repository root for the full terms. Third-party components are subject to their respective licenses.
