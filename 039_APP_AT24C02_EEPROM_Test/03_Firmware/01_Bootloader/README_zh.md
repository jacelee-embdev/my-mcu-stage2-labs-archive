**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## 简介

本工程是基于 STM32F4 标准外设库（SPL）的 Bootloader，起始地址为
`0x08000000`。它通过 USART1 和 YMODEM 接收加密固件，将密文暂存到外部
W25Q64 Flash；接收完成后，从 W25Q64 读取、解密并写入 MCU 内部 Flash
的 APP 运行区（`0x08008000`），校验后跳转运行。未请求升级或安装失败时，
Bootloader 留在等待循环中，可通过 PA0 按键再次发起升级。

## 主要功能

- 通过 SecureCRT 的 YMODEM 功能接收加密固件，处理文件信息包、
  128/1024 字节数据包及 ACK、NAK、CA 等控制字符。
- 使用 SPI1 访问 W25Q64；接收时按 4 KB 扇区缓冲写入，传输结束后
  写入不足一个扇区的剩余数据。
- 校验加密包长度和解密出的 APP 大小，逐块解密到内部 APP 运行区，
  按 32 位字写入并回读校验。
- 校验 APP 初始栈地址，切换 VTOR 和 MSP 后跳转到 APP 入口。
- USART1 使用 PA9/PA10、115200 波特率和轮询收发；SPI1 使用
  PA5/PA6/PA7，PA4 为 W25Q 软件片选。
- 提供 PA0 按键、PC13 LED、DWT/SysTick 延时，以及 EasyLogger 和
  SEGGER RTT 日志。

## 存储布局

MCU 内部 Flash：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP 运行区 | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| 接收阶段擦除区 | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

加密固件实际从外部 W25Q64 的偏移地址 `0x000000` 开始顺序写入，
占用空间取决于加密文件长度。接收阶段会擦除 MCU 内部 Flash 的
Sector 5；固件密文存放在 W25Q64。

## 编译与升级

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx` 并编译，
   再通过 J-Link/SWD 下载 Bootloader。
2. 编译链接地址为 `0x08008000`、明文大小不超过 96 KB 的 APP BIN，
   使用与 Bootloader 匹配的密钥、IV 和加密包格式生成升级文件。
3. 复位时按住 PA0 升级键，在 SecureCRT 中选择
   **Transfer → Send Ymodem** 发送加密文件。
4. 接收完成后，Bootloader 从 W25Q64 读取密文、解密安装到 APP
   运行区；校验通过后跳转到 APP。
5. 未触发升级或安装失败时，Bootloader 保持运行并等待 PA0 再次触发。

## 当前实现说明

- 加密包首个 16 字节块包含 APP 明文大小，后续为 16 字节密文块。
  YMODEM 接收端目前不计算或比对数据包末尾的 CRC16。
- 接收文件信息包后，`ymodem.c` 调用
  `Flash_erase(APP_BACKUP_BASE_ADDR, size)` 擦除 MCU 内部 Flash
  对应扇区；`W25Q64_WriteData()` 将密文写入外部 W25Q64。
- `W25Q64_ReadData()` 依赖 RAM 中的读写索引，固件接收与安装需在
  同一次上电运行中完成。

## 主要目录

```text
Core/                            程序入口及 GPIO、Flash、USART、SPI1
BSP/W25QXX/                     W25Q 驱动与缓冲读写接口
SYSTEM/delay/                    DWT 与 SysTick 延时模块
Drivers/                         CMSIS 与 STM32F4 SPL
Middlewares_User/Ymodem/         YMODEM 接收及串口辅助代码
Middlewares_User/Algorithms/AES/ AES 解密
Middlewares_User/                EasyLogger 与 SEGGER RTT
Tasks/Bootloader/                固件解密安装、APP 跳转逻辑
Tasks/Debug/                     EasyLogger 初始化
MDK-ARM/                         Keil 工程文件
```

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
