**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS APP

## 简介

本工程是由 Bootloader 跳转运行的 STM32F411CEU6 应用程序，基于 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS，链接地址为 `0x08008000`。

## 主要功能

- 进入 `main()` 后重新开启 Bootloader 关闭的全局中断。
- 初始化 HAL、100 MHz 系统时钟、GPIO、USART1 和 FreeRTOS。
- 默认任务每 100 ms 输出一条 EasyLogger 测试日志。
- PA0 按键为低电平时翻转 PC13 LED，用于验证 GPIO 和任务运行。
- EasyLogger 将整段日志直接写入 SEGGER RTT Up Buffer 0；日志包含时间、等级、标签、文件、行号和函数名。

## Flash 分区

| 区域 | 起始地址 | 大小 | 地址范围 |
|---|---:|---:|---:|
| APP | `0x08008000` | `0x78000`（480 KB） | `0x08008000 ~ 0x0807FFFF` |

APP 的向量表位于 `0x08008000`。Bootloader 跳转前将 `SCB->VTOR` 指向该地址，APP 在启动时重新开启中断。

## 编译与下载

1. 先下载配套 Bootloader。
2. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`。
3. 编译并通过 J-Link/SWD 下载 APP，擦除方式选择 **Erase Sectors**。
4. 复位开发板，在 SEGGER RTT Viewer 的 Terminal 0 查看日志。
5. 按下 PA0 按键，确认 PC13 LED 能够闪烁。

## 主要目录

```text
Core/                    HAL、FreeRTOS、GPIO、USART 和应用入口
Tasks/Debug/             EasyLogger 初始化与格式配置
Middlewares_User/        EasyLogger 与 SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil 工程文件
```

## 验证状态

已完成硬件验证：Bootloader 能够成功跳转到 APP，RTT 日志持续输出，按键和 LED 功能正常。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
