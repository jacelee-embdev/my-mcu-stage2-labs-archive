**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 OTA 与看门狗首次启动确认

本工程在加密 OTA 升级流程中加入新 APP 的首次启动确认，实际使用 **IWDG（独立看门狗）**。Bootloader 首次安装 APP1，随后由 APP1 接收 APP2 加密包并请求更新。

## 工程组成

| 工程 | 作用 | 链接地址 |
|---|---|---|
| [Bootloader](03_Firmware/01_Bootloader/README_zh.md) | 下载、解密、备份、安装与启动确认 | `0x08000000` |
| [APP1](03_Firmware/02_APP1/README_zh.md) | 首次安装的演示固件，支持应用内 OTA | `0x08008000` |
| [APP2](03_Firmware/03_APP2/README_zh.md) | 验证升级结果，保留相同 OTA 功能 | `0x08008000` |

[固件目录](03_Firmware/04_BIN_EncryptionBIN/)保存 BIN 与加密 `.mxxx` 包。Bootloader 预留 32 KiB，APP 运行区为 96 KiB；外部 Flash A/B 区各 64 KiB，地址为 `0x000000～0x00FFFF` / `0x010000～0x01FFFF`。

## 环境与连接

STM32F411CEU6、W25Q SPI Flash、AT24C02 EEPROM、USB 转串口与 J-Link；使用 Keil MDK、匹配的 AES 打包工具、YMODEM 发送工具及 RTT Viewer。

| 接口 | 配置 |
|---|---|
| USART1 | PA9 TX / PA10 RX；115200、8N1、无流控 |
| SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| EEPROM 软件 I2C | PB8 SCL / PB9 SDA |
| LED / 按键 | PC13 / PA0，按键低电平有效 |

使用 3.3 V 电平并共地，串口 TX/RX 交叉连接，I2C 保留上拉。

## 编译与升级

1. 将两个 APP 链接到 `0x08008000` 后编译，用最新 BIN 生成 `APP1_EncryptionBIN.mxxx`、`APP2_EncryptionBIN.mxxx`。打包须匹配 Bootloader 的 AES-256 CBC 格式、密钥和 IV，加密包不超过 64 KiB。
2. 首次安装先将 EEPROM 地址 `0x00` 的状态清为 `0x00`，擦除 MCU 内部 Flash 并烧录 Bootloader。按键进入下载，收到 `C` 后通过 YMODEM 发送 APP1 加密包。MCU 全片擦除不会清除 EEPROM 或外部 Flash。
3. 在 APP1 中发送 HEX `11 22 33`，收到 `C` 后通过 YMODEM 发送 APP2 加密包；等待 HEX `44 55 66` 和 `OTA ready` 日志。
4. 发送 HEX `77 88 99`，约 20 秒内按键确认，交由 Bootloader 安装并完成首次启动确认。切换串口工具前先关闭原连接。

YMODEM 保留逐帧 CRC-16/XMODEM 校验。切换 Bootloader / APP 后，RTT 控制块可能变化，需要重新连接。

## 状态与复位

EEPROM 地址 `0x00` 保存升级状态：

| 状态 | 含义 |
|---|---|
| `0x00` | 正常启动 |
| `0x11` | 下载中或下载未完成 |
| `0x22` | 加密包下载完成，等待安装 |
| `0x33` | 新 APP 安装完成，等待首次确认 |
| `0x44` | 新 APP 正在首次确认 |

安装后写入 `0x33` 并软件复位；Bootloader 写入 `0x44`、开启 IWDG 并跳转 APP。APP 完成外设初始化后写回 `0x00` 并等待看门狗复位，随后正常启动。

成功流程中，首次安装包含 **2 次自动复位**，APP 内升级包含 **3 次复位**；不计上电及额外手动复位。当前残留 `0x44` 仅清状态并软件复位，尚未实现跨复位自动恢复旧 APP。

## APP1 与 APP2 的区别

功能差异仅在小写文件 `Tasks/APP/app.c` 的 `app_task` 线程中，其余业务逻辑保持一致。

| 工程 | 运行日志 | PC13 LED 翻转间隔 |
|---|---|---|
| APP1 | `APP1 running...` | 500 个节拍，约 500 ms |
| APP2 | `APP2 running...` | 200 个节拍，约 200 ms |

两者均使用 HAL、FreeRTOS 和 1 kHz 节拍，共用相同的 OTA、CRC 与首次启动确认流程。

2026-10-07 板上日志已确认首次安装 APP1 和 APP1 → APP2 升级后正常运行，启动次数符合预期；首次试运行的 `0x44` 日志和复位原因尚未完整抓取。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
