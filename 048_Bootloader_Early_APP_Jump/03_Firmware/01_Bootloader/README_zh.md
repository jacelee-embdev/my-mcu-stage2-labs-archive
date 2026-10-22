**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# 工程 047 Bootloader

[工程 047](../../README_zh.md) 的启动与固件安装工程，基于 STM32F4 标准外设库，使用 IWDG（独立看门狗）完成新 APP 的首次启动确认。

## 主要功能

- 通过 YMODEM 接收加密固件，保留逐帧 CRC-16/XMODEM 校验。
- 将外部 Flash A 区的 AES-256 CBC 加密包解密到 B 区，备份有效旧 APP，再安装新 APP。
- 根据 EEPROM 状态选择正常启动、升级和首次确认流程。

## 首次启动确认

1. 安装成功后写入 `0x33`，执行软件复位。
2. Bootloader 读取 `0x33`，写入 `0x44`，开启 IWDG 并跳转新 APP。
3. APP 完成外设初始化后读取 `0x44`，写回 `0x00`，停止执行并等待看门狗复位。
4. Bootloader 读取 `0x00`，正常启动 APP，此次不再开启 IWDG。

当前残留 `0x44` 的处理仅为清状态并软件复位，尚未实现跨复位自动恢复旧 APP。

## 编译与使用

- Keil 工程：`MDK-ARM/STM32F411CEU6_SPL.uvprojx`。
- Bootloader 地址：`0x08000000`，预留 32 KiB；APP 地址：`0x08008000`。
- 首次安装：先将 EEPROM 地址 `0x00` 的状态清为 `0x00`，擦除 MCU 内部 Flash，再烧录 Bootloader。MCU 全片擦除不会清除 EEPROM 或外部 Flash。
- 按键进入下载，收到 `C` 后通过 YMODEM 发送最新 APP 加密包。
- APP 内升级时，Bootloader 使用 APP 已下载并提交为 `0x22` 的包，无须再次接收。

首次安装成功的正常流程包含 **2 次自动复位**；从运行中的 APP 发起升级包含 **3 次复位**。均不计上电及额外手动复位。

串口、硬件连接与整体操作见[总工程说明](../../README_zh.md)。切换 Bootloader / APP 后，RTT 控制块可能变化，需要重新连接。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
