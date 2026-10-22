**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 RTT 与 EasyLogger 开发模板

本工程基于 STM32F411CEU6，已完成 SEGGER RTT 和 EasyLogger 移植，可作为后续嵌入式项目的开发基线。复用现有外设初始化、调试输出和日志框架后，即可添加业务功能。当前采用裸机运行方式，尚未接入 FreeRTOS，HAL 时基使用 SysTick。

## 核心功能

- 标准 `printf` 重定向至 RTT 通道 0，EasyLogger 共用该输出通道。
- 支持多等级、彩色日志，包含运行时间、模块标签、源码路径、行号和函数名。
- 使用 `debug_init()` 统一初始化日志模块，当前采用同步输出。

## 使用方式

1. 使用 Keil MDK-ARM / ARMCC 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并通过 J-Link 下载。
2. 打开 J-Link RTT Viewer，选择目标芯片、SWD 接口及通道 0，运行或复位目标板。
3. 主循环约每两秒输出一次 `printf`、`SEGGER_RTT_printf` 和 EasyLogger 测试信息。

## 后续开发

业务代码从 `Core/Src/main.c` 扩展；日志初始化及格式在 `Middlewares_User/EasyLogger/debug/debug.c` 中维护，日志选项在 `Middlewares_User/EasyLogger/inc/elog_cfg.h` 中配置，`printf` 重定向位于 `Core/Src/usart.c`。

后续接入 FreeRTOS 时，先在 CubeMX 的 `System Core → SYS → Timebase Source` 中将 HAL 时基由 SysTick 改为其他定时器（如 TIM1），再启用 FreeRTOS 并生成代码。随后按任务模型调整业务逻辑与日志同步机制，并检查已有移植代码是否保留。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
