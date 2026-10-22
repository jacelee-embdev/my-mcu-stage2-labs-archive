**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 与 APP 示例

## 项目简介

本项目包含 STM32F411CEU6 Bootloader 和独立 APP。Bootloader 基于
STM32F4 标准外设库（SPL），通过 YMODEM 接收加密固件，将密文写入外部
W25Q64 Flash，再解密安装到 MCU 内部 Flash 的 APP 运行区并跳转。
APP 基于 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS。

```text
上电/复位
  → Bootloader（0x08000000）初始化 USART1、SPI1 与 W25Q64
  → PA0 触发升级：YMODEM 接收密文并写入外部 W25Q64
  → 从 W25Q64 读取、解密并写入内部 APP 运行区（0x08008000）
  → 校验 APP 栈顶，切换 VTOR 与 MSP，跳转到 FreeRTOS APP
  → 未触发升级或安装失败：等待 PA0 再次触发
```

## 存储布局

MCU 内部 Flash：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP 运行区 | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| 接收阶段擦除区 | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

加密固件实际从外部 W25Q64 偏移地址 `0x000000` 开始顺序存放。
接收阶段会擦除 MCU 内部 Flash 的 Sector 5；固件密文存放在 W25Q64。

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM 接收、W25Q64 密文暂存、解密安装及 APP 跳转。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，
  作为加密升级和跳转运行的目标固件。

## 编译与运行

1. 使用 Keil 编译 `01_Bootloader`，通过 J-Link/SWD 下载到 MCU，
   并连接 SEGGER RTT Viewer 查看日志。
2. 将 `02_APP` 编译为链接地址 `0x08008000`、明文大小不超过 96 KB
   的 BIN，按 Bootloader 使用的密钥、IV 和包格式生成加密文件。
3. 复位时按住 PA0，在 SecureCRT 中选择
   **Transfer → Send Ymodem** 发送加密文件。
4. 接收完成后，Bootloader 从 W25Q64 读取密文、解密并安装到 APP
   运行区，校验通过后跳转运行。
5. 未触发升级或安装失败时，Bootloader 等待 PA0 再次触发。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，起始地址 0x08000000
│   └── BSP/W25QXX/   外部 W25Q Flash 驱动与缓冲读写接口
└── 02_APP/            HAL + FreeRTOS APP，起始地址 0x08008000
```

## 当前实现说明

- YMODEM 接收端目前不计算或比对数据包末尾的 CRC16。
- `ymodem.c` 接收文件信息包后会擦除 MCU 内部 Flash 的 Sector 5，
  并将密文数据写入外部 W25Q64。
- 外部 Flash 读取依赖 RAM 中的读写索引，固件接收与安装需在同一次
  上电运行中完成。具体流程见
  [Bootloader 工程说明](03_Firmware/01_Bootloader/README_zh.md)。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
