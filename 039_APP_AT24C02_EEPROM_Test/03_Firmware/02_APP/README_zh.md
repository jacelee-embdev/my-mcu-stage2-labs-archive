**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS APP — AT24C02 测试

## 简介

本工程基于 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS，通过软件 I2C 集成 AT24C02。

**作为 Bootloader 配套 APP 时，链接及烧录地址为 `0x08008000`。
本次独立测试为省去制作加密固件和通过上位机 YMODEM 发送的过程，
临时将 APP 起始地址改为 `0x08000000`，直接烧录明文固件。**

## 主要功能

- 初始化 HAL、100 MHz 系统时钟、GPIO、USART1 和 FreeRTOS。
- 默认任务约每 2 秒翻转 PC13 LED，并调用一次 `ee_test()`。
- PB8 为 SCL、PB9 为 SDA；检测设备 ACK 并打印地址 `0x50` / `0xA0`。
- AT24C02 容量为 256 字节、每页 8 字节；测试仅覆盖最后一页 `0xF8～0xFF`。
- 每轮写入下一组递增数据，轮询 ACK 后读回比较，通过 EasyLogger/RTT 输出结果。

实测设备应答正常，数据随轮次递增；一轮写入 `0x18～0x1F`，8 个字节全部
读回一致，输出 `EEPROM read/write test successful.`。器件不支持读取芯片 ID。

## Flash 分区

| 运行方式 | 链接及烧录地址 | 当前配置大小 |
|---|---:|---:|
| Bootloader 配套 APP | `0x08008000` | `0x18000`（96 KB） |
| 独立测试（当前） | `0x08000000` | `0x18000`（96 KB） |

独立测试占用原 Bootloader 区域。联合运行时，Bootloader 将加密包解密安装到
`0x08008000`，跳转前设置 `SCB->VTOR` 和 MSP，APP 在启动时重新开启中断。

## 编译与下载

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`。
2. 独立测试时确认 IROM1 起始地址为 `0x08000000`，重新编译 APP。
3. 通过 J-Link/SWD 直接下载明文固件，复位后在 RTT Viewer Terminal 0 查看日志。
4. 恢复联合运行时，改回 `0x08008000`，重新编译不超过 96 KB 的 APP，
   重新下载 Bootloader，并通过 YMODEM 发送匹配的加密升级包。

## 主要目录

```text
Core/                    HAL、FreeRTOS、GPIO、USART 和应用入口
BSP/AT24Cxx/             软件 I2C、EEPROM 驱动与最后一页测试
Tasks/Debug/             EasyLogger 初始化与格式配置
Middlewares_User/        EasyLogger 与 SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil 工程文件
```

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
