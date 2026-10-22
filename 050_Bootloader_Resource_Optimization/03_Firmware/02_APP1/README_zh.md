**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 APP1：SFUD 驱动移植与 OTA 应用

## 项目说明

##### 项目简介

基于 STM32F411CEU6、HAL 和 FreeRTOS 的应用工程，通过 SFUD 将加密升级包写入 W25Q，交由 Bootloader 完成安装。

##### 主要功能

- USART1 / YMODEM 接收升级包，经队列写入外部 A 区，完成尾包刷新及长度核对。
- 保存 EEPROM 升级状态，接收更新命令并按键复位进入 Bootloader。
- 通过 RTT 输出 `APP1 running...`，应用线程每 500 ms 切换 LED 并输出日志。

##### 本阶段说明

本阶段优化 Bootloader 的 EasyLogger 和 SFUD 资源占用。APP1 作为初始应用和升级包接收端，继续用于 APP1 → APP2 升级验证，保留现有的日志与 SFUD 配置。优化后的 Bootloader 已上板验证，功能正常；优化内容与资源对比见[仓库总览](../../README_zh.md)。

##### 仓库结构

```text
Core/                    HAL 初始化与入口
BSP/W25Qxx/              外部闪存管理接口
Middlewares_User/SFUD/   SFUD 库、移植层及适配层
Tasks/APP/               应用线程
Tasks/SWC_OTA/           YMODEM 接收与升级控制
MDK-ARM/                Keil 工程
```

## 工程使用指南

##### 环境与依赖

STM32F411CEU6、W25Q、AT24C02；Keil MDK 5 / ARMCC 5.06 update 7、HAL、FreeRTOS、J-Link / RTT Viewer，以及串口 YMODEM 和配套固件加密工具。

##### 硬件连接与配置

| 外设 | 连接与配置 |
| --- | --- |
| W25Q SPI NOR | PA4：CS；PA5：SCK；PA6：MISO；PA7：MOSI |
| AT24C02 EEPROM | PB8：SCL；PB9：SDA，软件 I²C |
| USART1 | PA9：TX；PA10：RX，115200、8N1 |
| 按键 / LED | PA0：低电平按下；PC13：状态指示灯 |

使用 3.3 V 电平，开发板、外设及 USB 串口模块共地。

##### 编译、运行与观测

1. 打开 `MDK-ARM/STM32F411CEU6.uvprojx` 并编译，保持 APP 链接地址 `0x08008000`；将生成的 BIN 加密为 `.mxxx` 后，通过 Bootloader 安装。
2. 在本应用中，串口发送十六进制字节 `11 22 33`，再通过 YMODEM 发送APP2加密包；收到 `44 55 66` 后发送 `77 88 99`，按下 PA0 按键触发复位。
3. 使用 RTT Viewer 观察 `OTA ready` 和安装后对应 APP 的运行日志。

已完成硬件回归及联合测试：Bootloader 首次安装 APP1 → APP1 接收 APP2 → 复位后运行 APP2。

##### 移植说明

`Middlewares_User/SFUD/port/sfud_port.c` 适配 HAL SPI 和裸机 / FreeRTOS 延时，`inc/sfud_cfg.h` 配置设备，`BSP/W25Qxx/w25qxx_Handler.c` 保留本地业务接口。当前 A 区为 64 KiB，使用 4 KiB 擦除单元；更换平台时核对外设初始化、分区与 RTOS 调用环境。

## 作者思考

无

##### 项目关联思考

无

##### 项目弱关联思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
