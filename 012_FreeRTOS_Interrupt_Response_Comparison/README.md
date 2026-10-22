**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 FreeRTOS Interrupt Response-Time Optimization Experiment

## Overview

This project measures and optimizes the response time of a push-button external-interrupt path. The optimization targets the internal edge-event queue between the ISR and the key task. The original mode copies an entire `key_press_event_t` when sending and receiving the queue; the optimized mode transfers only a pointer to the event object, reducing the cost of copying a large structure. Two independent `.c/.h` implementations are retained and selected by one configuration switch, enabling a controlled comparison in a single project.

## Experiment Design

Pressing KEY1 changes PA0 from high to low and triggers EXTI0. The ISR creates an edge event and calls `xQueueSendFromISR()`, then toggles the PB12 trace pin near the end of the interrupt path. A logic analyzer captures PA0 and PB12 simultaneously:

```text
T_response = t(PB12 toggle) - t(PA0 falling edge)
```

This interval includes exception entry, HAL callback dispatch, event preparation, queue sending, GPIO configuration, and the trace-pin operation. It is the end-to-end interrupt-path time defined by this project, not the processor's bare exception-entry latency. Because PB12 toggles after the queue send, the results directly capture the ISR-side difference. The task-side receive also copies only a pointer in pointer mode, but it occurs outside this measurement window.

## Mode Selection and Optimization

The master switch is in `BSP/key/inc/bsp_key_config.h`:

```c
#define BSP_KEY_QUEUE_POINTER_MODE    0U
```

- `0U`: use `bsp_key_queue_value.c/.h` and copy the complete event object;
- `1U`: use `bsp_key_queue_pointer.c/.h` and copy only an event-object pointer.

Both pairs are included in the Keil project, while conditional compilation ensures that only the selected pair is active. `Core/Src/freertos.c` includes the matching header using the same switch. This optimization applies only to the internal `s_key_edge_queue`; the public `g_key_queue` continues to transfer `key_event_t` values.

## Test Results

| Test case (total structure size) | By value | By pointer | Reduction | Reduction rate | Average reduction per Byte |
|---|---:|---:|---:|---:|---:|
| Baseline: array commented out (8 Byte) | 10.85 µs | 10.85 µs | 0 µs | 0% | 0 |
| `uint32_t test_payload[32]` (136 Byte) | 20.40 µs | 10.85 µs | 9.55 µs | 46.81% | 0.0746 µs/Byte |
| `uint32_t test_payload[64]` (264 Byte) | 29.30 µs | 10.85 µs | 18.45 µs | 62.97% | 0.0695 µs/Byte |
| `uint32_t test_payload[100]` (408 Byte) | 39.40 µs | 10.85 µs | 28.55 µs | 72.46% | 0.0701 µs/Byte |

The benefit increases with structure size. For 408 Byte, pointer transfer reduces the measured interval by 28.55 µs, or 72.46%. On the PC, logic analyzer, MCU, and software configuration used for this experiment, each additional structure byte corresponds to an average reduction of approximately **0.071 µs** when using a pointer. This value describes only the trend in these measurements and is not a platform-independent performance constant.

## Test Platform and Constraints

| Item | Configuration |
|---|---|
| MCU / clock | STM32F411CEU6 (Cortex-M4F), 100 MHz |
| Software | STM32 HAL, FreeRTOS V10.3.1 |
| Toolchain | Keil MDK-ARM 5, ARM Compiler 5.06 update 7 (build 960), O3 |
| Firmware package | STM32Cube FW_F4 V1.28.3 |
| Measurement interval | PA0 input falling edge to PB12 trace edge |

Structure size depends on the compiler ABI, enum width, and memory-alignment rules. Results may also vary with MCU and clock, compiler optimization, FreeRTOS and interrupt load, analyzer sample rate and trigger threshold, probes and wiring, or debugger activity. Keep these conditions unchanged when repeating the experiment, and record the sample count, minimum, maximum, and average values.

## Reproduction and Known Limitation

Connect the logic analyzer to PA0, PB12, and GND. Keep the `test_payload` length identical in both key headers, then perform a full rebuild, flash, and repeated measurement once with `0U` and once with `1U`.

The current pointer implementation reuses one static event object. If a new edge occurs before an older event is consumed, multiple queued pointers may refer to the same updated object. This is suitable for the controlled performance experiment but should not be used directly in a high-concurrency production design. A production implementation should use a static object pool or another buffer scheme with explicit ownership.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
