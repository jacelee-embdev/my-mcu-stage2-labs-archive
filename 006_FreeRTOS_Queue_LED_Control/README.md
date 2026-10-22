**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Queue-Based LED Control

This project records connecting button input and LED control through message queues, focusing on communication, module encapsulation, and separation of responsibilities. It uses STM32F411CEU6, HAL, and FreeRTOS, with separate BSP modules for the button and LED.

## Message Flow

The button thread scans input and sends states to a button queue. The CubeMX default thread acts as a relay, converting received states into LED commands and sending them to an LED queue. The LED thread waits for commands and toggles the output. The path is: button thread → button queue → relay thread → LED queue → LED thread.

`Core/Src/freertos.c` creates the threads and relays messages. BSP drivers perform the hardware operations. Inspect the queue handles, message types, and send/receive calls to see how the modules cooperate without directly calling each other's drivers.

## Build and Run

Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx) with Keil MDK and ARM Compiler 5, rebuild, and download. Connect the button and LED according to the hardware configuration. Observe LED changes and use UART logs to inspect relay execution and error paths. When adding commands, update message definitions, conversion logic, and LED handlers together, and check queue creation and capacity.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
