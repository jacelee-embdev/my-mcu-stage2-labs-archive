**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# MCU 嵌入式学习第二阶段工程归档

本仓库归档了我在嵌入式学习第二阶段的一些工程，涵盖 FreeRTOS、DMA、Bootloader、OTA 升级、嵌入式调试等内容。

## 归档说明

本仓库保存嵌入式学习第二阶段积累的工程和笔记，用于个人学习记录、代码备份，以及后续回顾和复现实验。

学习内容包括任务调度与同步、数据传输与缓冲、固件升级、加密、故障诊断、链接脚本等。工程按学习顺序编号，保留对应学习阶段的实现方式和实验状态。

各实验分别打开和编译，部分实验包含 Bootloader 与 APP 等相互配合的固件工程。具体硬件、开发环境、编译顺序和预期现象，请以对应实验的说明为准。

## 工程索引

单击工程名称可查看对应的中文说明。

| 编号 | 工程 | 实现内容 |
|---:|---|---|
| 001 | [Button_LED_CubeMX_V1](001_Button_LED_CubeMX_V1/README_zh.md) | 在 CubeMX 工程中轮询按键并翻转 LED，串口周期输出测试信息。 |
| 002 | [Button_LED_Template_V2](002_Button_LED_Template_V2/README_zh.md) | 基于自建 HAL 模板封装按键和 LED 驱动，通过主循环轮询控制 LED 翻转。 |
| 003 | [FreeRTOS_UART_Printf](003_FreeRTOS_UART_Printf/README_zh.md) | FreeRTOS 默认线程轮询按键、翻转 LED，并通过 UART 输出测试信息。 |
| 004 | [FreeRTOS_Printf_Thread_Safety](004_FreeRTOS_Printf_Thread_Safety/README_zh.md) | 通过多个线程并发打印，观察字符级调度保护与整行输出的差异。 |
| 005 | [Keil_Debug_Basics](005_Keil_Debug_Basics/README_zh.md) | 使用周期串口输出练习 Keil 断点、单步及变量观察。 |
| 006 | [FreeRTOS_Queue_LED_Control](006_FreeRTOS_Queue_LED_Control/README_zh.md) | 通过按键队列、中继线程和 LED 队列解耦按键与灯控制。 |
| 007 | [FreeRTOS_Button_Short_Long_Press](007_FreeRTOS_Button_Short_Long_Press/README_zh.md) | 通过 FreeRTOS 线程和队列实现短按翻转、长按闪烁。 |
| 008 | [FreeRTOS_Software_Interrupt](008_FreeRTOS_Software_Interrupt/README_zh.md) | 通过 NVIC 软件挂起中断，并在线程上下文输出测试日志。 |
| 009 | [FreeRTOS_Button_EXTI_V1](009_FreeRTOS_Button_EXTI_V1/README_zh.md) | 使用 EXTI 边沿事件与线程状态机识别单个按键的短按、长按。 |
| 010 | [FreeRTOS_Button_EXTI_V2](010_FreeRTOS_Button_EXTI_V2/README_zh.md) | 为两个按键维护独立 EXTI 状态机，并通过队列控制 LED。 |
| 011 | [FreeRTOS_Button_Timer_LED_Control](011_FreeRTOS_Button_Timer_LED_Control/README_zh.md) | 将按键事件映射为 LED 命令，并通过 TIM2 中断驱动翻转。 |
| 012 | [FreeRTOS_Interrupt_Response_Comparison](012_FreeRTOS_Interrupt_Response_Comparison/README_zh.md) | 比较队列按值复制与传递指针对中断响应路径的影响。 |
| 013 | [SEGGER_RTT_Demo](013_SEGGER_RTT_Demo/README_zh.md) | 移植 SEGGER RTT，使用 RTT 接口和标准 printf 输出调试信息。 |
| 014 | [EasyLogger_UART_Port](014_EasyLogger_UART_Port/README_zh.md) | 移植 EasyLogger，通过 UART 输出分级彩色日志。 |
| 015 | [RTT_EasyLogger_Template_V1](015_RTT_EasyLogger_Template_V1/README_zh.md) | 组合 RTT 和 EasyLogger，形成裸机调试输出模板。 |
| 016 | [FreeRTOS_RTT_EasyLogger_Template_V2](016_FreeRTOS_RTT_EasyLogger_Template_V2/README_zh.md) | 集成 FreeRTOS、RTT 和 EasyLogger，统一线程与日志入口。 |

## 使用说明

1. 克隆或下载本仓库，根据工程索引选择实验。
2. 阅读对应实验的 `README_zh.md`，核对 MCU、硬件接线、所需组件和开发环境。
3. 使用 Keil MDK 打开相应的 `.uvprojx` 文件，并安装该工程所需的编译器和芯片支持包。
4. 完整重新编译工程，生成固件。涉及多个配套固件的实验，按其说明中的顺序编译和烧录。
5. 使用工程配置的调试器烧录固件，检查串口、LED 或其他说明中列出的实验现象。

可重新生成的编译产物不纳入版本管理。单独使用某个实验时，请一并保留其源代码、工程配置、`LICENSE` 和 `.gitignore`。不同实验的硬件和工具链配置可能有所差异，请结合对应说明与源代码进行核对。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
