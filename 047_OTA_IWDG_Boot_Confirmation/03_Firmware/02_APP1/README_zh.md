**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# 工程 047 APP1

[工程 047](../../README_zh.md) 的首次安装演示固件，基于 STM32CubeMX 生成的 HAL 与 FreeRTOS 工程，支持应用内接收加密固件并交由 Bootloader 安装。

## APP1 与 APP2 的区别

功能差异仅在小写文件 `Tasks/APP/app.c` 的 `app_task` 线程中：运行日志和 PC13 LED 翻转间隔不同，其余业务逻辑保持一致。

| 工程 | 运行日志 | PC13 LED 翻转间隔 |
|---|---|---|
| APP1 | `APP1 running...` | 500 个节拍，约 500 ms |
| APP2 | `APP2 running...` | 200 个节拍，约 200 ms |

两者均使用 1 kHz FreeRTOS 节拍，链接地址均为 `0x08008000`，共用相同的 OTA、CRC 与首次启动确认流程。

## 启动与升级

首次试运行时，APP 在外设初始化后读取 EEPROM 状态 `0x44`，写回 `0x00` 并等待看门狗复位；后续状态为 `0x00` 时正常启动 FreeRTOS。

1. 编译 `MDK-ARM/STM32F411CEU6.uvprojx`，用最新 BIN 生成 `APP1_EncryptionBIN.mxxx`，通过 [Bootloader](../01_Bootloader/README_zh.md) 首次安装。
2. 将 [APP2](../03_APP2/README_zh.md) 按相同地址编译并加密，生成 `APP2_EncryptionBIN.mxxx`。
3. 在 APP1 中发送 HEX `11 22 33`，收到 `C` 后通过 YMODEM 发送 APP2 加密包。
4. 等待 HEX `44 55 66` 与 `OTA ready` 日志，确认 EEPROM 状态为 `0x22`。
5. 发送 HEX `77 88 99`，约 20 秒内按键确认；APP1 软件复位，由 Bootloader 完成安装和首次启动确认。
6. 正常流程共复位 3 次，最终输出 `APP2 running...`，LED 翻转间隔约 200 ms。

串口使用 115200、8N1、无流控；切换串口工具前关闭原连接。固件文件位于 `../04_BIN_EncryptionBIN/`，完整硬件配置见[总工程说明](../../README_zh.md)。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
