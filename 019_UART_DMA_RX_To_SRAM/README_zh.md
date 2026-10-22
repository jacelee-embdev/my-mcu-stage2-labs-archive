**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 USART1 DMA 接收至 SRAM

## 项目说明

本项目演示 USART1 通过 DMA 将串口数据接收到 SRAM 缓冲区，并在普通模式下通过完成回调重新启动接收。

##### 项目简介

项目基于 STM32F411CEU6，目标是掌握 UART、DMA 和接收完成回调之间的协作关系。

##### 主要功能

- USART1 以 115200、8N1 接收串口数据。
- DMA2 Stream2 将 32 字节数据搬运到 SRAM。
- 普通 DMA 模式完成后由回调启动下一次接收。

##### 仓库结构

```text
Core/          应用代码与外设初始化
Drivers/       CMSIS 与 STM32F4 HAL 驱动
MDK-ARM/       Keil MDK 工程
Docs/Images/   README 图片
*.ioc          STM32CubeMX 配置文件
```

## 工程使用指南

完成串口连接并下载程序后，发送固定长度数据，通过调试器查看接收缓冲区。

##### 环境与依赖

- STM32F411CEU6 开发板、USB 转串口模块和下载器。
- STM32CubeMX 6.16.1、STM32CubeF4 1.28.3、Keil MDK-ARM 5.32。
- 任意支持 115200、8N1 的串口工具。

##### 硬件连接与配置

| STM32F411CEU6 | USB 转串口 |
| --- | --- |
| PA10 / USART1_RX | TX |
| PA9 / USART1_TX | RX（可选） |
| GND | GND |

串口配置为 115200、8 数据位、1 停止位、无校验、无流控，并确保电平兼容。

##### 编译、运行与观测

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并下载。
2. 串口发送 32 字节，例如 `ABCDEFGHIJKLMNOPQRSTUVWXYZ123456`。
3. 在调试器中查看 `recv_data_buf`，确认数据已写入 SRAM。

   <img src="./Docs/Images/recv_data_buf.png" style="zoom:25%;" />

##### 移植说明

更换芯片或串口时，需要重新配置 UART 引脚、DMA Stream/Channel、中断入口及接收缓冲区长度，并核对目标芯片的数据手册。

## 作者思考

##### 项目关联思考

1. 下图中,`XferCpltCallback` 等是什么？

   ![](./Docs/Images/XferCpltCallback.png)

   >这些是 DMA 句柄中的函数指针，用于指定 DMA 发生不同事件时调用哪个函数。
   >
   >比如说：
   >• XferCpltCallback：表示全部完成传输时调用
   >• XferHalfCpltCallback：传输一半时调用
   >•XferErrorCallback：传输错误时调用
   >• XferAbortCallback：传输中止时调用.当前设为空指针，也就是没有这个函数。

2. 为什么 DMA 回调调用的**却是** UART 函数？

   >DMA 搬运的是 UART 接收的数据。DMA 接收完成后，需要调用 UART 内部的函数更新串口的状态，然后通知用户层面数据接收完成。

3. DMA 为什么不靠用户回调搬运数据？

   >数据由 DMA 硬件自动从 UART 数据寄存器搬到目标地址的 SRAM，用户回调不参与搬运。HAL_UART_RxCpltCallback 只负责通知用户：“数据已经接收完成”。
   >
   >- 普通模式：通常在回调中重新启动 DMA。
   >- 循环模式：DMA 自动循环，不需要重新启动。

4. 完整调用链路

   ```c
   初始化 DMA 和 UART
       ↓
   HAL_UART_Receive_DMA()
       ↓
   UART_Start_Receive_DMA()
   注册 DMA 回调并启动 DMA
       ↓
   DMA 自动搬运 32 字节
       ↓
   DMA2_Stream2_IRQHandler()
       ↓
   HAL_DMA_IRQHandler()
       ↓
   XferCpltCallback()
       ↓
   UART_DMAReceiveCplt()
       ↓
   HAL_UART_RxCpltCallback()
       ↓
   普通模式下重新启动 DMA
   ```

##### 项目弱关联思考

暂无。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
