**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Keil 调试基础

本工程记录使用 Keil 调试 STM32 基础程序的过程。工程由 STM32CubeMX 生成，包含 GPIO、USART1 和 HAL 初始化代码，可以用一个简单且可重复的串口输出循环观察程序执行、断点停留及外设状态。

## 当前程序

`Core/Src/main.c` 完成初始化后，每隔约两秒调用 `printf` 输出 `Hello World!`，延时通过 `HAL_Delay(2000)` 实现。有效的打印重定向位于 `Core/Src/usart.c`，使用 USART1 发送字符，串口参数为 115200、8N1。

源码保留了通过 `ITM_SendChar()` 重定向打印的注释示例，目前运行路径使用串口。观察当前程序时使用串口终端；研究 ITM 时需要结合调试器、跟踪时钟和输出配置单独验证。

## 编译与调试

使用 Keil MDK、ARM Compiler 5 打开 [工程文件](MDK-ARM/STM32F411CEU6.uvprojx)，完整重新编译并下载。连接工程配置的 SWD 调试器，在主循环和重定向函数处设置断点，结合单步执行、变量窗口和寄存器窗口观察调用过程。串口模块接收端连接 USART1 的 TX 并与开发板共地。暂停在断点时，周期输出也会暂停；恢复运行后再检查日志节奏。调试前核对所选芯片、下载算法和调试器连接状态。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
