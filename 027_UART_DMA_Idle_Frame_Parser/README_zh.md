**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 UART DMA 空闲接收与数据帧解析

## 项目说明

本工程基于 STM32F411CEU6 和 FreeRTOS，实现 USART1 DMA 循环接收、
空闲事件处理和数据帧解析。

主要功能：

- 使用 `HAL_UARTEx_ReceiveToIdle_DMA()` 启动不定长接收。
- 使用 APP 层全局环形缓冲区 `g_uart_rx_buffer` 保存 DMA 数据。
- 统一处理 DMA 半满、全满和 UART 空闲事件。
- 采用 `head_total`、`tail_total` 累计计数，并在访问数组时取余。
- 编译期检查 DMA 缓冲区大小是否为非零的 2 的幂。
- 中断更新队头并通过任务通知唤醒解析线程。
- 状态机校验帧头、长度、载荷、校验和及帧尾。
- 有效数据包通过队列交给应用线程处理。

数据流：

```text
USART1 + Circular DMA
        ↓
HT / TC / IDLE 回调更新 head_total
        ↓
任务通知唤醒 packet_parser_task
        ↓
解析线程更新 tail_total 并解析数据帧
        ↓
有效数据包通过队列交给 app_task
```

## 仓库结构

```text
App/                                  数据帧解析与应用线程
BSP/                                  USART1 DMA 接收驱动
Middlewares_User/circular_buffer/     环形缓冲区中间件
Core/                                 STM32 初始化及中断代码
MDK-ARM/STM32F411CEU6.uvprojx         Keil 工程
STM32F411CEU6.ioc                     STM32CubeMX 工程
```

## 使用指南

### 开发环境

- STM32F411CEU6，系统时钟 100 MHz
- Keil MDK 5，ARM Compiler 5.06
- STM32CubeMX
- FreeRTOS
- USB 转串口、J-Link 和 RTT Viewer

### 串口配置

| USB 转串口 | STM32F411 |
|---|---|
| TX | PA10 / USART1_RX |
| GND | GND |

- 波特率：115200
- 数据格式：8-N-1
- 硬件流控：关闭
- USART1 RX DMA：Circular 模式
- DMA 缓冲区大小：128 字节
- DMA 缓冲区要求：非零且必须为 2 的幂

### 数据帧格式

```text
FF FE | LEN | DATA[LEN] | CHECK | AA
```

- `LEN`：单字节有效载荷长度，实际范围为 1～255。
- `CHECK`：`LEN + DATA` 累加结果的低 8 位。
- 完整帧长度：`LEN + 5` 字节。
- `MAX_PAYLOAD_LEN` 当前为 256，用于预留载荷存储空间。

测试帧：

```text
FF FE 04 01 02 03 04 0E AA
```

### 编译与运行

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`。
2. 编译工程并下载到开发板。
3. 使用串口工具以 HEX 模式发送测试帧。
4. 在 RTT Viewer 中查看解析后的载荷数据。

### 配置与移植

- 串口、引脚和 DMA 配置位于 `STM32F411CEU6.ioc`。
- 缓冲区大小在 `circular_buffer.h` 中通过 `BUFFER_SIZE` 配置，配置为
  0 或非 2 的幂时会触发编译错误。
- 载荷存储空间在 `app.h` 中通过 `MAX_PAYLOAD_LEN` 配置；由于协议
  `LEN` 字段为 `uint8_t`，单帧最多表示 255 字节有效载荷。
- 全局缓冲区 `g_uart_rx_buffer` 可以直接加入 Keil Watch 窗口观察。
- 数据帧字段和解析状态机位于 `app.c`。
- UART DMA 接收和事件回调位于 `bsp_uart_driver.c`。

> 当前方案未实现缓冲区溢出保护。持续高速接收时，解析线程必须及时消费
> 数据，避免 DMA 覆盖尚未处理的内容。

## 作者思考

### 项目关联思考

- 需求分析与业务流向

![01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向](Docs/Images/01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向.png)

源文件：[01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向.drawio](Docs/01_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_需求分析与业务流向.drawio)

- 软件资源架构

![02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构](Docs/Images/02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构.png)

源文件：[02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构.drawio](Docs/02_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_软件资源架构.drawio)

- 程序分析

![03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析](Docs/Images/03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析.png)

源文件：[03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析.drawio](Docs/03_串口私有协议：DMA空闲中断+单环形缓冲接收+本地解包_程序分析.drawio)

### 项目弱关联思考

暂无。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
