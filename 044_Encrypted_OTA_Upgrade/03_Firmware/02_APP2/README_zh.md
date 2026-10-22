**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 APP2 应用内 OTA 升级实验

## 项目说明

##### 项目简介

APP2 是 工程 044 的升级目标应用，基于 STM32F411CEU6、HAL 和 FreeRTOS 运行，用日志标志及指示灯切换间隔区分升级前后的应用。它也提供与 APP1 相同的应用内 OTA 接收和升级请求功能。完整实验见 [工程 044 总览](../../README_zh.md)。

##### 主要功能

- 在 FreeRTOS 下同时运行应用测试线程与 OTA 线程，输出运行日志并切换 PC13 指示灯。
- 通过 USART1 DMA 双缓冲和 YMODEM 接收加密包，利用队列与信号量协调外部 Flash A 区写入。
- 下载前保存 EEPROM 状态 `0x11`；写入和长度确认后保存包长度及 `0x22`，回复完成应答，再经更新命令和按键确认软复位至 Bootloader。

**APP1 与 APP2 的硬件配置、通信协议及 OTA 流程相同，仅 `Tasks/APP/app.c` 测试线程的日志和延时不同：**

| 工程 | 日志 | 每轮延时 / PC13 切换间隔 |
| --- | --- | --- |
| APP1 | `APP1 running...` | 500 ms |
| APP2 | `APP2 running...` | 200 ms |

当前 FreeRTOS 节拍为 1000 Hz，每轮切换一次 PC13；表中为电平切换间隔。

##### 仓库结构

```text
02_APP2/
├── Core/              # HAL 外设初始化、DMA 与中断
├── BSP/               # 串口接收封装、W25Q Flash、AT24C02 EEPROM
├── Tasks/APP/         # 日志及指示灯测试线程
├── Tasks/SWC_OTA/     # OTA 状态管理与 Flash 写入线程
├── SysSupport/        # 日志和 RTOS 资源初始化
├── Middlewares/       # FreeRTOS
├── Middlewares_User/  # YMODEM、EasyLogger、SEGGER RTT
└── MDK-ARM/           # Keil 工程
```

## 工程使用指南

##### 环境与依赖

- 硬件：STM32F411CEU6、W25Q 系列 SPI Flash、AT24C02 EEPROM、按键、PC13 指示灯、USB 转串口及 J-Link。
- 软件：Keil MDK（ARM Compiler 5.06 update 7）、HAL、FreeRTOS V10.3.1、SecureCRT 及支持 HEX 发送的串口工具；日志观测使用 J-Link RTT Viewer V8.12g。库与中间件已随工程提供。

##### 硬件连接与配置

| 对象 | 引脚与配置 |
| --- | --- |
| USART1 | PA9/TX → 串口模块 RX，PA10/RX ← 串口模块 TX；115200、8N1、无流控 |
| W25Q Flash | PA4/CS、PA5/SCK、PA6/MISO、PA7/MOSI；SPI 模式 0 |
| AT24C02 | PB8/SCL、PB9/SDA；软件 I²C |
| 按键 / 指示灯 | PA0 上拉、按下接地；PC13 指示灯 |
| 调试与供电 | J-Link 使用 SWD；各设备共地，使用匹配的 3.3 V 电平 |

APP 运行基址为 `0x08008000`，由配套 Bootloader 启动。外部 A 区为 `0x000000～0x00FFFF`，B 区为 `0x010000～0x01FFFF`，各 64 KiB；APP 下载到 A 区，后续解密与安装由 Bootloader 执行。

##### 编译、运行与观测

1. 打开 `MDK-ARM/STM32F411CEU6.uvprojx` 并编译，保持 APP 链接基址 `0x08008000`。明文 `APP2.bin` 与加密包 `APP2_EncryptionBIN.mxxx` 均在 [固件目录](../04_BIN_EncryptionBIN/)；重新打包时须匹配 Bootloader 的 AES-256 CBC 文件头格式、密钥和 IV，加密包不超过 64 KiB 且按 16 字节对齐。
2. 本轮先按 [Bootloader 使用指南](../01_Bootloader/README_zh.md)安装并启动 APP1，再通过以下应用内 OTA 步骤发送 APP2 加密包，安装并启动本工程。
3. 在运行中的 APP 中发送 HEX `11 22 33`，收到 `C` 后切换至 SecureCRT，用 YMODEM 发送加密包；本轮从 APP1 发送的是 `APP2_EncryptionBIN.mxxx`。切换工具时先关闭原工具的串口连接。
4. 等待 HEX `44 55 66`（ASCII 显示为 `DUf`）和 `OTA ready: size=39440, EEPROM state=0x22`。此时加密包及元信息已确认，当前 APP 继续运行，尚未安装新 APP。
5. 发送 HEX `77 88 99`，在约 20 秒内按下按键确认。APP 软复位后，Bootloader 将 A 区解密到 B 区、备份有效旧 APP 到 A 区，再安装并启动新 APP。按键超时会回到等待下载。
6. 用 RTT 日志和 PC13 指示灯确认运行状态；若跳转后日志中断，可重新打开 RTT Viewer 搜索控制块。

**2026-10-06 板上回归结果（代码规范整理后）：**

| 观测项 | 实测结果 |
| --- | --- |
| APP1 运行 | `APP1 running...` 约每 500 ms 输出一次，指示灯闪烁 |
| APP1 下载 APP2 | 传输显示 38 KB、约 11 秒、0 Errors；收到 `44 55 66`，日志确认包长 39,440 字节、EEPROM 状态 `0x22` |
| 升级至 APP2 | 更新命令及按键确认后，`APP2 running...` 约每 200 ms 输出一次 |

本轮 APP1 → APP2 正常升级回归通过；APP2 接收后续固件的流程与 APP1 相同，本轮未单独测试反向升级。

##### 移植说明

同步调整 HAL 外设与 DMA 配置、RTOS 中断优先级、APP 链接地址和向量表，以及与 Bootloader 共用的 Flash 分区、EEPROM 元信息和命令字节。加密工具与 Bootloader 的包格式、密钥和 IV 必须一致。实测平台为 STM32F411CEU6。

## 作者思考

##### 项目关联思考

无。

##### 项目弱关联思考

无。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
