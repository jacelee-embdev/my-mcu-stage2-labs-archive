**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS Short and Long Button Presses

This project records identifying short and long button presses under FreeRTOS and controlling an LED through message queues. Button detection, event conversion, and LED execution are separated into threads and BSP modules to explore state detection and asynchronous communication.

## Behavior and Organization

A short press toggles the LED; a long press blinks it three times. The button thread scans input, applies debounce and duration logic, and sends recognized events to the button queue. The default thread maps these events to commands in the LED queue, and the LED thread performs the hardware operation.

`BSP/key/inc/bsp_key.h` defines the interfaces, states, and events; `BSP/key/src/bsp_key.c` implements detection. LED interfaces and command processing are in `BSP/led`. Thread creation and message conversion are in `Core/Src/freertos.c`. Keep the event and command types consistent when extending the behavior.

## Build and Observe

Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx) with Keil MDK and ARM Compiler 5, rebuild, and download. Connect the button and LED according to the GPIO configuration. Try brief and sustained presses and observe toggling and three blinks. Use the button driver as the reference for thresholds and polling intervals. When adjusting them, check release handling, repeated triggers, and queue results so separate presses remain distinct.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
