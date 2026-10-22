**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# FreeRTOS Button LED Control and Software Interrupt Test

This STM32F411CEU6 and FreeRTOS project supports short-press LED toggling, long-press blinking three times, and a software interrupt test. NVIC pends USART1 IRQ in software, while test logs are printed from thread context.

Button and LED threads communicate through queues and the default relay thread:

```text
Button thread -> Button event queue -> Default thread -> LED command queue -> LED thread
```

## Software Interrupt Test

The button thread calls `NVIC_SetPendingIRQ(USART1_IRQn)` to pend the interrupt. The CPU enters `USART1_IRQHandler()`, which only increments a software interrupt counter. Back in thread context, the button thread reads the counter and prints `Software interrupt test!` through USART1.

```text
Button thread -> Pend USART1 IRQ -> USART1_IRQHandler
              -> Increment counter -> Return to button thread -> Print log
```

The ISR does not call `printf()` directly, avoiding indirect use of thread-only FreeRTOS APIs from interrupt context.

## Core Files

- `BSP/key/inc/bsp_key.h`: states, events, and button interfaces.
- `BSP/key/src/bsp_key.c`: detection, event sending, software interrupt triggering, and logs.
- `Core/Src/freertos.c`: thread creation and event-to-command conversion.
- `Core/Src/stm32f4xx_it.c`: USART1 interrupt handling and interrupt count.
- `Core/Src/usart.c`: USART1 initialization and print retargeting.
- `BSP/led/inc/bsp_led.h`: LED states, commands, and interfaces.
- `BSP/led/src/bsp_led.c`: command processing and queue reception.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
