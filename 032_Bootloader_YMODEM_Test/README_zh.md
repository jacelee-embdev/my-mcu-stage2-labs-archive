**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 与 APP 示例

## 项目简介

本项目包含 STM32F411CEU6 Bootloader 和独立 APP。Bootloader 使用 STM32F4
SPL，提供 APP 跳转、GPIO、USART、Flash、延时、日志和 YMODEM 通信验证；
APP 使用 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS。

```text
上电/复位
  → Bootloader（0x08000000）
  → 校验 APP 并切换 VTOR 与主栈
  → APP Reset_Handler（0x08008000）
  → APP 启动 FreeRTOS
```

## Flash 分区

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP | 2–7 | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM 通信验证、外设验证及 APP 跳转逻辑。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，按键控制 LED
  并输出日志。

## 编译与下载

1. 使用 Keil 打开相应子工程并编译。
2. Flash Download 选择 **Erase Sectors**。
3. 通过 J-Link/SWD 下载，并使用 SEGGER RTT Viewer 查看日志。

> 当前 Bootloader 为外设验证模式。恢复 APP 跳转前应删除扇区 3 擦写测试，
> 再启用 `bootloader_jump_to_app()`。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，地址 0x08000000
└── 02_APP/            HAL + FreeRTOS APP，地址 0x08008000
```

## 验证状态

已完成 Bootloader 的按键、LED、USART1、SEGGER RTT、Flash 擦写和延时验证；
YMODEM 与 SecureCRT 的文件传输链路、APP 跳转及 FreeRTOS 运行流程也已完成
硬件验证。当前 YMODEM 尚未启用固件 Flash 存储。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
