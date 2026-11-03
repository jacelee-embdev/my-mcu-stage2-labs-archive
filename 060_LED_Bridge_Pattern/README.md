**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# LED Bridge Pattern for STM32F411CEU6

This project records my practice with object-oriented design in embedded C. Structures encapsulate object state, function pointers expose operations, and the bridge pattern separates LED behavior from hardware and operating system implementations. Application code submits blink requests without directly manipulating GPIO or FreeRTOS queues, making it easier to replace low-level interfaces and add LED instances.

## Module Design

| Module | Responsibility |
|---|---|
| `led.c/.h` | Initialize PC13 and provide `LED_ON()`, `LED_OFF()`, and toggle operations |
| `bsp_led_driver.c/.h` | Encapsulate individual LED state, blink parameters, and hardware, time-base, and delay interfaces |
| `bsp_led_handler.c/.h` | Register Driver objects and schedule asynchronous blinking through a message queue |
| `system_adaption.c/.h` | Bind hardware and FreeRTOS services, construct objects, and register the onboard LED |
| `os_init`, `led_task`, `status_task` | Create tasks, run blink tests, and report task status |

Both Driver and Handler use caller-provided object memory. The system layer supplies static objects and read-only interface tables that remain valid during background access. OS support is enabled by default, while synchronous Driver control is disabled; the Handler executes asynchronous blinking.

## Usage and Testing

Initialize the LED hardware and logging first, then call `System_InitResources()` after the scheduler starts. Driver construction includes a 600 ms delay. The system then creates the Handler queue and worker thread and registers the Driver.

`System_LEDControl()` accepts a period, blink count, and on/off ratio. The Handler supports up to 10 registered Drivers and a 10-item queue, with ratios of 1:1, 1:2, and 1:3. Validated requests are copied into the queue without waiting. `SYSTEM_OK` means the request was enqueued; the worker executes requests sequentially and logs execution results separately. Each processing iteration ends with a 1000 ms delay before polling again. Registration-table access uses a critical section; delays and LED operations run outside it.

The current `led_task` repeatedly tests periods of 400, 800, and 1200 ms. Each period cycles through all three ratios, with three blinks per group. Between groups, it waits for the group's nominal execution duration plus 2000 ms. The `status_task` logs every 5000 ticks and does not control the LED.

## Startup Markers

The project retains the dedicated `bsp_target` object section and custom linker layout. Initialization flags start at `0xEC` and become `0x01` after successful instantiation. Startup copying only prepares initial RAM contents; interface binding, OS resource creation, and the Driver delay still occur at runtime.

## Build and Verification

The target is an STM32F411CEU6, with an active-low LED on PC13. Open the [project file](MDK-ARM/STM32F411CEU6.uvprojx) in Keil MDK 5 / ARM Compiler 5, build, and download through J-Link/SWD. Use RTT Viewer channel 0 for EasyLogger output and SystemView for task tracing.

After flashing, the expected behavior is repeated groups of three PC13 LED blinks, cycling through the periods and on/off ratios above. Check RTT logs for resource initialization, queued requests, execution results, and periodic status messages. An enqueue-success message alone does not confirm that blinking completed.

The project's existing verification notes report that both synchronous-control configurations built and linked with **zero errors and warnings**, and that the updated LED functionality passed on-board verification. These are prior project records. This archive update checks file integrity, project references, ignore rules, and documentation links; it does not repeat compilation, flashing, or hardware testing.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
