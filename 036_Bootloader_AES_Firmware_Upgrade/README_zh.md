**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 与 APP 示例

## 项目简介

本项目包含 STM32F411CEU6 Bootloader 和独立 APP。Bootloader 使用 STM32F4
SPL，通过 YMODEM 将加密固件下载到 B 备份区，解密安装到 A 运行区并
完成程序跳转；APP 使用 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS。

```text
上电/复位
  → Bootloader（0x08000000）检测升级按键
  → 通过 YMODEM 将加密固件写入 APP Backup
  → 解密头部，校验大小，逐块解密写入 APP Run 并回读校验
  → 校验 APP 初始栈顶，切换 VTOR 与 MSP，跳转到 APP
  → APP 启动 FreeRTOS，输出日志；按键可触发 LED 翻转
```

## Flash 分区

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、YMODEM 接收、加密固件解密安装、回读校验及 APP 跳转。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，作为加密升级和跳转运行的目标固件。

## 编译与运行

1. 编译 `02_APP`，生成链接地址为 `0x08008000`、最大 96 KB 的 BIN 固件。
2. 按 Bootloader 使用的密钥、IV 和包格式将 APP BIN 加密。
3. 编译并通过 J-Link/SWD 下载 `01_Bootloader`。
4. 复位开发板并按下 PA0 升级键，在 SecureCRT 中选择
   **Transfer → Send Ymodem** 发送加密文件。
5. Bootloader 将密文写入 B 区，解密安装到 A 区并跳转运行。
6. 可通过 PC13 LED 和 SEGGER RTT 日志观察 APP 运行情况。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，地址 0x08000000
└── 02_APP/            HAL + FreeRTOS APP，地址 0x08008000
```

## 协议说明

当前 YMODEM 实现面向固件接收，尚未计算并比对接收数据包末尾的 CRC16。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
