**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 APP2：YMODEM 帧 CRC 与应用内固件升级实验

## 项目说明

##### 项目简介

APP2 是 [工程 046](../../README_zh.md) 的更新目标，由 APP1 复制而来。两者的 HAL/FreeRTOS OTA 和 CRC 功能一致，通过运行日志及 LED 闪烁节奏识别更新后的应用。

##### 主要功能

- 使用 USART1 DMA 与同步双缓冲接收 YMODEM 加密数据包。
- 完整数据区通过 CRC-16/XMODEM 校验后才交给外部 Flash 写入线程；填充参与 CRC，但不写入文件。
- 提交包长度和 EEPROM 状态 `0x22`，经更新命令及按键确认后复位；解密与安装由 Bootloader 执行。
- 输出 `App2 running...`，每轮翻转 PC13 后延时 200 个节拍；不计算整文件 CRC。

##### 仓库结构

```text
Core/, Drivers/     HAL 外设与芯片支持
Middlewares/        FreeRTOS
Middlewares_User/   YMODEM、CRC、日志与 RTT
BSP/, SysSupport/   存储、串口、调试与系统初始化
Tasks/APP/          指示灯与应用运行日志线程
Tasks/SWC_OTA/      下载状态机与 Flash 写入线程
MDK-ARM/            Keil 工程
```

## 工程使用指南

##### 环境与依赖

STM32F411CEU6、W25Q SPI Flash、AT24C02 EEPROM、USB 转串口及 J-Link；使用 Keil MDK 5.38 / ARMCC 5.06 update 7、AES 打包工具、HEX 命令发送工具、SecureCRT 等 YMODEM 发送工具和 RTT Viewer。HAL、FreeRTOS、libcrc、EasyLogger、SEGGER RTT 已包含在工程中。

##### 硬件连接与配置

使用 3.3 V 电平并共地，串口 TX/RX 交叉连接。

| 接口 | 配置 |
|---|---|
| USART1 | PA9 TX / PA10 RX；115200 波特率、8N1、无流控 |
| W25Q SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| AT24C02 软件 I2C | PB8 SCL / PB9 SDA；保留总线上拉 |
| 状态与输入 | PC13 指示灯；PA0 低电平有效按键 |

两个 APP 均链接到 `0x08008000`，RTOS 节拍为 1 kHz；下载使用外部 A 区 `0x000000～0x00FFFF`，容量 64 KiB。Bootloader 保留在 `0x08000000`。

##### 编译、运行与观测

1. 将 `MDK-ARM/STM32F411CEU6.uvprojx` 链接到 `0x08008000` 并编译，用最新 APP2 BIN 生成 `../04_BIN_EncryptionBIN/APP2_EncryptionBIN.mxxx`（重新编译生成）。
2. 从已安装的 [APP1](../02_APP1/README_zh.md) 开始，发送 HEX `11 22 33`，收到 `C` 后通过 YMODEM 发送 APP2 加密包。同一串口切换工具前先关闭原连接。
3. 等待 HEX `44 55 66` 和 `OTA ready … EEPROM state=0x22`，再发送 HEX `77 88 99`，约 20 秒内按键确认。
4. APP1 复位进入 Bootloader，在相同 APP 地址安装 APP2。通过 `App2 running...` 和约 200 ms 的 PC13 翻转间隔，确认运行标识已从 APP1 切换到 APP2。

| 应用 | 运行日志 | 每次翻转 LED、输出日志后的延时 |
|---|---|---|
| APP1 | `App1 running...` | 500 个节拍，约 500 ms |
| APP2 | `App2 running...` | 200 个节拍，约 200 ms |

两者的 FreeRTOS 节拍均为 1 kHz；C/H 文件仅在 `Tasks/APP/app.c` 中有区别，OTA 与 CRC 实现一致。

APP2 在本轮测试中用于确认新应用已安装并启动。它保留了后续 OTA 接收能力，但本轮测试止于 APP2。2026-10-07 板上测试已通过 APP1 → APP2 更新；本轮未进行 CRC 错帧注入。

##### 移植说明

保持 APP 链接基址、Bootloader 跳转与向量表地址、Flash 分区、EEPROM 元信息、AES 包格式和串口参数一致。更换开发板时调整 HAL 外设、DMA 与 RTOS 中断优先级；本工程已验证的平台为 STM32F411CEU6。

## 作者思考

##### 项目关联思考

无

##### 项目弱关联思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
