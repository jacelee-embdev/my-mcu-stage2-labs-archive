**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 FreeRTOS Interrupt-Driven Keys and Timer-Based LED Control

## Overview

This project targets the STM32F411CEU6. GPIO external interrupts capture the press and release edges of two keys, FreeRTOS tasks perform debounce and short/long-press recognition, and message queues decouple key recognition from LED control.

TIM2 periodic interrupts drive LED toggle sequences, so the LED task does not block on delay functions.

## Features

| Input | Result | Behavior |
|---|---|---|
| Key1 short press | `KEY_EVENT_SHORT_PRESS` | Request one LED toggle |
| Key1 long press | `KEY_EVENT_LONG_PRESS` | Request 20 LED toggles, producing ten complete on/off cycles at 5 Hz |
| Key2 short press | `KEY_EVENT_SHORT_PRESS` | Print a short-press log through USART1; leave the LED unchanged |
| Key2 long press | `KEY_EVENT_LONG_PRESS` | Print a long-press log through USART1; leave the LED unchanged |
| Debounce interval or press timeout | `KEY_EVENT_NONE` | Produce no downstream event |

The PC13 LED is active-low and initialized to the on state at startup. A long press produces an even number of toggles, so the LED returns to its previous state when the sequence completes.

## Hardware and Pins

| Function | Pin | Configuration |
|---|---|---|
| Key1 | PA0 / EXTI0 | Pull-up input; press connects to GND |
| Key2 | PA1 / EXTI1 | Pull-up input; press connects to GND |
| LED | PC13 | Push-pull output; active-low |
| USART1 TX/RX | PA9 / PA10 | 115200, 8N1, no flow control |
| SWDIO / SWCLK | PA13 / PA14 | SWD debug interface |

The USB-to-UART adapter must share ground with the board and use 3.3 V logic levels.

## Key Classification

`HAL_GetTick()` captures the press and release timestamps in the EXTI callback with 1 ms resolution. The result is classified after release according to the press duration `Δt`:

| Duration | Classification |
|---:|---|
| `Δt < 10 ms` | Debounce; invalid |
| `10 ms ≤ Δt < 1000 ms` | Short press |
| `1000 ms ≤ Δt ≤ 5000 ms` | Long press |
| `Δt > 5000 ms` | Timeout; invalid |

Key1 and Key2 maintain independent state machines. The EXTI callback records only the key ID, edge, and timestamp; recognition and logging run in task context.

## Software Flow

```text
PA0/PA1 EXTI
    → s_key_edge_queue
    → key_task_entry()
        ├─ Key1 → g_key_queue → StartDefaultTask()
        │          → g_led_queue → led_task_entry()
        │          → pending toggle count → TIM2 → led_timer_callback() → PC13
        └─ Key2 → USART1 log
```

All three queues have a length of 10:

| Queue | Data | Purpose |
|---|---|---|
| `s_key_edge_queue` | `key_press_event_t` | Transfer raw edge events from EXTI |
| `g_key_queue` | `key_event_t` | Transfer Key1 short/long-press results |
| `g_led_queue` | `uint8_t` | Transfer the number of LED toggles to execute |

Task priorities are `led_Task` (High) > `defaultTask` (Normal) > `key_task` (Low).

## Timer-Driven LED Control

TIM2 uses a 100 MHz timer clock, a prescaler factor of 10000, and a counter period of 1000, producing an update interrupt every 100 ms:

```text
100 MHz ÷ 10000 ÷ 1000 = 10 Hz
```

`led_task_entry()` receives a toggle count and updates the shared counter inside a critical section. On each interrupt, `led_timer_callback()` toggles the LED at most once and decrements the remaining count after a successful operation.

- A short press sends `1`: toggle once on the next timer period.
- A long press sends `20`: toggle every 100 ms for ten complete on/off cycles, taking about two seconds.
- A new request replaces any unfinished toggle count; the latest request takes priority.

## Quick Start

1. Connect the keys, LED, SWD debugger, and optional UART adapter according to the table above.
2. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil µVision.
3. Select **Rebuild all target files**, then download the firmware through ST-Link.
4. Reset the board and test both keys. To view Key2 logs, open a serial terminal configured for 115200, 8N1, and no flow control.

Expected Key2 logs:

```text
[I][KEY] Key2 short press.
[I][KEY] Key2 long press.
```

## Main Files

The key and LED modules are under `BSP/key/` and `BSP/led/`. Task creation and event mapping are in `Core/Src/freertos.c`. TIM2 initialization and callback handling are in `Core/Src/tim.c` and `Core/Src/main.c`. The Keil project is under `MDK-ARM/`.

## Operational Notes

- A key result is produced only after release; a held key does not report a long press early.
- ISR and application queues use non-blocking sends. If a queue is full, the current event is dropped and an error is logged.
- When the LED task receives a new request, it replaces the remaining toggle count instead of waiting for the previous sequence to finish.
- `printf` uses blocking USART1 transmission and is intended only for debugging.
- Before regenerating code with STM32CubeMX, back up the project and review changes to `freertos.c`, GPIO, interrupt, and BSP files.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
