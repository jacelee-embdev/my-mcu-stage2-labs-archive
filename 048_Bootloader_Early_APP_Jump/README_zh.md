**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Bootloader 启动优化

在原有加密 OTA 与 IWDG 首次启动确认基础上，通过 **保留 RAM 跳转请求 + 软件复位**，让 Bootloader 在再次进入 `main()` 时提前跳转 APP。本次仅修改 Bootloader，APP 工程保持原样；板上验证使用已有 APP1。

## 工程组成

| 工程 | 作用 | 链接地址 |
|---|---|---|
| [Bootloader](03_Firmware/01_Bootloader/README_zh.md) | OTA 管理、启动确认与复位后提前跳转 | `0x08000000` |
| [APP1](03_Firmware/02_APP1/README_zh.md) | 本次验证使用的已有应用，支持应用内 OTA | `0x08008000` |
| [APP2](03_Firmware/03_APP2/README_zh.md) | 保留原有升级演示固件 | `0x08008000` |

Bootloader 预留 32 KiB，APP 运行区为 96 KiB。[固件目录](03_Firmware/04_BIN_EncryptionBIN/)保存 BIN 与加密 `.mxxx` 包；原有 OTA 操作见各子工程 README。

## 启动流程与变更

1. **首次上电**：没有有效跳转请求时，`Core/Src/main.c` 初始化外设、打印日志并等待约 5 秒，再调用 `OTA_state_mangner()`。
2. **写请求并复位**：`Tasks/Bootloader/bootloader.c` 在正常启动状态 `0x00` 下，确认未按下载键且 APP 入口有效后，写入 `g_app_jump_request = 0x55AA55AAU`，调用 `System_SoftwareReset()`。
3. **复位后提前跳转**：再次进入 `main()` 时，读取请求、清为 `0`，调用 `bootloader_jump_to_app()`，进入 APP 启动入口。

`MDK-ARM/bootloader_noinit.sct` 将 SRAM 最后 16 字节（`0x2001FFF0–0x2001FFFF`）设为 `UNINIT`，接收变量所在的 `NO_INIT` 节，使请求跨软件复位保留。Keil Linker 取消勾选 **Use Memory Layout from Target Dialog**，选择 `./bootloader_noinit.sct`。

提前跳转发生在 `main()` 的外设初始化之前；`SystemInit()` 和 C 运行库初始化仍会执行。首次约 5 秒等待保留，复位后不再重复初始化外设和等待。

## 环境与连接

STM32F411CEU6、W25Q SPI Flash、AT24C02 EEPROM、USB 转串口与 J-Link；使用 Keil MDK（ARM Compiler 5）和 RTT Viewer。

| 接口 | 配置 |
|---|---|
| USART1 | PA9 TX / PA10 RX；115200、8N1、无流控 |
| SPI1 | PA4 CS / PA5 SCK / PA6 MISO / PA7 MOSI |
| EEPROM 软件 I2C | PB8 SCL / PB9 SDA |
| LED / 按键 | PC13 / PA0，按键低电平有效 |

使用 3.3 V 电平并共地，串口 TX/RX 交叉连接，I2C 保留上拉。

## 编译与验证

1. 编译 Bootloader；Keil 下载设置选择 **Erase Sectors、Program、Verify**，只擦除和下载 Bootloader 使用的区域，保留 `0x08008000` 的 APP。不要选择 **Erase Full Chip**，本次无需重新发送加密固件。
2. 确认 EEPROM 地址 `0x00` 的升级状态为 `0x00`，不按下载键。退出调试后重新上电，观察启动日志及 APP1 的 PC13 LED 闪烁。
3. 调试时在 `main()` 的请求检查处设断点：首次检查后按 **F5（继续运行）**，程序自行写请求并软复位；再次命中断点应读到 `0x55AA55AA`。继续运行并暂停，确认请求已清为 `0`，PC 位于 APP 区域 `0x08008000–0x0801FFFF`。

**2026-10-08 已完成验证**：Bootloader 编译为 0 错误、0 警告；板上观察到请求 `0 → 0x55AA55AA → 0`，暂停后 PC 为 `0x0800ED12`，APP1 LED 正常闪烁。保留 RAM 和复位后提前跳转流程通过验证。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
