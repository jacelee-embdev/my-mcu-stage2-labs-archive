**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Bootloader：SFUD 驱动移植与 OTA 升级

## 项目说明

##### 项目简介

基于 STM32F411CEU6 和 SPL 的裸机 Bootloader，通过 SFUD 访问 W25Q 外部闪存，完成加密固件的接收、解密、安装与启动。

##### 主要功能

- USART1 / YMODEM 接收加密固件，使用外部 A/B 分区暂存与解密。
- 安装到内部 Flash，结合 EEPROM 状态和看门狗完成首次启动确认。
- W25Q 管理接口接入 SFUD；安装失败时支持恢复已备份的旧 APP。

##### 仓库结构

```text
Core/                    启动与 SPL 外设接口
BSP/W25QXX/              外部闪存管理接口
Middlewares_User/SFUD/   SFUD 库、移植层及适配层
Tasks/Bootloader/        OTA 状态机与固件安装
MDK-ARM/                Keil 工程
```

## 工程使用指南

##### 环境与依赖

STM32F411CEU6、W25Q、AT24C02；Keil MDK 5 / ARMCC 5.06 update 7、J-Link、RTT Viewer，以及支持 YMODEM 的串口工具和配套固件加密工具。

##### 硬件连接与配置

| 外设 | 连接与配置 |
| --- | --- |
| W25Q SPI NOR | PA4：CS；PA5：SCK；PA6：MISO；PA7：MOSI |
| AT24C02 EEPROM | PB8：SCL；PB9：SDA，软件 I²C |
| USART1 | PA9：TX；PA10：RX，115200、8N1 |
| 按键 / LED | PA0：低电平按下；PC13：状态指示灯 |

使用 3.3 V 电平，开发板、外设及 USB 串口模块共地。

##### 编译、运行与观测

1. 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx`，编译并通过 J-Link 烧录 Bootloader。
2. APP 按 `0x08008000` 链接，生成 BIN 后加密为 `.mxxx`；按 Bootloader 提示按键进入下载，通过 YMODEM 发送 APP1 包。
3. 使用 RTT Viewer 观察安装和 APP1 启动，再由 APP1 下载 APP2 并触发更新。

已完成硬件回归及联合测试：Bootloader 首次安装 APP1 → APP1 接收 APP2 → 复位后运行 APP2。

##### 移植说明

硬件适配集中在 `Middlewares_User/SFUD/port/sfud_port.c`，设备配置在 `inc/sfud_cfg.h`，业务接口在 `BSP/W25QXX/w25qxx_Handler.c`。外部 A/B 分区各 64 KiB，管理层使用 4 KiB 擦除单元；更换平台时同步核对 SPI、延时、分区和 APP 链接地址。

## 作者思考

无

##### 项目关联思考

无

##### 项目弱关联思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
