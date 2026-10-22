**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## 简介

本工程是基于 STM32F4 标准外设库（SPL）的 Bootloader，负责从 Flash 起始地址运行，并跳转到位于 `0x08008000` 的 APP。

## 主要功能

- 从 APP 向量表读取并校验初始主栈指针。
- 从 `0x08008004` 读取 APP 的 `Reset_Handler` 地址。
- 跳转前关闭中断，将 `SCB->VTOR` 切换到 APP 向量表。
- 复位时钟配置、加载 APP 主栈并进入 APP。
- 通过 EasyLogger 和 SEGGER RTT Terminal 0 输出运行日志。
- APP 无效时留在 Bootloader 中并输出诊断日志。

## Flash 分区

| 区域 | 起始地址 | 大小 | 地址范围 |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | `0x8000`（32 KB） | `0x08000000 ~ 0x08007FFF` |
| APP | `0x08008000` | `0x78000`（480 KB） | `0x08008000 ~ 0x0807FFFF` |

## 编译与下载

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx`。
2. 编译并通过 J-Link/SWD 下载 Bootloader。
3. Flash Download 的擦除方式选择 **Erase Sectors**，避免擦除 APP 分区。
4. 再下载 APP，复位后 Bootloader 将自动跳转到 APP。

## 主要目录

```text
Core/                    启动文件和程序入口
Drivers/                 CMSIS 与 STM32F4 SPL
Middlewares/             EasyLogger 与 SEGGER RTT
Tasks/Bootloader/        APP 校验与跳转逻辑
Tasks/Debug/             EasyLogger 初始化
MDK-ARM/                 Keil 工程文件
```

## 验证状态

已完成硬件验证：Bootloader 能够正确切换 APP 向量表和主栈，并成功跳转到 `0x08008000` 运行 APP。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
