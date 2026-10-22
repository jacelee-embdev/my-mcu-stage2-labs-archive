**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 AES 软件加解密实验

## 项目说明

基于 STM32F411CEU6 的 AES 软件加密实验工程，核心为 `Middlewares_User/Algorithms/AES` 下的 `aes.c` / `aes.h` 两个文件。

##### 项目简介

在 STM32F411CEU6 上以纯软件方式实现 AES 加密算法，并通过串口打印加密、解密及往返校验结果，验证实现的正确性。

##### 主要功能

- AES 加密与解密，密钥长度 128 / 192 / 256 位可选（`AES_KEY_LENGTH`），默认 AES-128
- ECB / CBC 两种工作模式可选（`AES_MODE`），默认 CBC，支持原地加解密
- 内置加解密示例 `test()`：上电后自动完成一次加密 → 解密 → 往返比对并打印结果
- 附带运行时 S 盒生成函数（`generate_aes_sbox` 等），可用于验证静态 S 盒表

##### 仓库结构

```
├── Core/                  # HAL 初始化与应用入口（main.c 中调用 test()）
├── Drivers/               # CMSIS 与 STM32F4 HAL 库
├── Middlewares_User/
│   ├── Algorithms/AES/    # AES 核心：aes.c / aes.h
│   ├── EasyLogger/        # 日志组件
│   └── SEGGER_RTT/        # RTT 调试输出
├── Tasks/Debug/           # 调试模块初始化
├── MDK-ARM/               # Keil µVision 工程
└── STM32F411CEU6.ioc      # CubeMX 配置文件
```

## 工程使用指南

##### 环境与依赖

| 项目 | 说明 |
| --- | --- |
| 硬件 | STM32F411CEU6 开发板（必需） |
| 调试器 | J-Link（烧录必需） |
| 工具链 | Keil MDK-ARM（µVision） |
| 固件库 | STM32Cube FW_F4 V1.28.3 |
| 串口工具 | USB 转 TTL + 任意串口终端（观测结果必需） |

##### 硬件连接与配置

USART1 用于打印结果，参数为 115200 8N1：

| 开发板引脚 | 连接 | 说明 |
| --- | --- | --- |
| PA9 (USART1_TX) | USB 转 TTL 的 RXD | 串口发送 |
| PA10 (USART1_RX) | USB 转 TTL 的 TXD | 串口接收 |
| GND | USB 转 TTL 的 GND | 必须共地 |

##### 编译、运行与观测

1. 用 Keil µVision 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译工程。
2. 通过 J-Link 将程序下载到开发板并复位运行。
3. 打开串口终端（115200 8N1），可看到 `test()` 输出的密文十六进制、解密后的明文 `JesMicro`，以及往返校验结果 `AES roundtrip = PASS`。

##### 移植说明

- `aes.c` / `aes.h` 与硬件无关，移植时只需处理 `#include "main.h"`（提供 `uint8_t` 等类型即可）和 `test_sbox()` 中对 EasyLogger 的引用。
- 文件内已保留 `code` / `xdata` 等 C51 兼容宏，可直接用于 8051 平台。
- 数据长度必须为 16 字节整数倍，工程未实现填充，需调用方自行补齐。

## 作者思考

##### 项目关联思考

无

##### 项目弱关联思考

无

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
