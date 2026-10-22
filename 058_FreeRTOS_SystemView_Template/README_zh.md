**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 HAL + SEGGER RTT + EasyLogger + FreeRTOS + SystemView

本工程是集成 HAL、SEGGER RTT、EasyLogger、FreeRTOS 和 SEGGER SystemView 的 STM32F411CEU6 模板，提供统一任务创建入口、状态任务和系统跟踪配置，适合需要任务调度、日志输出及任务执行分析的项目。工程保留 CubeMX 配置和 Keil 工程文件。

## 基础配置

- HSI 经 PLL 倍频至 100 MHz；HAL 使用 TIM1 时基，FreeRTOS 节拍频率为 1000 Hz。
- USART1：PA9/TX、PA10/RX，115200 baud、8N1；`printf` 与 EasyLogger 使用 RTT 通道 0。
- SystemView 自动分配名为 `SysView` 的独立 RTT 通道；`FreeRTOSConfig.h` 已引入跟踪适配头文件，并启用 trace facility。

## 快速使用

使用 Keil MDK 5 / ARM Compiler 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并通过 SWD 下载。连接 J-Link，在 RTT Viewer 通道 0 查看日志；使用 SEGGER SystemView 通过 SWD 连接目标，并在上位机启动录制。`SystemView_Init()` 在调度器启动前完成配置，初始化时不会自动开始录制。

## 扩展入口

在 `Z02_Business/Init/os_init.c` 的 `OS_Init()` 中创建任务。状态任务每隔 500 个节拍翻转 PC13 LED 并输出日志；板级驱动位于 `Z01_BSP`，中间件位于 `Middlewares_User`。

跟踪描述和频率配置位于 `Middlewares_User/SystemView/Src/SEGGER_SYSVIEW_Config_FreeRTOS.c`。CubeMX 重新生成代码后，保留 TIM1 时基、`OS_Init()`、日志与 SystemView 初始化、RTT 重定向、跟踪钩子及相关分组和头文件路径；继续禁用默认任务。修改系统时钟时，应核对时间戳配置。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
