**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 加密固件 OTA Bootloader 实验

## 项目说明

##### 项目简介

本工程是 工程 044 的 Bootloader 子工程，基于 STM32F411CEU6 和标准外设库实现串口固件升级。它负责接收或读取加密包、解密、备份旧 APP、安装新 APP，并跳转到内部 Flash 中的应用程序，与 APP1、APP2 工程配合完成升级实验。

##### 主要功能

- 通过 USART1 和 YMODEM 接收加密固件，写入外部 Flash 的 A 区。
- 将 A 区固件按 AES-256 CBC 解密到 B 区，检查长度及 APP 启动入口。
- 将有效旧 APP 备份到 A 区，再把 B 区新 APP 写入内部 Flash；无有效旧 APP 时允许首次安装，有本次有效备份时支持安装失败恢复。
- 根据 EEPROM 状态选择启动路径：`0x00` 正常启动或按键下载；`0x11` 优先启动旧 APP，失败后重新下载；`0x22` 使用已下载的加密包升级。

##### 仓库结构

```text
01_Bootloader/
├── Core/              # 启动入口、外设初始化及内部 Flash 操作
├── BSP/               # W25Q 外部 Flash、AT24C02 EEPROM 驱动
├── Tasks/Bootloader/  # 状态管理、解密、备份、安装和跳转
├── Middlewares_User/  # YMODEM、AES、EasyLogger、SEGGER RTT
├── SYSTEM/            # 延时支持
├── Drivers/           # CMSIS 与 STM32F4 标准外设库
└── MDK-ARM/           # Keil 工程
```

## 工程使用指南

##### 环境与依赖

- 硬件：STM32F411CEU6、W25Q 系列 SPI Flash、AT24C02 EEPROM、按键、PC13 指示灯、USB 转串口及 J-Link。
- 软件：Keil MDK（工程配置为 ARM Compiler 5.06 update 7）、支持 YMODEM 的 SecureCRT、可发送 HEX 字节的串口工具；本次日志观测使用 J-Link RTT Viewer V8.12g。库与中间件已随工程提供。

##### 硬件连接与配置

| 对象 | 引脚与配置 |
| --- | --- |
| USART1 | PA9/TX → 串口模块 RX，PA10/RX ← 串口模块 TX；115200、8N1、无流控 |
| W25Q Flash | PA4/CS、PA5/SCK、PA6/MISO、PA7/MOSI；SPI 模式 0 |
| AT24C02 | PB8/SCL、PB9/SDA；软件 I²C，器件地址字节 `0xA0` |
| 按键 / 指示灯 | PA0 上拉、按下接地；PC13 指示灯 |
| 调试与供电 | J-Link 使用 SWD；串口模块、外设与开发板共地，使用匹配的 3.3 V 电平 |

| 存储区域 | 地址范围 | 用途 |
| --- | --- | --- |
| 内部 Bootloader | `0x08000000～0x08007FFF`（32 KiB） | 引导程序 |
| 内部 APP | `0x08008000～0x0801FFFF`（96 KiB） | 应用运行区 |
| 外部 A 区 | `0x000000～0x00FFFF`（64 KiB） | 先存新固件密文，解密后可覆盖为旧 APP 备份 |
| 外部 B 区 | `0x010000～0x01FFFF`（64 KiB） | 新 APP 明文 |

加密包长度上限为 64 KiB，须按 16 字节对齐；包的文件头、密钥和 IV 必须与本工程一致。

##### 编译、运行与观测

1. 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx` 并编译。配套 APP 的链接基址为 `0x08008000`，测试加密包位于 [`../04_BIN_EncryptionBIN/`](../04_BIN_EncryptionBIN/)。
2. 首次安装测试前，单独将 EEPROM 地址 `0x00` 的升级标志清为 `0x00`，再选择 **Erase Chip** 擦除内部 Flash 并烧录 Bootloader。Erase Chip 不会清除外部 Flash 或 EEPROM。
3. 复位后等待约 13 秒的启动准备时间，按键进入下载；收到连续的 `C` 后，用 SecureCRT 的 YMODEM 发送 APP1 加密包。接收完成后自动解密、安装并启动 APP1；此时没有旧 APP，跳过备份。
4. 在 APP1 中发送 HEX `11 22 33`，收到 `C` 后切换到 SecureCRT，发送 APP2 加密包。收到 HEX `44 55 66`（ASCII 显示为 `DUf`）表示加密包已接收并确认就绪，此时尚未安装 APP2。同一串口切换工具时需先关闭原工具的串口连接。
5. 发送 HEX `77 88 99`，在约 20 秒内按下按键确认。APP 软复位后，Bootloader 按 `0x22` 流程将 A 区密文解密到 B 区、把内部 APP1 备份到 A 区，再安装并启动 APP2。
6. 通过 RTT 日志和 PC13 闪烁观察运行状态。跳转后若日志中断，可重新打开 RTT Viewer，让它重新搜索控制块；本轮在重新打开后看到了 APP1 日志。

**2026-10-06 板上回归结果（代码规范整理后）：**

| 阶段 | 实测结果 |
| --- | --- |
| Bootloader → APP1 | 传输显示 38 KB、约 11 秒、0 Errors；解密日志记录 APP 明文 39,420 字节，并提示无有效旧 APP；最终 `APP1 running...` 约每 500 ms 输出一次，指示灯闪烁 |
| APP1 接收 APP2 | 传输显示 38 KB、约 11 秒、0 Errors；收到 `44 55 66`，日志为 `OTA ready: size=39440, EEPROM state=0x22` |
| Bootloader → APP2 | 更新命令及按键确认后完成升级，最终 `APP2 running...` 约每 200 ms 输出一次 |

首次安装与 `0x22` 正常升级回归通过；`0x11` 中断恢复和安装失败回滚未纳入本轮板上测试。

##### 移植说明

移植时同步调整外设引脚、时钟、内部 Flash 扇区及容量、APP 链接地址与向量表、外部 A/B 分区和 EEPROM 元信息布局。配套加密工具与 Bootloader 的包格式、密钥和 IV 必须一致。本工程实测平台为 STM32F411CEU6。

## 作者思考

##### 项目关联思考

无。

##### 项目弱关联思考

无。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
