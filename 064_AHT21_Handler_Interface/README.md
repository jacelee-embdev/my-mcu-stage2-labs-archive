**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AHT21 Temperature and Humidity Handler

This stage defines the Handler interface on top of the existing AHT21 Driver baseline. The Driver triggers measurements over software I²C, checks the CRC, and converts the raw sensor data to temperature and relative humidity. The Handler is designed around queued measurement requests. The caller submits a request, and a worker thread retrieves it from the queue for processing.

The current stage provides the public declarations and a source skeleton registered in the Keil project. Queue processing, completion callbacks, and application integration remain to be implemented.

## Learning notes

This project records the interface design for asynchronous request handling, separating the synchronous Driver from the Handler. Events describe requested quantities, reading freshness, and result delivery; platform interfaces reserve queue and scheduling dependencies. These declarations describe the intended contract rather than an operating queue flow. The application retains the direct Driver sampling baseline. Handler validation requires implementation of event processing, cache updates, worker execution, and application integration. Check module power and voltage compatibility before connecting.

## Handler interface

- `bsp_temp_humi_handler_inst()` is the initialization entry point. It takes the platform interfaces for I²C, the millisecond time base, OS queue operations, and RTOS delay. These are the dependencies for setting up the Driver and the request queue.
- `bsp_temp_humi_read()` is the request submission entry point. A request can ask for temperature, relative humidity, or both. The event carries a maximum reading age, an event timestamp, output pointers, and a callback. The return value describes the status of submitting the request. Measurement processing is assigned to the worker thread.
- `temp_humi_handler_thread()` is the worker entry point for queued requests. The intended flow uses the requested measurement type and maximum reading age to decide whether a fresh measurement is needed.

See the public declarations in [sw_bsp_aht21_handler.h](Z02_BSP/AHT21/sw_bsp_aht21_handler/Inc/sw_bsp_aht21_handler.h) and the source skeleton in [sw_bsp_aht21_handler.c](Z02_BSP/AHT21/sw_bsp_aht21_handler/Src/sw_bsp_aht21_handler.c).

## Wiring and Driver baseline

Connect SDA to PB13 and SCL to PB14, and connect the sensor and board grounds. The AHT21 uses the 7-bit I²C address `0x38`. Use Keil MDK 5 with ARM Compiler 5 to build the project, then flash through SWD. The project file is [STM32F411CEU6.uvprojx](MDK-ARM/STM32F411CEU6.uvprojx).

The existing FreeRTOS sampling task is registered in `os_init.c`. It binds the GPIO-based software I²C bus and the FreeRTOS time-base and delay callbacks. The task still calls the Driver directly and waits two seconds after each read. With a J-Link attached, RTT Viewer channel 0 shows the EasyLogger output from that task. See the sampling entry point in [aht21_task.c](Z03_Business/AHT21_Task/aht21_task.c).

## Archive validation

This archive preserves the original learning stage and run records. This pass checks source, project references, documentation links, and retained inputs only. It does not rebuild, flash, or test hardware; hardware results described here belong to the imported project history.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
