**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 SEGGER RTT 示例

## 项目说明

本工程演示如何在 STM32F411CEU6 上配置并使用 SEGGER Real Time Transfer（RTT）。RTT 源文件已经集成到 Keil 工程中，可通过 J-Link 和 SWD 查看调试信息，无需占用 UART 作为输出通道。

示例同时使用 `SEGGER_RTT_printf()` 和重定向到 RTT 的标准 `printf()`，并为 `SEGGER_RTT_printf()` 增加了基本的浮点数输出支持。

## RTT 配置

- RTT 源文件和配置文件位于 `Middlewares/RTT`。
- `SEGGER_RTT.c` 和 `SEGGER_RTT_printf.c` 已加入 Keil 编译目标。
- `Middlewares/RTT` 已加入编译器头文件搜索路径。
- `Core/Src/usart.c` 中的 `fputc()` 调用 `SEGGER_RTT_PutChar()`，将标准 `printf()` 重定向到 RTT Up Buffer 0。
- `Core/Src/main.c` 周期性输出测试字符串和浮点数。

## 工程结构

```text
Core/Src/main.c                RTT 输出示例
Core/Src/usart.c               printf 重定向
Middlewares/RTT/               SEGGER RTT 源文件及配置
MDK-ARM/STM32F411CEU6.uvprojx  Keil MDK 工程
```

## 运行方法

1. 安装 Keil MDK-ARM 和 SEGGER J-Link Software and Documentation Pack。
2. 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并下载程序。
3. 启动 J-Link RTT Viewer，选择 USB、`STM32F411CE`、SWD，并自动检测 RTT Control Block。
4. 运行或复位目标板，RTT Terminal 0 将显示类似内容：

```text
val:1.12
Test !
val:1.140000
```

以上信息约每秒输出一次。

## 开发环境

| 项目 | 配置 |
|---|---|
| MCU / 主频 | STM32F411CEU6（Cortex-M4F），100 MHz |
| 固件包 | STM32Cube FW_F4 V1.28.3 |
| IDE / 编译器 | Keil MDK 5，ARM Compiler 5 |
| 调试接口 | J-Link，SWD |

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
