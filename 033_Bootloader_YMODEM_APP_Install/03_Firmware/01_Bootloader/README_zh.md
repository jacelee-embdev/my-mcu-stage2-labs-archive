**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## 简介

本工程是基于 STM32F4 标准外设库（SPL）的 Bootloader，运行于
`0x08000000`。它通过 USART1 和 YMODEM 接收 APP BIN 固件，将固件写入
内部 Flash，并跳转到位于 `0x08008000` 的 APP。

## 主要功能

- 通过 SecureCRT 的 YMODEM 功能接收 APP BIN 固件。
- 处理文件信息包、128/1024 字节数据包及 ACK、NAK、CA 等控制字符。
- 根据固件大小擦除 APP 区域，并按 32 位字写入和回读校验。
- 校验 APP 主栈地址，切换 VTOR 和 MSP 后跳转到 APP 入口。
- USART1（PA9/PA10，115200）采用轮询方式收发数据。
- 提供 PA0 按键、PC13 LED、DWT/SysTick 延时等基础驱动。
- 通过 EasyLogger 和 SEGGER RTT 输出运行日志。

## Flash 分区

| 区域 | 起始地址 | 大小 | 地址范围 |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP | `0x08008000` | 480 KB | `0x08008000 ~ 0x0807FFFF` |

## 编译与升级

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx` 并编译。
2. 通过 J-Link/SWD 下载 Bootloader；全芯片擦除后需重新下载本工程。
3. 复位开发板，确认 RTT Viewer 输出 `Bootloader running...`。
4. 在 SecureCRT 中选择 **Transfer → Send Ymodem**，发送链接地址为
   `0x08008000` 的 APP BIN 文件。
5. 传输结束后，Bootloader 自动完成 Flash 写入并跳转到 APP。

## 主要目录

```text
Core/                    程序入口及 GPIO、Flash、USART
SYSTEM/delay/            DWT 与 SysTick 延时模块
Drivers/                 CMSIS 与 STM32F4 SPL
Middlewares/Ymodem/      YMODEM 接收及串口辅助代码
Middlewares/             EasyLogger 与 SEGGER RTT
Tasks/Bootloader/        APP 校验与跳转逻辑
Tasks/Debug/             EasyLogger 初始化
MDK-ARM/                 Keil 工程文件
```

## 验证状态

已完成硬件回归测试：YMODEM 文件传输无错误，固件能够写入 APP Flash
区域，Bootloader 能够成功跳转，APP 的 FreeRTOS 线程、LED 闪烁和 RTT
日志均正常运行。跳转后若 RTT Viewer 暂时没有显示 APP 日志，可断开并
重新连接 RTT Viewer。

当前实现仅用于接收固件，发送方向只使用 YMODEM 必需的控制字符；接收端
尚未对数据包末尾的 CRC16 进行计算和比对。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
