**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# 工程 047 APP2

[工程 047](../../README_zh.md) 中用于验证 APP1 升级结果的目标固件。基于 STM32CubeMX 生成的 HAL 与 FreeRTOS 工程，保留与 APP1 相同的应用内 OTA 功能。

## APP1 与 APP2 的区别

功能差异仅在小写文件 `Tasks/APP/app.c` 的 `app_task` 线程中：运行日志和 PC13 LED 翻转间隔不同，其余业务逻辑保持一致。

| 工程 | 运行日志 | PC13 LED 翻转间隔 |
|---|---|---|
| APP1 | `APP1 running...` | 500 个节拍，约 500 ms |
| APP2 | `APP2 running...` | 200 个节拍，约 200 ms |

两者均使用 1 kHz FreeRTOS 节拍，链接地址均为 `0x08008000`，共用相同的 OTA、CRC 与首次启动确认流程。

## 编译与验证

1. 编译 `MDK-ARM/STM32F411CEU6.uvprojx`，用最新 BIN 生成 `APP2_EncryptionBIN.mxxx`。
2. 在运行中的 [APP1](../02_APP1/README_zh.md) 中发送 HEX `11 22 33`，收到 `C` 后通过 YMODEM 发送该加密包。
3. 收到 HEX `44 55 66` 后发送 HEX `77 88 99`，约 20 秒内按键确认，由 Bootloader 安装 APP2。
4. APP2 首次试运行读取 `0x44`，写回 `0x00` 并等待看门狗复位；随后正常启动 FreeRTOS。
5. 确认 `APP2 running...` 和约 200 ms 的 PC13 LED 翻转间隔，证明已切换到 APP2。

APP1 → APP2 的正常流程共复位 3 次，不计额外手动复位。APP2 也可按相同命令继续接收下一版固件；完整操作与硬件配置见[总工程说明](../../README_zh.md)。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
