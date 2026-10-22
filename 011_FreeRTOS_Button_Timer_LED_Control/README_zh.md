**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 FreeRTOS 中断按键与定时器 LED 控制

## 项目简介

本工程面向 STM32F411CEU6，使用 GPIO 外部中断采集两个按键的按下和释放边沿，在 FreeRTOS 任务中完成消抖、短按/长按识别，并通过消息队列解耦按键识别与 LED 控制。

LED 的连续翻转由 TIM2 周期中断驱动，不会在 LED 任务中使用延时函数阻塞等待。

## 功能

| 输入 | 识别结果 | 行为 |
|---|---|---|
| Key1 短按 | `KEY_EVENT_SHORT_PRESS` | 请求 LED 翻转1次 |
| Key1 长按 | `KEY_EVENT_LONG_PRESS` | 请求 LED 翻转20次，即以5 Hz 完成10个亮灭周期 |
| Key2 短按 | `KEY_EVENT_SHORT_PRESS` | USART1 输出短按日志，LED 不变 |
| Key2 长按 | `KEY_EVENT_LONG_PRESS` | USART1 输出长按日志，LED 不变 |
| 消抖区间或按压超时 | `KEY_EVENT_NONE` | 不产生下游事件 |

PC13 LED 为低电平点亮，上电初始化为点亮状态。长按产生偶数次翻转，因此完整执行后会恢复到原来的亮灭状态。

## 硬件与引脚

| 功能 | 引脚 | 配置 |
|---|---|---|
| Key1 | PA0 / EXTI0 | 上拉输入，按下接地 |
| Key2 | PA1 / EXTI1 | 上拉输入，按下接地 |
| LED | PC13 | 推挽输出，低电平点亮 |
| USART1 TX/RX | PA9 / PA10 | 115200，8N1，无流控 |
| SWDIO / SWCLK | PA13 / PA14 | SWD 调试接口 |

USB 转串口模块必须与开发板共地，并使用 3.3 V 逻辑电平。

## 按键判定

按下和释放时间由 `HAL_GetTick()` 在 EXTI 回调中采集，时间基准为1 ms。按压结果在释放后根据持续时间 `Δt` 判定：

| 持续时间 | 判定 |
|---:|---|
| `Δt < 10 ms` | 消抖，无效 |
| `10 ms ≤ Δt < 1000 ms` | 短按 |
| `1000 ms ≤ Δt ≤ 5000 ms` | 长按 |
| `Δt > 5000 ms` | 超时，无效 |

Key1 和 Key2 各自维护独立状态机。EXTI 回调只记录按键编号、边沿和时间戳，识别与日志处理均在任务上下文完成。

## 软件流程

```text
PA0/PA1 EXTI
    → s_key_edge_queue
    → key_task_entry()
        ├─ Key1 → g_key_queue → StartDefaultTask()
        │          → g_led_queue → led_task_entry()
        │          → 待翻转计数 → TIM2 → led_timer_callback() → PC13
        └─ Key2 → USART1 日志
```

三个队列的长度均为10：

| 队列 | 数据 | 用途 |
|---|---|---|
| `s_key_edge_queue` | `key_press_event_t` | 从 EXTI 传递原始边沿事件 |
| `g_key_queue` | `key_event_t` | 传递 Key1 短按/长按结果 |
| `g_led_queue` | `uint8_t` | 传递定时器需要执行的 LED 翻转次数 |

任务优先级为 `led_Task`（High）> `defaultTask`（Normal）> `key_task`（Low）。

## LED 定时控制

TIM2 定时器时钟为100 MHz，预分频系数为10000，计数周期为1000，因此每100 ms产生一次更新中断：

```text
100 MHz ÷ 10000 ÷ 1000 = 10 Hz
```

`led_task_entry()` 接收翻转次数，并在临界区内更新共享计数。`led_timer_callback()` 每次中断最多翻转一次 LED，成功后将剩余次数减一。

- 短按发送 `1`：在下一个定时器周期翻转一次。
- 长按发送 `20`：每100 ms翻转一次，共10个完整亮灭周期，约2秒。
- 新的翻转请求会替换尚未执行完的剩余次数，以最新请求为准。

## 快速开始

1. 按上表连接按键、LED、SWD 和可选串口模块。
2. 使用 Keil µVision 打开 `MDK-ARM/STM32F411CEU6.uvprojx`。
3. 执行 **Rebuild all target files**，通过 ST-Link 下载到目标板。
4. 复位后测试两个按键；如需查看 Key2 日志，打开 115200、8N1、无流控的串口终端。

Key2 的正常日志为：

```text
[I][KEY] Key2 short press.
[I][KEY] Key2 long press.
```

## 主要文件

按键和 LED 模块位于 `BSP/key/`、`BSP/led/`；任务与事件映射位于 `Core/Src/freertos.c`；TIM2 初始化及回调位于 `Core/Src/tim.c`、`Core/Src/main.c`。Keil 工程位于 `MDK-ARM/`。

## 运行注意事项

- 按键结果只在释放后产生，按住期间不会提前上报长按。
- ISR 和业务队列均采用非阻塞发送；队列已满时，当前事件会被丢弃并记录错误日志。
- LED 任务接收新请求后会覆盖当前剩余翻转次数，而不是等待上一组翻转完成。
- `printf` 通过 USART1 阻塞发送，仅建议用于调试。
- 使用 STM32CubeMX 重新生成代码前，应备份并检查 `freertos.c`、GPIO、中断和 BSP 文件的差异。

## 许可

本工程自定义代码采用 [MIT License](LICENSE)。`Drivers/`、`Middlewares/`、STM32 HAL、CMSIS 和 FreeRTOS 仍适用各自的许可证。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
