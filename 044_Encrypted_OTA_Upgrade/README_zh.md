**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 加密固件 OTA 升级实验

## 项目说明

##### 项目简介

本项目基于 STM32F411CEU6，包含 Bootloader、APP1 和 APP2，演示从无 APP 的首次安装到应用运行期间下载并升级新固件的完整流程。上位机通过串口 YMODEM 发送加密包，APP 提交升级状态并请求复位，Bootloader 完成解密、旧 APP 备份、安装与启动。

##### 主要功能

- Bootloader 支持按键下载；APP 支持运行期间通过 USART1 DMA 双缓冲接收加密固件。
- 外部 Flash A 区先保存新固件密文，解密后的明文存入 B 区；解密成功后将有效旧 APP 备份到 A 区，再将 B 区安装到内部 Flash。
- EEPROM 保存升级状态及长度：`0x00` 为正常启动，`0x11` 为下载中/未完成，`0x22` 为完整包就绪；安装失败时可用本次有效旧 APP 备份恢复。
- APP1 与 APP2 的功能、硬件配置及 OTA 流程相同，仅测试线程的日志和延时不同，用于判断升级后的运行状态。

| 工程 | 测试日志 | 每轮延时 / PC13 切换间隔 | 说明 |
| --- | --- | --- | --- |
| [Bootloader](03_Firmware/01_Bootloader/README_zh.md) | `Bootloader running...` | — | 标准外设库，负责引导、解密及安装 |
| [APP1](03_Firmware/02_APP1/README_zh.md) | `APP1 running...` | 500 ms | HAL + FreeRTOS，首次安装的应用 |
| [APP2](03_Firmware/02_APP2/README_zh.md) | `APP2 running...` | 200 ms | 与 APP1 相同的应用功能，作为升级目标 |

APP 每轮切换一次 PC13；表中为电平切换间隔。

##### 仓库结构

```text
044_Encrypted_OTA_Upgrade/
├── 00_Reference/      # 参考资料
├── 01_FunctionMap/    # 功能划分
├── 02_Hardware/       # 硬件资料
├── 03_Firmware/
│   ├── 01_Bootloader/       # 引导程序
│   ├── 02_APP1/             # 初始应用
│   ├── 02_APP2/             # 升级目标应用
│   └── 04_BIN_EncryptionBIN/ # 明文固件与加密包
└── README_zh.md       # 本项目总览
```

[固件目录](03_Firmware/04_BIN_EncryptionBIN/)同时保存 APP1、APP2 的未加密固件与加密包，当前文件如下：

| 文件 | 大小（字节） | 用途 |
| --- | --- | --- |
| `APP1.bin` | 39,420 | APP1 明文固件 |
| `APP1_EncryptionBIN.mxxx` | 39,440 | Bootloader 首次安装时发送的加密包 |
| `APP2.bin` | 39,420 | APP2 明文固件 |
| `APP2_EncryptionBIN.mxxx` | 39,440 | APP1 应用内升级时发送的加密包 |

## 工程使用指南

##### 环境与依赖

- 硬件：STM32F411CEU6、W25Q 系列 SPI Flash、AT24C02 EEPROM、PA0 按键、PC13 指示灯、USB 转串口及 J-Link。
- 软件：Keil MDK（三个工程配置为 ARM Compiler 5.06 update 7）、支持 YMODEM 的 SecureCRT、支持 HEX 发送的串口工具；实测使用 J-Link RTT Viewer V8.12g。Bootloader 使用标准外设库，两个 APP 使用 HAL 和 FreeRTOS V10.3.1，所需库已随工程提供。

##### 硬件连接与配置

| 对象 | 引脚与配置 |
| --- | --- |
| 串口升级 | USART1：PA9/TX → 串口模块 RX，PA10/RX ← 串口模块 TX；115200、8N1、无流控 |
| 外部 Flash | PA4/CS、PA5/SCK、PA6/MISO、PA7/MOSI；SPI 模式 0 |
| EEPROM | PB8/SCL、PB9/SDA；软件 I²C |
| 按键 / 指示灯 | PA0 上拉、按下接地；PC13 指示灯 |
| 调试与供电 | J-Link 使用 SWD；各设备共地，使用匹配的 3.3 V 电平 |

| 存储区域 | 地址范围 | 作用 |
| --- | --- | --- |
| 内部 Bootloader | `0x08000000～0x08007FFF`（32 KiB） | 引导程序 |
| 内部 APP | `0x08008000～0x0801FFFF`（96 KiB） | APP1 / APP2 共用运行区 |
| 外部 A 区 | `0x000000～0x00FFFF`（64 KiB） | 新固件密文，解密后可覆盖为旧 APP 明文备份 |
| 外部 B 区 | `0x010000～0x01FFFF`（64 KiB） | 待安装的新 APP 明文 |

两个 APP 使用相同的链接基址 `0x08008000`，依次安装到同一个运行区。加密包上限为 64 KiB，须按 16 字节对齐并匹配 Bootloader 的 AES-256 CBC 文件头格式、密钥和 IV。

##### 编译、运行与观测

1. 在各工程的 `MDK-ARM/` 目录打开 Keil 工程并编译；详细使用方法见上方三个工程的 README。
2. 首次测试将 EEPROM 地址 `0x00` 的升级标志单独清为 `0x00`，再选择 **Erase Chip** 擦除 MCU 内部 Flash 并烧录 Bootloader，此时内部没有 APP。Erase Chip 不会清除外部 Flash 或 EEPROM。
3. 复位后等待约 13 秒，按键进入 Bootloader 下载；收到连续的 `C` 后，用 YMODEM 发送 `APP1_EncryptionBIN.mxxx`。Bootloader 将密文写 A 区、解密到 B 区，跳过不存在的旧 APP 备份，安装并启动 APP1。
4. 在 APP1 中发送 HEX `11 22 33`，收到 `C` 后用 YMODEM 发送 `APP2_EncryptionBIN.mxxx`。包及元信息确认后，APP1 保存 EEPROM 状态 `0x22` 并回复 HEX `44 55 66`（ASCII 显示为 `DUf`）；此时仍运行 APP1。切换串口工具时先关闭原工具的连接。
5. 发送 HEX `77 88 99`，在约 20 秒内按下按键确认。APP 软复位进入 Bootloader，依次解密 A → B、备份内部 APP1 → A、安装 B → 内部 APP 运行区，最后启动 APP2。
6. 观察 PC13 和 RTT 日志：APP1 约每 500 ms 输出一次，APP2 约每 200 ms 输出一次。跳转后若日志中断，可重新打开 RTT Viewer 搜索控制块。

**2026-10-06 板上回归结果（代码规范整理后）：**

| 阶段 | 实测结果 |
| --- | --- |
| 首次安装 APP1 | 传输显示 38 KB、约 11 秒、0 Errors；解密日志记录 APP 明文 39,420 字节并提示无有效旧 APP；最终出现 `APP1 running...`，指示灯闪烁 |
| APP1 接收 APP2 | 传输显示 38 KB、约 11 秒、0 Errors；收到 `44 55 66`，日志为 `OTA ready: size=39440, EEPROM state=0x22` |
| 更新并启动 APP2 | 更新命令及按键确认后完成升级，最终日志为 `APP2 running...`，输出间隔约 200 ms |

首次安装与 APP1 → APP2 的 `0x22` 正常升级回归通过。该结果覆盖代码规范整理后的正常链路；`0x11` 中断恢复、安装失败回滚及反向升级未纳入本轮板上测试。

##### 移植说明

同步修改 Bootloader 和两个 APP 的引脚、时钟、Flash 扇区与容量、APP 链接地址和向量表、外部 A/B 分区及 EEPROM 元信息布局，并保持串口命令、加密包格式、密钥和 IV 一致。实测平台为 STM32F411CEU6。

## 作者思考

##### 项目关联思考

无。

##### 项目弱关联思考

无。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
