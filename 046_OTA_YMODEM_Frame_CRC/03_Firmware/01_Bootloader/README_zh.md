**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Bootloader：YMODEM 帧 CRC 与加密固件安装

## 项目说明

##### 项目简介

本工程是 [工程 046](../../README_zh.md) 的 Bootloader 子工程，基于 STM32F4 标准外设库。它接收加密固件或使用 APP 已下载的加密包，负责解密、安装及启动应用。

##### 主要功能

- 在写入外部 A 区前，检查 YMODEM 包类型、包序号反码及 CRC-16/XMODEM。
- 将 A 区按 AES-256 CBC 解密到 B 区，检查镜像长度和启动向量，备份有效旧 APP 并安装新 APP。
- 根据 EEPROM 状态 `0x00`、`0x11`、`0x22` 选择启动或升级路径。
- 保留传输与落盘长度检查；CRC 覆盖各帧数据区及其填充，不计算整文件 CRC。

##### 仓库结构

```text
Core/, Drivers/     外设初始化与 SPL/CMSIS
BSP/, SYSTEM/       外部存储、GPIO 与延时
Middlewares_User/   YMODEM、CRC、AES、日志与 RTT
Tasks/Bootloader/   状态管理、解密、安装与启动
MDK-ARM/            STM32F411CEU6_SPL.uvprojx
```

## 工程使用指南

##### 环境与依赖

STM32F411CEU6、W25Q SPI Flash、AT24C02 EEPROM、USB 转串口及 J-Link；使用 Keil MDK 5.38 / ARMCC 5.06 update 7、SecureCRT 等 YMODEM 发送工具和 RTT Viewer。SPL/CMSIS、libcrc、AES、EasyLogger、SEGGER RTT 已包含在工程中。

##### 硬件连接与配置

使用 3.3 V 电平并共地，串口 TX/RX 交叉连接。

| 接口 | 配置 |
|---|---|
| USART1 | PA9 TX / PA10 RX；115200 波特率、8N1、无流控 |
| W25Q SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| AT24C02 软件 I2C | PB8 SCL / PB9 SDA；保留总线上拉 |
| 状态与输入 | PC13 指示灯；PA0 低电平有效按键 |

Bootloader 位于 `0x08000000`，容量 32 KiB；APP 运行区位于 `0x08008000`，容量 96 KiB。外部 A/B 区为 `0x000000～0x00FFFF` / `0x010000～0x01FFFF`，各 64 KiB。

##### 编译、运行与观测

1. 编译 `MDK-ARM/STM32F411CEU6_SPL.uvprojx`。将 [APP1](../02_APP1/README_zh.md) 链接到 `0x08008000` 后编译，以匹配的 AES-256 CBC 格式、密钥和 IV 加密最新 BIN。包长须大于 16 字节、按 16 字节对齐且不超过 64 KiB。
2. 首次安装先将 EEPROM 地址 `0x00` 的 OTA 状态清为 `0x00`，整片擦除 MCU 内部 Flash 后烧录 Bootloader；外部 Flash 和 EEPROM 不会随之擦除。
3. 等待约 13 秒，按键进入下载，收到 `C` 后通过 YMODEM 发送 `../04_BIN_EncryptionBIN/APP1_EncryptionBIN.mxxx`（重新编译生成）。
4. 通过 RTT 观察 `APP installed … Launching APP` 及 `App1 running...`。首次安装没有有效旧 APP，因此跳过旧 APP 备份。
5. 应用内更新时，由 APP 下载并提交状态 `0x22`；更新命令和按键确认后复位，Bootloader 安装已保存的加密包，操作见 [APP1 使用说明](../02_APP1/README_zh.md)。

2026-10-07 板上测试已通过首次安装及 APP1 → APP2 更新；本轮未测试 CRC 错帧注入和失败回滚。跳转后若日志中断，可重新连接 RTT Viewer 并搜索新的控制块。

##### 移植说明

移植时调整标准外设库外设与时钟、Flash 扇区、镜像及向量地址检查、EEPROM 元信息和外部分区。APP 地址、AES 参数须与配套应用和加密工具一致。

## 作者思考

##### 项目关联思考

无

##### 项目弱关联思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
