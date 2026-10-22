**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS APP — W25Qxx 测试

## 简介

本工程基于 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS，通过 SPI1 集成 W25Qxx。
当前测试器件 ID 读取，同时保留 AT24C02 EEPROM 的软件 I2C 驱动及测试代码。

**作为 Bootloader 配套 APP 时，链接及烧录地址为 `0x08008000`。
本次独立测试为省去制作加密固件和通过上位机 YMODEM 发送的过程，
临时将 APP 起始地址改为 `0x08000000`，直接烧录明文固件。**

## 主要功能

- 初始化 HAL、100 MHz 系统时钟、GPIO、USART1、SPI1 和 FreeRTOS。
- 默认任务约每 2 秒翻转 PC13 LED，并调用 `W25Qx_Read_ID()`。
- PA5/PA6/PA7 分别为 SCK/MISO/MOSI，PA4 为软件片选；SPI1 使用模式 0、256 分频。
- 通过 EasyLogger/RTT 输出 `w25q_id`；W25Q64 的预期标识为 `EF16`。
- W25Qxx 驱动保留复位、参数获取、读取、分页写入和擦除接口，默认任务仅测试 ID。
- EEPROM 驱动仍使用 PB8/PB9；`ee_test()` 最后一页测试保留在 `#if 0` 中。

当前 `main.c` 在 `MX_GPIO_Init()` / `MX_SPI1_Init()` 之前调用 `W25Qx_Init()`，
且未检查返回值，因此不能据此确认启动时的复位及参数获取成功。
默认任务在外设初始化后读取 ID；当前测试不验证数据读写或擦除。

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
3. 按上述 SPI1 引脚连接 W25Q，通过 J-Link/SWD 直接下载明文固件，
   复位后在 RTT Viewer Terminal 0 查看 `w25q_id` 日志。
4. 恢复联合运行时，改回 `0x08008000`，重新编译不超过 96 KB 的 APP，
   重新下载 Bootloader，并通过 YMODEM 发送匹配的加密升级包。

## 主要目录

```text
Core/                    HAL、FreeRTOS、GPIO、USART、SPI 和应用入口
BSP/W25Qxx/              SPI Flash 驱动，默认任务测试器件 ID 读取
BSP/AT24Cxx/             保留的软件 I2C、EEPROM 驱动与最后一页测试
Tasks/Debug/             EasyLogger 初始化与格式配置
Middlewares_User/        EasyLogger 与 SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil 工程文件
```

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
