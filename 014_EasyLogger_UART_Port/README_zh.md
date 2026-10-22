**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 EasyLogger 移植示例

## 项目简介

本工程演示如何在 STM32F411CEU6 裸机项目中移植并使用 EasyLogger。工程由 STM32CubeMX 生成，使用 Keil MDK 和 ARM Compiler 5 编译，通过 USART1 将不同等级的彩色日志发送到 PC 串口终端。示例能够输出日志等级、模块标签、系统运行时间、源文件、行号和函数名，便于观察程序运行状态和定位问题。

## EasyLogger 配置

- EasyLogger 2.2.99 源码位于 `Middlewares/easylogger`，核心源码和 STM32 移植接口已经加入 Keil 工程。
- `Middlewares/easylogger/inc` 已加入编译器头文件搜索路径。
- `elog_port_output()` 使用 `printf()` 输出日志，标准 `printf()` 通过 `fputc()` 重定向到 USART1。
- `elog_port_get_time()` 使用 `HAL_GetTick()` 获取启动后的毫秒计数，并转换为“秒.毫秒”格式。
- 日志颜色以及函数名、文件路径和行号格式已经开启，换行符设置为 `\r\n`。
- 当前工程采用同步即时输出，异步输出和缓冲输出功能均未开启。

## 工程结构

```text
Core/Src/main.c                  EasyLogger 初始化及日志测试
Core/Src/usart.c                 USART1 配置和 printf 重定向
Middlewares/easylogger/inc/      EasyLogger 头文件和配置文件
Middlewares/easylogger/src/      EasyLogger 核心源码
Middlewares/easylogger/port/     STM32 平台移植接口
MDK-ARM/STM32F411CEU6.uvprojx    Keil MDK 工程文件
```

## 运行方法

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并下载程序。
2. 运行或复位开发板，打开支持 ANSI 颜色的串口终端，例如 MobaXterm。
3. 选择开发板对应的串口，设置波特率为 115200。
4. 程序每隔约 2 秒输出 Assert、Error、Warn、Info、Debug 和 Verbose 六个等级的测试日志。

示例输出形式如下：

```text
I/main            [0.012] (../Core/Src/main.c:95 test_log)Hello EasyLogger!
```

## 开发环境

| 项目 | 配置 |
|---|---|
| MCU / 主频 | STM32F411CEU6（Cortex-M4F），100 MHz |
| 日志组件 | EasyLogger 2.2.99 |
| IDE / 编译器 | Keil MDK 5，ARM Compiler 5 |
| 串口 | USART1，PA9/PA10，115200 bps |
| 下载调试 | J-Link，SWD |

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
