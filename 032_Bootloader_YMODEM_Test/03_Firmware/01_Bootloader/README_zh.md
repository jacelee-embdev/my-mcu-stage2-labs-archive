**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## 简介

本工程是基于 STM32F4 标准外设库（SPL）的 Bootloader，运行于
`0x08000000`，提供 APP 跳转框架、基础外设及 YMODEM 通信验证功能。

## 主要功能

- 校验并跳转到位于 `0x08008000` 的 APP。
- PA0 按键输入与 PC13 LED 控制。
- USART1（PA9/PA10，115200）轮询收发。
- 通过 YMODEM 与 SecureCRT 进行固件文件传输验证。
- 内部 Flash 扇区擦除与 32 位字写入。
- 基于 DWT 和 SysTick 的微秒、毫秒及秒级延时。
- 通过 EasyLogger 和 SEGGER RTT 输出日志。

## Flash 分区

| 区域 | 起始地址 | 大小 | 地址范围 |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## 编译与下载

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx`。
2. 编译并通过 J-Link/SWD 下载。
3. Flash Download 选择 **Erase Sectors**。

## 主要目录

```text
Core/                    程序入口及 GPIO、Flash、USART
SYSTEM/delay/            DWT 与 SysTick 延时模块
Drivers/                 CMSIS 与 STM32F4 SPL
Middlewares/             YMODEM、EasyLogger 与 SEGGER RTT
Tasks/Bootloader/        APP 校验与跳转逻辑
Tasks/Debug/             EasyLogger 初始化
MDK-ARM/                 Keil 工程文件
```

## 验证状态

已完成硬件验证：按键与 LED、USART1 的 `A/B` 输出、SEGGER RTT 日志，
Flash 擦写以及 YMODEM 与 SecureCRT 的通信链路均工作正常。

当前 `main.c` 为外设验证模式，APP 跳转暂时关闭。正式启用跳转前，应删除
扇区 3 擦写测试，避免破坏 APP 数据。

当前 YMODEM 仅验证握手、数据包接收和传输结束流程，固件数据的 Flash
擦除与写入尚未启用，因此暂不构成完整的固件升级功能。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
