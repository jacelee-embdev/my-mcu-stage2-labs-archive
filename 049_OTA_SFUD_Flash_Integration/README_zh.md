**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 OTA：SFUD 驱动可移植性优化

## 项目说明

##### 项目简介

本仓库包含 Bootloader、APP1、APP2 三套固件，通过移植 SFUD 降低 OTA 业务与 W25Q 底层驱动的耦合。Bootloader 使用 SPL，应用使用 HAL 和 FreeRTOS。

##### 主要功能

- Bootloader 接收、解密并安装加密固件，管理 EEPROM 升级状态与首次启动确认。
- APP 接收升级包并通过 SFUD 写入外部闪存，复位后由 Bootloader 完成更新。
- APP2 由 APP1 复制，应用线程日志分别为 APP1 / APP2，延时分别为 500 / 200 ms。

##### 仓库结构

```text
03_Firmware/
├── 01_Bootloader/          启动与固件安装
├── 02_APP1/                首次安装及在线升级应用
├── 03_APP2/                升级目标验证应用
└── 04_BIN_EncryptionBIN/   APP1 / APP2 加密包
```

## 工程使用指南

##### 环境与依赖

STM32F411CEU6、W25Q SPI NOR、AT24C02 EEPROM；Keil MDK 5 / ARMCC 5.06 update 7、J-Link / RTT Viewer、支持 YMODEM 的串口工具及配套固件加密工具。

##### 硬件连接与配置

| 外设 | 连接与配置 |
| --- | --- |
| W25Q SPI NOR | PA4：CS；PA5：SCK；PA6：MISO；PA7：MOSI |
| AT24C02 EEPROM | PB8：SCL；PB9：SDA，软件 I²C |
| USART1 | PA9：TX；PA10：RX，115200、8N1 |
| 按键 / LED | PA0：低电平按下；PC13：状态指示灯 |

使用 3.3 V 电平，开发板、外设及 USB 串口模块共地。

##### 编译、运行与观测

1. 按 [Bootloader](03_Firmware/01_Bootloader/README_zh.md)、[APP1](03_Firmware/02_APP1/README_zh.md)、[APP2](03_Firmware/03_APP2/README_zh.md) 的说明依次编译；APP 链接地址为 `0x08008000`。
2. 将 APP 的 BIN 加密为 `.mxxx`，烧录 Bootloader 后通过 YMODEM 首次安装 APP1。
3. 在 APP1 中下载 APP2 加密包，触发更新并复位，通过 RTT 日志确认运行 APP2。

上述完整流程已通过硬件回归和联合测试，使用的是重新构建、重新加密的 APP 固件。

##### 移植说明

三套工程均在 `Middlewares_User/SFUD/` 集成 SFUD。更换平台时适配 `port/sfud_port.c`、`inc/sfud_cfg.h` 和外设初始化，保留本地 W25Q 管理接口；同步核对 64 KiB 分区、4 KiB 擦除单元及 APP 链接地址。

## 作者思考

无

##### 项目关联思考

无

##### 项目弱关联思考

无

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
