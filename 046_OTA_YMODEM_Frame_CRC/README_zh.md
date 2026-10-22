**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 YMODEM 帧 CRC 与加密固件升级实验

## 项目说明

##### 项目简介

本工程在加密固件升级流程中加入逐帧 CRC-16/XMODEM 校验。Bootloader 首次安装 APP1，随后由 APP1 接收 APP2 加密包，并请求 Bootloader 完成更新。

##### 主要功能

- 使用 CRC-16/XMODEM 校验完整的 128/1024 字节 YMODEM 数据区，包含填充；多项式为 `0x1021`、初值为 `0x0000`，不一致的包不交给写入流程。
- 加密包接收到外部 A 区，由 Bootloader 解密到 B 区、备份有效旧 APP 并安装新 APP。
- 使用 EEPROM 状态 `0x00` / `0x11` / `0x22` 区分正常启动、未完成或失败的下载、已完成的下载。
- 通过日志及 PC13 闪烁节奏区分 APP1、APP2；保留文件长度检查，尚未实现整文件 CRC。

##### 仓库结构

```text
00_ProjectManagement/, 00_Reference/   项目管理与参考资料
01_FunctionMap/, 02_Hardware/         功能图谱与硬件资料
03_Firmware/
├── 01_Bootloader/                    下载、解密、安装与启动
├── 02_APP1/                          首次运行应用及 OTA 接收端
├── 03_APP2/                          本轮更新目标及运行状态标识
└── 04_BIN_EncryptionBIN/             明文 BIN 与加密 .mxxx 包
04_Software/, 05_Mechanical/          配套软件与结构目录
06_Factory/, 07_Tools/                生产与工具目录
```

子工程说明：[Bootloader](03_Firmware/01_Bootloader/README_zh.md)、[APP1](03_Firmware/02_APP1/README_zh.md)、[APP2](03_Firmware/03_APP2/README_zh.md)。

## 工程使用指南

##### 环境与依赖

需要 STM32F411CEU6 开发板、W25Q SPI Flash、AT24C02 EEPROM、USB 转串口及 J-Link。使用 Keil MDK 5.38 / ARMCC 5.06 update 7、SecureCRT 等 YMODEM 发送工具及与 Bootloader 匹配的 AES 打包工具。工程已包含 Bootloader 使用的标准外设库、APP 使用的 HAL/FreeRTOS，以及 libcrc、EasyLogger、SEGGER RTT；通过 RTT Viewer 查看日志。

##### 硬件连接与配置

使用 3.3 V 电平并共地，串口 TX/RX 交叉连接。

| 接口 | 配置 |
|---|---|
| USART1 | PA9 TX / PA10 RX；115200 波特率、8N1、无流控 |
| W25Q SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| AT24C02 软件 I2C | PB8 SCL / PB9 SDA；保留总线上拉 |
| 状态与输入 | PC13 指示灯；PA0 低电平有效按键 |

Bootloader 起始地址为 `0x08000000`，容量 32 KiB；两个 APP 均链接到 `0x08008000`，运行区容量 96 KiB。外部 A/B 区分别为 `0x000000～0x00FFFF`、`0x010000～0x01FFFF`，各 64 KiB。

##### 编译、运行与观测

1. 将两个 APP 链接到 `0x08008000` 并编译，再用最新 BIN 生成 `APP1_EncryptionBIN.mxxx`、`APP2_EncryptionBIN.mxxx`，放入[固件目录](03_Firmware/04_BIN_EncryptionBIN/)。打包须匹配 Bootloader 的 AES-256 CBC 文件头、密钥和 IV；包长须大于 16 字节、按 16 字节对齐且不超过 64 KiB。
2. 首次测试先将 EEPROM 地址 `0x00` 的 OTA 状态清为 `0x00`，整片擦除 MCU 内部 Flash 后烧录 Bootloader。整片擦除不会清除外部 Flash 或 EEPROM。
3. 等待约 13 秒启动准备，按键进入下载，收到 `C` 后通过 YMODEM 发送 APP1 加密包；Bootloader 解密、安装并启动 APP1。
4. 在 APP1 中发送 HEX `11 22 33`，收到 `C` 后发送 APP2 加密包；等待 HEX `44 55 66` 和 `OTA ready … EEPROM state=0x22`。
5. 发送 HEX `77 88 99`，约 20 秒内按键确认。APP1 复位，由 Bootloader 安装并启动 APP2。同一串口切换发送工具前，先关闭原工具的连接。

| 应用 | 运行日志 | 每次翻转 LED、输出日志后的延时 |
|---|---|---|
| APP1 | `App1 running...` | 500 个节拍，约 500 ms |
| APP2 | `App2 running...` | 200 个节拍，约 200 ms |

两者的 FreeRTOS 节拍均为 1 kHz；C/H 文件仅在 `Tasks/APP/app.c` 中有区别，OTA 与 CRC 实现一致。

2026-10-07 板上测试已确认 Bootloader → APP1、APP1 → Bootloader → APP2 两条链路成功。本轮未进行 CRC 错帧注入及失败回滚测试。

##### 移植说明

移植时同步调整引脚与时钟、Flash 扇区及容量、DMA/RTOS 配置、EEPROM 布局和 AES 包参数。APP 链接地址及向量表地址须与 Bootloader 一致；其他平台尚未验证。

## 作者思考

##### 项目关联思考

无

##### 项目弱关联思考

无

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
