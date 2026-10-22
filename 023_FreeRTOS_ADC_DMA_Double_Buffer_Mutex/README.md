**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 ADC DMA Double Buffering with Mutex Synchronization

## Project Description

This project uses the STM32F411CEU6, FreeRTOS, and STM32 HAL to implement ADC DMA double-buffer acquisition, inter-task communication, mutex-protected buffer handoff, raw-data processing, and voltage reporting.

### Introduction

ADC1 alternates between two dynamically allocated buffers using DMA in Normal mode. `adc_sample_task` handles DMA completion events, switches the DMA destination, and publishes the completed buffer. `adc_process_task` reads the completed buffer, converts the 12-bit ADC value to a voltage, and reports the result through EasyLogger and SEGGER RTT.

The tasks coordinate buffer ownership through the buffer-ready queue and a mutex. Before reusing a buffer, the sampling task confirms that the previous ready message has been removed by the processing task, then waits for the processing task to release the mutex. This prevents DMA and the processing task from accessing the same buffer concurrently and prevents unprocessed data from being overwritten.

### Main Features

- Dynamically allocates two 4-byte buffers for alternating ADC DMA acquisition and processing.
- Uses `dma_done_queue` to deliver DMA completion events from the interrupt callback to the sampling task.
- Uses `adc_ready_queue` to deliver completed-buffer identifiers from the sampling task to the processing task.
- Uses `adc_buffer_mutex` together with the ready-queue state to coordinate buffer ownership and prevent concurrent access or premature reuse.
- Converts 12-bit ADC values to millivolts using a 3.3 V reference and reports them through EasyLogger and SEGGER RTT.
- Checks memory allocation, queue creation, mutex creation, task creation, DMA startup, queue communication, and ADC error conditions.

### Processing and Synchronization Flow

1. `app_init()` allocates the two ADC buffers and creates two queues, one mutex, and two application tasks.
2. `adc_sample_task` starts the first DMA transfer into `p_buffer1`.
3. When the transfer completes, `HAL_ADC_ConvCpltCallback()` sends an event to `dma_done_queue` from interrupt context and requests a task switch when necessary.
4. After receiving the event, `adc_sample_task` waits until the previous `adc_ready_queue` message has been removed. It then takes `adc_buffer_mutex` to confirm that the processing task has finished reading, converting, and reporting the previous buffer.
5. The sampling task starts DMA on the alternate buffer, releases the mutex, and publishes the newly completed buffer identifier through `adc_ready_queue`.
6. `adc_process_task` waits for a ready message with `xQueuePeek()`, takes the mutex, and removes the message with a non-blocking `xQueueReceive()`. It releases the mutex after processing the buffer and reporting the result.

```text
DMA completion interrupt
    │
    └─ dma_done_queue ──> adc_sample_task
                              │
                              ├─ Wait until the prior ready message is removed
                              ├─ Take mutex and wait for prior processing
                              ├─ Start DMA on the alternate buffer
                              ├─ Release mutex
                              │
                              └─ adc_ready_queue ──> adc_process_task
                                                        │
                                                        ├─ Peek ready message
                                                        ├─ Take mutex
                                                        ├─ Receive and process buffer
                                                        └─ Report and release mutex
```

### Repository Structure

```text
.
├── App/                 # ADC buffering, queue, and mutex application code
├── Core/                # CubeMX-generated initialization and interrupt code
├── Docs/                # Project documentation
├── Drivers/             # STM32 HAL and CMSIS drivers
├── Images/              # README and project images
├── Middlewares/         # FreeRTOS, EasyLogger, and SEGGER RTT
├── MDK-ARM/             # Keil MDK project
├── References/          # Reference material
└── STM32F411CEU6.ioc    # STM32CubeMX configuration
```

## Usage Guide

After preparing the environment and connections below, build and flash the project, then view the acquisition results through RTT.

### Environment and Dependencies

| Item | Requirement |
| --- | --- |
| Target MCU | STM32F411CEU6 |
| Build environment | Keil MDK-ARM V5.32, ARM Compiler 5.06 update 7 |
| Configuration tool | STM32CubeMX 6.16.1 (only required when changing peripheral configuration) |
| Software components | FreeRTOS 10.3.1, EasyLogger 2.2.99, and SEGGER RTT 8.12g (included in this repository) |
| Debug tools | J-Link and SEGGER RTT Viewer |

### Hardware Connection and Configuration

| Connection | Description |
| --- | --- |
| PA1 / ADC1_IN1 | Connect a 0–3.3 V analog signal |
| GND | Share ground with the signal source and debugger |
| SWDIO / SWCLK | Connect to J-Link for flashing and RTT log viewing |

ADC1 uses 12-bit resolution, right-aligned data, and software-triggered single conversions. DMA is configured as DMA2 Stream 0, Channel 0, peripheral-to-memory, word-aligned, and Normal mode; each transfer contains one 32-bit sample. Do not exceed the MCU's permitted input-voltage range.

### Build, Run, and Observe

1. Connect the analog signal, common ground, and debugger.
2. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil.
3. Build the project, flash it to the target board, and reset the MCU.
4. Open SEGGER RTT Viewer and observe the alternating raw ADC and voltage values from both buffers.

Example output:

```text
Buffer1 raw:2048, voltage:1.650 V
Buffer2 raw:2048, voltage:1.650 V
```

### Porting Notes

- Reconfigure the ADC channel, GPIO, DMA, and interrupt for the target MCU.
- Update `ADC_MAX_RAW_VALUE` and `VDD_VALUE` if the ADC resolution or reference voltage changes.
- If the samples per transfer change, update `ADC_BUFFER_SIZE`, the buffer-processing logic, and the DMA transfer length together.
- If the task count or buffer-handoff design changes, review the ownership constraints of `adc_ready_queue` and `adc_buffer_mutex`.
- Reimplement the EasyLogger output port when SEGGER RTT is not used.

## Author's Reflections

##### Experiment:

1. Use `malloc` to allocate two 4-byte ADC data buffers named `Buffer1` and `Buffer2`, and define the DMA buffer state, task handles, and queue handles.
2. Create two message queues: `dma_done_queue` for communication from the interrupt to the sampling task, and `adc_ready_queue` for communication from the sampling task to the processing task.
3. Create the ADC sampling task `adc_sample_task` and ADC processing task `adc_process_task` for DMA double-buffer control and ADC data processing.
4. Implement the ADC DMA conversion-complete callback. When a DMA transfer completes, use `xQueueSendFromISR()` to send a completion message to the sampling task and request a context switch when necessary.
5. Implement DMA double-buffer switching in Task A, the ADC sampling task. Start with `Buffer1`; after completion, switch to `Buffer2` and notify Task B to process `Buffer1`. On the next completion, switch back to `Buffer1` and notify Task B to process `Buffer2`.
6. In Task B, the ADC processing task, read the completed buffer, convert the 12-bit ADC value to a voltage using the 3.3 V reference, and output both the raw and calculated values through EasyLogger.
7. Add error handling for memory allocation, queue creation, task creation, DMA startup, and queue communication, and handle ADC overrun and DMA transfer errors in the ADC error callback.

##### Further Improvements:

1. ###### Problem Description

   >There are currently two tasks, A and B, and two buffers, buffer 1 and buffer 2. Task A writes data to the buffers, while Task B reads data from them. In this producer-consumer relationship, Task A is the producer and Task B is the consumer.
   >
   >The following situations may occur:
   >
   >1. Task A may overwrite a buffer before Task B has consumed its previous contents.
   >2. Tasks A and B may read and write the same buffer concurrently.

2. ###### Solution Stage 1: Introduce a Mutex

   >Task B takes the mutex while processing a buffer, and Task A must also try to take the mutex before switching buffers. If Task B is using the buffer, Task A cannot take the mutex and must wait until Task B finishes and releases it.

3. ###### Remaining Issue After Introducing the Mutex

   >Suppose the system contains not only Tasks A and B, but also a Task C whose priority is higher than both. The following situation may occur:
   >
   >Task A sends a buffer-complete message to Task B through the queue and expects Task B to run. Because Task C has a higher priority, however, Task C runs first. Task B receives no CPU time and therefore never takes the mutex.
   >
   >After Task C finishes, Task A runs again and attempts to take the mutex. Because Task B never took it, Task A succeeds even though Task B has not processed the buffer. This creates a synchronization gap between the tasks.

4. ###### Solution Stage 2: Check Whether the Buffer-Complete Queue Has Been Consumed Before Task A Takes the Mutex

   >Before taking the mutex, Task A checks whether the buffer-complete message it sent to Task B has been consumed.
   >
   >If the message has been consumed, Task B has processed the corresponding buffer. If it has not been consumed, Task B has not yet processed that buffer.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
