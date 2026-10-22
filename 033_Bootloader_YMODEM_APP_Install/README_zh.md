**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 与 APP 示例

## 项目简介

本项目包含 STM32F411CEU6 Bootloader 和独立 APP。Bootloader 使用 STM32F4
SPL，通过 YMODEM 接收 APP BIN 固件、写入内部 Flash 并完成程序跳转；APP
使用 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS，用于验证升级后的实际运行状态。

```text
上电/复位
  → Bootloader（0x08000000）等待 YMODEM 传输
  → 擦除 APP 区域并写入接收到的固件
  → 校验 APP 主栈，切换 VTOR 与 MSP
  → APP Reset_Handler（0x08008000）
  → APP 启动 FreeRTOS，线程翻转 LED 并输出日志
```

## Flash 分区

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP | 2–7 | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM 接收、Flash 擦写、回读校验及 APP 跳转。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，作为
  YMODEM 升级和跳转运行的目标固件。

## 编译与运行

1. 编译 `02_APP`，生成链接地址为 `0x08008000` 的 BIN 固件。
2. 编译并通过 J-Link/SWD 下载 `01_Bootloader`。
3. 复位开发板，在 SecureCRT 中选择 **Transfer → Send Ymodem** 发送
   APP BIN 文件。
4. 传输完成后，Bootloader 自动写入并跳转到 APP。
5. 通过 LED 闪烁和 SEGGER RTT 日志确认 FreeRTOS 线程正常运行。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，地址 0x08000000
└── 02_APP/            HAL + FreeRTOS APP，地址 0x08008000
```

## 验证状态

硬件回归测试已经通过：SecureCRT 的 YMODEM 传输完成且无错误，APP 固件
成功写入 Flash，Bootloader 能够跳转到 APP，FreeRTOS 线程、LED 闪烁和
RTT 日志均正常。跳转后若暂时看不到 APP 日志，可重新连接 RTT Viewer。

当前 YMODEM 实现面向固件接收，尚未计算并比对接收数据包末尾的 CRC16。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
