**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# FreeRTOS 中断按键控制 LED

本工程基于 STM32F411CEU6 和 FreeRTOS，通过 GPIO EXTI 中断识别按键短按与长按，并使用队列在任务之间传递事件。

## 功能

- 短按：翻转 LED 状态。
- 长按：LED 闪烁 3 次。

## 消息流程

```text
GPIO EXTI 回调
    ↓ key_press_event_t（边沿 + Tick）
按键内部队列 s_key_edge_queue
    ↓
按键任务 key_task_entry
    ↓ key_event_t（短按 / 长按）
按键结果队列 g_key_queue
    ↓
默认任务（事件转换）
    ↓ led_command_t
LED 命令队列 g_led_queue
    ↓
LED 任务 led_task_entry
```

按键模块只负责生成按键事件，不直接控制 LED。默认任务将按键事件转换为 LED 命令，再交给 LED 任务执行。

## 按键驱动

### 中断采集

按键为上拉输入，按下产生下降沿，释放产生上升沿。EXTI 回调记录边沿类型和 `HAL_GetTick()` 时间戳，并将 `key_press_event_t` 发送到内部队列。

每次中断后，回调会切换下一次触发边沿：

```text
下降沿（按下）→ 等待上升沿
上升沿（释放）→ 等待下降沿
```

### 状态机

按键任务使用 `portMAX_DELAY` 阻塞等待内部事件，并通过两个状态完成处理：

| 当前状态 | 收到边沿 | 处理 | 下一状态 |
|---|---|---|---|
| `KEY_FSM_WAIT_PRESS` | 下降沿 | 保存按下 Tick | `KEY_FSM_WAIT_RELEASE` |
| `KEY_FSM_WAIT_PRESS` | 上升沿 | 忽略 | `KEY_FSM_WAIT_PRESS` |
| `KEY_FSM_WAIT_RELEASE` | 下降沿 | 更新按下 Tick | `KEY_FSM_WAIT_RELEASE` |
| `KEY_FSM_WAIT_RELEASE` | 上升沿 | 计算按压时间并分类 | `KEY_FSM_WAIT_PRESS` |

按压时间为：

```text
press_interval_ms = release_tick - press_first_tick
```

分类规则：

```text
小于 10 ms          → 无效事件
10 ms 至 499 ms     → 短按
大于或等于 500 ms  → 长按
```

## 事件转换

```text
KEY_EVENT_SHORT_PRESS → LED_CMD_TOGGLE
KEY_EVENT_LONG_PRESS  → LED_CMD_BLINK_3_TIMES
```

## 核心文件

- `BSP/key/inc/bsp_key.h`：按键参数、事件和状态机定义。
- `BSP/key/src/bsp_key.c`：EXTI 回调、按键状态机和事件发送。
- `Core/Src/gpio.c`：按键 GPIO、EXTI 和 NVIC 配置。
- `Core/Src/stm32f4xx_it.c`：EXTI0 中断入口。
- `Core/Src/freertos.c`：任务创建和按键事件转换。
- `BSP/led/inc/bsp_led.h`：LED 状态和命令定义。
- `BSP/led/src/bsp_led.c`：LED 命令处理。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
