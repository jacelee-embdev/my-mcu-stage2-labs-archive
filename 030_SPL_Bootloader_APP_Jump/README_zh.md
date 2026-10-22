**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 跳转 APP 示例

## 项目简介

本项目演示 STM32F411CEU6 从 Bootloader 跳转到独立 APP 的完整流程。Bootloader 使用 STM32F4 SPL，APP 使用 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS；两个工程均通过 EasyLogger 和 SEGGER RTT 输出日志。

启动流程：

```text
上电/复位
  → Bootloader（0x08000000）
  → 校验 APP 主栈指针
  → 关闭中断并切换 VTOR
  → 设置 APP 主栈
  → APP Reset_Handler（0x08008000）
  → APP 重新开启中断并启动 FreeRTOS
```

## Flash 分区

STM32F411CEU6 内部 Flash 共 512 KB：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | `0x8000`（32 KB） | `0x08000000 ~ 0x08007FFF` |
| APP | 2–7 | `0x08008000` | `0x78000`（480 KB） | `0x08008000 ~ 0x0807FFFF` |

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：校验 APP、切换向量表和主栈，并跳转到 APP。
- [02_APP](03_Firmware/02_APP/README_zh.md)：运行 FreeRTOS，周期输出日志，并通过 PA0 按键控制 PC13 LED。

## 编译与下载

1. 分别使用 Keil 打开两个子工程并编译。
2. Flash Download 的擦除方式选择 **Erase Sectors**。
3. 下载 Bootloader，再下载 APP。
4. 复位开发板，通过 SEGGER RTT Viewer Terminal 0 查看日志。
5. 确认 APP 周期输出 `elog TEST!`，按下 PA0 后 PC13 LED 闪烁。

> 不要使用整片擦除方式分别下载两个固件，否则后下载的工程会擦除先前写入的分区。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，地址 0x08000000
└── 02_APP/            HAL + FreeRTOS APP，地址 0x08008000
```

其余顶层目录用于项目管理、参考资料、硬件、软件、机械、生产和工具文件。

## 验证状态

已完成硬件验证：Bootloader 能够成功跳转到 APP；APP 的 FreeRTOS 任务、RTT 日志、PA0 按键和 PC13 LED 均运行正常。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
