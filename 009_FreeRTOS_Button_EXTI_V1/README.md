**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# FreeRTOS Interrupt-Driven Button LED Control

This STM32F411CEU6 and FreeRTOS project uses GPIO EXTI interrupts to identify short and long button presses and passes events between threads through queues.

## Behavior

- Short press: toggle the LED.
- Long press: blink the LED three times.

## Message Flow

```text
GPIO EXTI callback
  -> key_press_event_t (edge + tick)
  -> Internal edge queue s_key_edge_queue
  -> Button thread key_task_entry
  -> key_event_t (short / long press)
  -> Result queue g_key_queue
  -> Default thread (event conversion)
  -> led_command_t
  -> LED command queue g_led_queue
  -> LED thread led_task_entry
```

The button module produces events. The default thread converts them to LED commands, and the LED thread executes them.

## Button Driver

### Edge Capture

The pull-up input produces a falling edge on press and a rising edge on release. The EXTI callback records the edge and `HAL_GetTick()` timestamp, then sends a `key_press_event_t` to the internal queue. After each interrupt it switches the next trigger edge:

```text
Falling edge (press) -> Wait for rising edge
Rising edge (release) -> Wait for falling edge
```

### State Machine

The button thread blocks on its internal queue with `portMAX_DELAY`.

| State | Edge | Action | Next state |
|---|---|---|---|
| `KEY_FSM_WAIT_PRESS` | Falling | Save press tick | `KEY_FSM_WAIT_RELEASE` |
| `KEY_FSM_WAIT_PRESS` | Rising | Ignore | `KEY_FSM_WAIT_PRESS` |
| `KEY_FSM_WAIT_RELEASE` | Falling | Update press tick | `KEY_FSM_WAIT_RELEASE` |
| `KEY_FSM_WAIT_RELEASE` | Rising | Calculate and classify duration | `KEY_FSM_WAIT_PRESS` |

The duration is `press_interval_ms = release_tick - press_first_tick`. Intervals below 10 ms are invalid; 10–499 ms is a short press; 500 ms or more is a long press.

## Event Conversion

```text
KEY_EVENT_SHORT_PRESS -> LED_CMD_TOGGLE
KEY_EVENT_LONG_PRESS -> LED_CMD_BLINK_3_TIMES
```

## Core Files

- `BSP/key/inc/bsp_key.h`: parameters, events, and states.
- `BSP/key/src/bsp_key.c`: EXTI callback, state machine, and event sending.
- `Core/Src/gpio.c`: GPIO, EXTI, and NVIC configuration.
- `Core/Src/stm32f4xx_it.c`: EXTI0 entry point.
- `Core/Src/freertos.c`: thread creation and event conversion.
- `BSP/led/inc/bsp_led.h`: LED states and commands.
- `BSP/led/src/bsp_led.c`: command handling.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
