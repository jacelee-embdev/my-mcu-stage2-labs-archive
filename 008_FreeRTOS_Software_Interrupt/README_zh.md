**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# FreeRTOS 按键控制 LED 与软件中断测试

## 功能说明

本工程基于 STM32F411CEU6 和 FreeRTOS，实现以下功能：

- 按键短按：LED 状态翻转。
- 按键长按：LED 闪烁 3 次。
- 软件中断测试：通过 NVIC 主动挂起 USART1 IRQ，并在任务上下文打印测试日志。

按键任务、默认任务和 LED 任务通过 FreeRTOS 队列传递事件与命令：

```text
按键任务 -> 按键事件队列 -> 默认任务 -> LED 命令队列 -> LED 任务
```

## 软件中断测试

按键任务调用 `NVIC_SetPendingIRQ(USART1_IRQn)`，以软件方式将 USART1 IRQ
设置为 Pending。CPU 随后进入 `USART1_IRQHandler()`，中断服务函数只增加
软件中断计数；返回任务上下文后，按键任务读取计数并通过 USART1 打印
`Software interrupt test!`。

```text
按键任务 -> 设置 USART1 IRQ Pending -> USART1_IRQHandler
        -> 软件中断计数加 1 -> 返回按键任务 -> 打印测试日志
```

中断服务函数不直接调用 `printf()`，避免在 ISR 中间接调用非 ISR 安全的
FreeRTOS 任务级 API。

## 核心文件

- `BSP/key/inc/bsp_key.h`：定义按键状态、按键事件及相关接口。
- `BSP/key/src/bsp_key.c`：实现按键检测、事件发送、软件中断触发及测试日志输出。
- `Core/Src/freertos.c`：创建任务，并完成按键事件到 LED 命令的映射与转发。
- `Core/Src/stm32f4xx_it.c`：实现 USART1 中断处理，并记录软件中断次数。
- `Core/Src/usart.c`：初始化 USART1，并将 `printf()` 重定向到串口输出。
- `BSP/led/inc/bsp_led.h`：定义 LED 状态、控制命令及相关接口。
- `BSP/led/src/bsp_led.c`：实现 LED 控制、命令处理及队列接收。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
