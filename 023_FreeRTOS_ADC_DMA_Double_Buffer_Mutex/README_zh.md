**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 ADC DMA 双缓冲与互斥同步实验

## 项目说明

本项目基于 STM32F411CEU6、FreeRTOS 和 STM32 HAL，实现 ADC DMA 双缓冲采集、任务间通信、缓冲区互斥同步、原始数据处理及电压输出。

### 项目简介

ADC1 以 DMA Normal 模式交替向两个动态分配的缓冲区写入采样结果。`adc_sample_task` 负责响应 DMA 完成事件、切换 DMA 目标并发布就绪缓冲区；`adc_process_task` 负责读取就绪缓冲区、将 12 位 ADC 原始值换算为电压，并通过 EasyLogger 和 SEGGER RTT 输出结果。

两个任务通过缓冲区就绪队列和互斥锁完成交接：采样任务在复用缓冲区前，先确认上一条就绪消息已被处理任务取走，再等待处理任务释放互斥锁，从而避免 DMA 写入与数据处理同时访问同一个缓冲区，也避免尚未处理的数据被提前覆盖。

### 主要功能

- 动态分配两个 4 字节缓冲区，交替完成 ADC DMA 采集与数据处理。
- 使用 `dma_done_queue` 将 DMA 完成事件从中断回调传递给采样任务。
- 使用 `adc_ready_queue` 将就绪缓冲区编号从采样任务传递给处理任务。
- 使用 `adc_buffer_mutex` 和队列状态协调缓冲区所有权，防止并发读写及未处理数据被覆盖。
- 将 12 位 ADC 原始值按 3.3 V 参考电压换算为毫伏值，并通过 EasyLogger 和 SEGGER RTT 输出。
- 检查内存分配、队列创建、互斥锁创建、任务创建、DMA 启动、队列通信及 ADC 错误状态。

### 工作流程与同步机制

1. `app_init()` 分配两个 ADC 缓冲区，创建两个队列、一个互斥锁及两个应用任务。
2. `adc_sample_task` 首先启动 DMA，将 ADC1 的转换结果写入 `p_buffer1`。
3. DMA 传输完成后，`HAL_ADC_ConvCpltCallback()` 从中断上下文向 `dma_done_queue` 发送完成事件，并在需要时请求任务切换。
4. `adc_sample_task` 收到完成事件后，等待上一条 `adc_ready_queue` 消息被取走，再获取 `adc_buffer_mutex`，确认处理任务已经完成上一缓冲区的读取、换算和日志输出。
5. 采样任务启动另一个缓冲区的 DMA，释放互斥锁，然后通过 `adc_ready_queue` 发布刚刚完成采集的缓冲区编号。
6. `adc_process_task` 先通过 `xQueuePeek()` 等待就绪消息，再获取互斥锁并通过非阻塞 `xQueueReceive()` 取走消息。处理完成并输出日志后，释放互斥锁。

```text
DMA 完成中断
    │
    └─ dma_done_queue ──> adc_sample_task
                              │
                              ├─ 等待上一条就绪消息被取走
                              ├─ 获取 mutex，等待上一缓冲区处理结束
                              ├─ 切换并启动下一缓冲区的 DMA
                              ├─ 释放 mutex
                              │
                              └─ adc_ready_queue ──> adc_process_task
                                                        │
                                                        ├─ Peek 就绪消息
                                                        ├─ 获取 mutex
                                                        ├─ Receive 并处理缓冲区
                                                        └─ 输出日志并释放 mutex
```

### 仓库结构

```text
.
├── App/                 # ADC 双缓冲、队列通信与互斥同步代码
├── Core/                # CubeMX 生成的初始化和中断代码
├── Docs/                # 项目文档
├── Drivers/             # STM32 HAL 与 CMSIS 驱动
├── Images/              # README 与项目说明图片
├── Middlewares/         # FreeRTOS、EasyLogger 和 SEGGER RTT
├── MDK-ARM/             # Keil MDK 工程
├── References/          # 参考资料
└── STM32F411CEU6.ioc    # STM32CubeMX 配置文件
```

## 工程使用指南

按照下面的环境和接线说明完成准备后，可直接编译、下载并通过 RTT 查看采样结果。

### 环境与依赖

| 项目 | 要求 |
| --- | --- |
| 目标芯片 | STM32F411CEU6 |
| 编译环境 | Keil MDK-ARM V5.32，ARM Compiler 5.06 update 7 |
| 配置工具 | STM32CubeMX 6.16.1（仅修改外设配置时需要） |
| 软件组件 | FreeRTOS 10.3.1、EasyLogger 2.2.99、SEGGER RTT 8.12g（已包含在仓库中） |
| 调试工具 | J-Link 及 SEGGER RTT Viewer |

### 硬件连接与配置

| 连接项 | 说明 |
| --- | --- |
| PA1 / ADC1_IN1 | 接入 0～3.3 V 模拟信号 |
| GND | 与信号源、调试器共地 |
| SWDIO / SWCLK | 连接 J-Link，用于下载和 RTT 日志观测 |

ADC1 使用 12 位分辨率、右对齐和软件触发单次转换。DMA 配置为 DMA2 Stream0、Channel 0、外设到内存、Word 对齐和 Normal 模式；每次传输一个 32 位采样值。输入电压不得超过芯片允许范围。

### 编译、运行与观测

1. 完成模拟信号、共地和调试器连接。
2. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`。
3. 编译工程并将程序下载到目标板，然后复位运行。
4. 打开 SEGGER RTT Viewer，观察两个缓冲区交替输出的 ADC 原始值和电压值。

日志示例：

```text
Buffer1 raw:2048, voltage:1.650 V
Buffer2 raw:2048, voltage:1.650 V
```

### 移植说明

- 根据目标芯片重新配置 ADC 通道、GPIO、DMA 和中断。
- 若 ADC 分辨率或参考电压变化，更新 `ADC_MAX_RAW_VALUE` 和 `VDD_VALUE`。
- 若单次采样数量变化，需同步调整 `ADC_BUFFER_SIZE`、缓冲区处理逻辑和 DMA 传输长度。
- 若改变任务数量或缓冲区交接方式，需重新检查 `adc_ready_queue` 与 `adc_buffer_mutex` 的所有权约束。
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

##### 后续完善:

1. ###### 问题描述

   >目前有 A、B 两个线程，buffer 1 和 buffer 2 两个缓冲区。A 线程负责往缓冲区写入数据，B 线程负责往缓冲区取出数据，即生产消费关系为 A 负责生产，B 负责消费。
   >
   >存在的问题是，可能会出现以下几种情况：
   >
   >1. 同一缓冲区还未被线程 B 消费，就再次被线程 A 进行写入覆盖
   >2. A、B 线程同时对同一缓冲区进行读写操作

2. ###### 解决阶段一:引入互斥量

   >线程 B 处理 buffer 时拿互斥锁，线程 A 想切换 buffer 时也要先尝试拿互斥锁。如果 B 正在使用则拿不到，等 B 用完释放锁，A 才能进行 buffer 切换。

3. ###### 引入互斥量后残留的问题

   >如果此时不仅只有线程 A、B，还有一个线程 C，且 C 的优先级均高于线程 A 和 B，那么可能会出现这种情况：
   >
   >线程 A 将缓冲区已经写入完成的消息通过队列发送至线程 B，希望线程 B 进行调度。但由于线程 C 的优先级更高，线程 C 开始运行，导致线程 B 根本没有得到 CPU 运行时间，也就没有拿到互斥锁。
   >
   >当线程 C 运行结束后，线程 A 重新获得运行机会去拿互斥锁。由于线程 B 根本没有拿过互斥锁，所以线程 A 此时拿锁成功，但其实这个 buffer 并没有被线程 B 处理过，从而出现了线程间同步漏洞。

4. ###### 解决阶段二:线程A 拿锁之前查看"缓冲区写入完成"的队列是否被消费

   >线程 A 在拿锁之前，查看线程 A 向线程 B 发送的“缓冲区写入完成”队列是否被消费。
   >
   >如果已被消费，则证明线程 B 已经对该缓冲区（该 buffer）进行过处理；如果未被消费，则该 buffer 还未被线程 B 处理。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
