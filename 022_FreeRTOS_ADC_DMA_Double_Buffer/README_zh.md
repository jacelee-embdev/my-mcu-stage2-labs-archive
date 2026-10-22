**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 ADC DMA 双缓冲采集实验

## 项目说明

本项目基于 STM32F411CEU6、FreeRTOS 和 STM32 HAL，实现 ADC DMA 双缓冲采集、任务间通信、原始数据处理及电压输出。

##### 项目简介

ADC1 通过 DMA 将采样结果写入两个动态缓冲区。采样线程负责切换 DMA 目标，处理线程负责将 12 位 ADC 原始值换算为电压，并通过 EasyLogger 输出结果。

##### 主要功能

- 使用两个 4 字节缓冲区实现 ADC DMA 交替采集。
- 使用两个 FreeRTOS 队列连接 DMA 中断、采样线程和处理线程。
- 将 12 位 ADC 原始值换算为电压，并通过 SEGGER RTT 输出日志。
- 检查内存、队列、线程、DMA 和 ADC 错误状态。

##### 仓库结构

```text
.
├── App/                 # ADC 双缓冲应用代码
├── Core/                # CubeMX 生成的初始化和中断代码
├── Drivers/             # STM32 HAL 与 CMSIS 驱动
├── Middlewares/         # FreeRTOS、EasyLogger 和 SEGGER RTT
├── MDK-ARM/             # Keil MDK 工程
└── STM32F411CEU6.ioc    # STM32CubeMX 配置文件
```

## 工程使用指南

按照下面的环境和接线说明完成准备后，可直接编译、下载并通过 RTT 查看采样结果。

##### 环境与依赖

| 项目 | 要求 |
| --- | --- |
| 目标芯片 | STM32F411CEU6 |
| 编译环境 | Keil MDK-ARM V5.32，ARM Compiler 5.06 update 7 |
| 配置工具 | STM32CubeMX 6.16.1（仅修改外设配置时需要） |
| 软件组件 | FreeRTOS 10.3.1、EasyLogger 2.2.99、SEGGER RTT 8.12g（已包含在仓库中） |
| 调试工具 | J-Link 及 SEGGER RTT Viewer |

##### 硬件连接与配置

| 连接项 | 说明 |
| --- | --- |
| PA1 / ADC1_IN1 | 接入 0～3.3 V 模拟信号 |
| GND | 与信号源、调试器共地 |
| SWDIO / SWCLK | 连接 J-Link，用于下载和 RTT 日志观测 |

ADC1 使用 12 位分辨率和右对齐格式；DMA 配置为 DMA2 Stream0、Channel 0、外设到内存、Word 对齐和 Normal 模式。输入电压不得超过芯片允许范围。

##### 编译、运行与观测

1. 完成模拟信号、共地和调试器连接。
2. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`。
3. 编译工程并将程序下载到目标板，然后复位运行。
4. 打开 SEGGER RTT Viewer，观察两个缓冲区的 ADC 原始值和电压值。

日志示例：

```text
Buffer1 raw:2048, voltage:1.650 V
Buffer2 raw:2048, voltage:1.650 V
```

##### 移植说明

- 根据目标芯片重新配置 ADC 通道、GPIO、DMA 和中断。
- 若 ADC 分辨率或参考电压变化，更新 `ADC_MAX_RAW_VALUE` 和 `VDD_VALUE`。
- 若采样数量变化，调整缓冲区大小、DMA 长度和队列容量。
- 若不使用 SEGGER RTT，需要重新实现 EasyLogger 输出端口。

## 作者思考

### 实现记录

1. `malloc` 分配两个 4 字节 ADC 缓冲区，分别由 `p_buffer1` 和 `p_buffer2` 指向；状态变量和句柄记录缓冲区、线程及队列。
2. `dma_done_queue` 连接 DMA 中断与采样线程，`adc_ready_queue` 连接采样线程与处理线程。
3. `adc_sample_task` 控制 DMA 双缓冲切换，`adc_process_task` 完成采样数据处理。
4. DMA 完成回调通过 `xQueueSendFromISR()` 发送完成消息，并在需要时触发线程切换。
5. 采样从第一个缓冲区开始，完成后交替使用另一个缓冲区，并将刚完成采集的缓冲区通知处理线程。
6. 处理线程读取完成的缓冲区，按 3.3 V 参考电压换算 12 位 ADC 原始值，并通过 EasyLogger 输出结果。
7. 程序检查内存、队列、线程、DMA 启动及通信结果，并在 ADC 错误回调中处理溢出和 DMA 错误。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
