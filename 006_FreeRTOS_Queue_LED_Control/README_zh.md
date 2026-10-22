**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS 队列控制 LED

本工程记录使用消息队列连接按键输入与 LED 控制的过程，重点学习线程间通信、模块封装和职责划分。工程基于 STM32F411CEU6、HAL 和 FreeRTOS，将按键与 LED 分别封装为独立 BSP 模块，使输入检测和灯的控制通过消息协作。

## 消息流程

按键线程扫描输入并向按键队列发送状态；CubeMX 默认线程作为中继，接收按键消息并转换为 LED 控制命令，再写入 LED 队列。LED 线程等待命令并执行翻转。通信路径为“按键线程 → 按键队列 → 中继线程 → LED 队列 → LED 线程”。

`Core/Src/freertos.c` 负责线程创建和消息转接，按键与灯的具体操作位于 BSP 驱动中。阅读时可以对照队列句柄、消息类型及发送和接收函数，观察各模块如何在不直接调用对方驱动的情况下完成协作。

## 编译与运行

使用 Keil MDK、ARM Compiler 5 打开 [工程文件](MDK-ARM/STM32F411CEU6.uvprojx)，完整重新编译后下载。按实际硬件连接按键和 LED，观察按键消息引起的灯状态变化，并通过串口日志检查中继线程运行和错误分支。扩展命令时同步修改消息定义、转换逻辑和 LED 处理分支，核对队列创建是否成功及容量是否合适。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
