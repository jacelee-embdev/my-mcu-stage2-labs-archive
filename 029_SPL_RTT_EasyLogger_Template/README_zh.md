**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 SPL 工程模板

这是一个基于 STM32F4 标准外设库（SPL）的 STM32F411CEU6 基础工程模板，集成了 EasyLogger 和 SEGGER RTT，可作为后续项目的起点。

工程已完成编译和硬件运行验证，能够正常下载并进入 `main()`。

## 开发环境

- Keil MDK-ARM 5.38
- Arm Compiler 5.06 update 7
- Keil STM32F4xx DFP 2.17.1
- STM32F4xx SPL（STSW-STM32065 V1.9.0）

## 工程结构

```text
STM32F411CEU6_SPL_Template
├── BSP
├── Core
│   ├── Inc
│   └── Src
├── Drivers
│   ├── CMSIS
│   └── STM32F4xx_StdPeriph_Driver
├── MDK-ARM
├── Middlewares
├── OBJ
├── SYSTEM
├── Tasks
├── .gitignore
├── LICENSE
├── README.md
└── README_zh.md
```

## 使用方法

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx`。
2. 编译工程。
3. 配置 ST-Link，并将程序下载到目标板。
4. 通过 SEGGER RTT Viewer 查看 EasyLogger 和 RTT 调试输出。

## 日志支持

- EasyLogger 日志组件
- SEGGER RTT 实时调试输出

EasyLogger 已通过端口层连接至 SEGGER RTT Terminal 0。

## 验证状态

- 编译结果：0 Error，0 Warning
- 启动文件：`startup_stm32f411xe.s`
- 目标芯片：STM32F411CEU6
- 已通过 ST-Link 验证程序能够进入 `main()` 并持续运行
- 已完成 EasyLogger 与 SEGGER RTT 的编译和输出链路验证

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
