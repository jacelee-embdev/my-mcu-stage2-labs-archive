**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## 简介

本工程是基于 STM32F4 标准外设库（SPL）的 Bootloader，运行于
`0x08000000`。它通过 USART1 和 YMODEM 接收加密固件，将其写入 B 备份区，
解密后写入 A 运行区，并跳转到位于 `0x08008000` 的 APP。当前固件还启用了
SPI1 与 W25Q64 的循环擦写测试；未触发升级时会进入该测试循环。

## 主要功能

- 通过 SecureCRT 的 YMODEM 功能接收加密固件。
- 处理文件信息包、128/1024 字节数据包及 ACK、NAK、CA 等控制字符。
- 擦除 B 备份区，写入密文并回读校验。
- 校验包长及解密出的 APP 大小，逐块解密写入 A 运行区并回读校验。
- 校验 APP 主栈地址，切换 VTOR 和 MSP 后跳转到 APP 入口。
- USART1（PA9/PA10，115200）采用轮询方式收发数据。
- SPI1 使用 PA5/PA6/PA7，PA4 作为软件片选；支持 W25Q 标识读取、擦写及回读。
- 提供 PA0 按键、PC13 LED、DWT/SysTick 延时等基础驱动。
- 通过 EasyLogger 和 SEGGER RTT 输出运行日志。

## Flash 分区

| 区域 | 起始地址 | 大小 | 地址范围 |
|---|---:|---:|---:|
| Bootloader | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

上表为 MCU 内部 Flash。外部 W25Q 测试会擦除 `0x007FF000` 起始的 4 KB 扇区。

## 编译、测试与升级

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx` 并编译。
2. 通过 J-Link/SWD 下载 Bootloader；全芯片擦除后需重新下载本工程。
3. 复位并查看 RTT：W25Q64 ID 为 `61206`（`0xEF16`）；测试依次写入、擦除、
   再写入并读回。`write` 每轮加 2，匹配 `read` 时显示 `OK`，每轮后延时 1 秒。
4. 如需加密升级，编译链接地址为 `0x08008000`、不超过 96 KB 的 APP BIN，
   使用与 Bootloader 匹配的密钥、IV 和包格式生成加密文件。
5. 复位时按住 PA0 升级键，在 SecureCRT 中选择 **Transfer → Send Ymodem**
   发送加密文件。成功后 Bootloader 将其从 B 区解密安装到 A 区并跳转。

## 主要目录

```text
Core/                    程序入口及 GPIO、Flash、USART、SPI1
BSP/W25QXX/             W25Q 标识读取、擦写及容量参数
SYSTEM/delay/            DWT 与 SysTick 延时模块
Drivers/                 CMSIS 与 STM32F4 SPL
Middlewares_User/Ymodem/      YMODEM 接收及串口辅助代码
Middlewares_User/Algorithms/AES/  AES 解密
Middlewares_User/             EasyLogger 与 SEGGER RTT
Tasks/Bootloader/        固件解密安装、APP 跳转逻辑
Tasks/Debug/             EasyLogger 初始化
MDK-ARM/                 Keil 工程文件
```

## 协议说明

加密包首个 16 字节块包含 APP 明文大小，后续为按 16 字节分块的密文。
当前 YMODEM 接收端尚未计算和比对数据包末尾的 CRC16。
原回退循环由 `#if 0` 保留，W25Q 测试循环由 `#if 1` 启用。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
