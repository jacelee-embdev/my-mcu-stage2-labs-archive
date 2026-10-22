**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 ADC DMA Double-Buffer Acquisition Experiment

## Project Description

This project uses the STM32F411CEU6, FreeRTOS, and STM32 HAL to implement ADC DMA double-buffer acquisition, inter-task communication, raw-data processing, and voltage reporting.

##### Introduction

ADC1 transfers samples into two dynamically allocated buffers through DMA. The sampling task switches the DMA target, while the processing task converts each 12-bit ADC value to a voltage and reports the result through EasyLogger.

##### Main Features

- Alternates ADC DMA acquisition between two 4-byte buffers.
- Uses two FreeRTOS queues to connect the DMA interrupt, sampling task, and processing task.
- Converts 12-bit ADC values to voltages and outputs logs through SEGGER RTT.
- Checks memory, queue, task, DMA, and ADC error conditions.

##### Repository Structure

```text
.
├── App/                 # ADC double-buffer application code
├── Core/                # CubeMX-generated initialization and interrupt code
├── Drivers/             # STM32 HAL and CMSIS drivers
├── Middlewares/         # FreeRTOS, EasyLogger, and SEGGER RTT
├── MDK-ARM/             # Keil MDK project
└── STM32F411CEU6.ioc    # STM32CubeMX configuration
```

## Usage Guide

After preparing the environment and connections below, build and flash the project, then view the acquisition results through RTT.

##### Environment and Dependencies

| Item | Requirement |
| --- | --- |
| Target MCU | STM32F411CEU6 |
| Build environment | Keil MDK-ARM V5.32, ARM Compiler 5.06 update 7 |
| Configuration tool | STM32CubeMX 6.16.1 (only required when changing peripheral configuration) |
| Software components | FreeRTOS 10.3.1, EasyLogger 2.2.99, and SEGGER RTT 8.12g (included in this repository) |
| Debug tools | J-Link and SEGGER RTT Viewer |

##### Hardware Connection and Configuration

| Connection | Description |
| --- | --- |
| PA1 / ADC1_IN1 | Connect a 0–3.3 V analog signal |
| GND | Share ground with the signal source and debugger |
| SWDIO / SWCLK | Connect to J-Link for flashing and RTT log viewing |

ADC1 uses 12-bit resolution and right-aligned data. DMA is configured as DMA2 Stream 0, Channel 0, peripheral-to-memory, word-aligned, and Normal mode. Do not exceed the MCU's permitted input-voltage range.

##### Build, Run, and Observe

1. Connect the analog signal, common ground, and debugger.
2. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
3. Build the project, flash it to the target board, and reset the MCU.
4. Open SEGGER RTT Viewer to observe the raw ADC and voltage values from both buffers.

Example output:

```text
Buffer1 raw:2048, voltage:1.650 V
Buffer2 raw:2048, voltage:1.650 V
```

##### Porting Notes

- Reconfigure the ADC channel, GPIO, DMA, and interrupt for the target MCU.
- Update `ADC_MAX_RAW_VALUE` and `VDD_VALUE` if the ADC resolution or reference voltage changes.
- Adjust the buffer size, DMA length, and queue capacity if the sample count changes.
- Reimplement the EasyLogger output port when SEGGER RTT is not used.

## Author's Notes

**Experiment**:

1. Use `malloc` to allocate two 4-byte ADC data buffers named `Buffer1` and `Buffer2`, and define the DMA buffer state, task handles, and queue handles.
2. Create two message queues: `dma_done_queue` for communication from the interrupt to the sampling task, and `adc_ready_queue` for communication from the sampling task to the processing task.
3. Create the ADC sampling task `adc_sample_task` and ADC processing task `adc_process_task` for DMA double-buffer control and ADC data processing.
4. Implement the ADC DMA conversion-complete callback. When a DMA transfer completes, use `xQueueSendFromISR()` to send a completion message to the sampling task and request a context switch when necessary.
5. Implement DMA double-buffer switching in Task A, the ADC sampling task. Start with `Buffer1`; after completion, switch to `Buffer2` and notify Task B to process `Buffer1`. On the next completion, switch back to `Buffer1` and notify Task B to process `Buffer2`.
6. In Task B, the ADC processing task, read the completed buffer, convert the 12-bit ADC value to a voltage using the 3.3 V reference, and output both the raw and calculated values through EasyLogger.
7. Add error handling for memory allocation, queue creation, task creation, DMA startup, and queue communication, and handle ADC overrun and DMA transfer errors in the ADC error callback.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
