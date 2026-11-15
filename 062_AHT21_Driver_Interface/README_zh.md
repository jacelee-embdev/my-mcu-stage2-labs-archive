**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AHT21 连通性测试与 BSP 接口

工程通过 GPIO 模拟 IIC 检测 AHT21 地址应答，使用 DWT 延时，通过 SEGGER RTT 和 EasyLogger 输出日志。原工程说明记录已验证硬件连通性；本次归档未重新编译、烧录或实机复测。

## 学习记录

本工程保存 AHT21 驱动设计的接口阶段，重点是用结构体与函数指针描述总线、时基和可选 RTOS 延时依赖，为后续协议实现留下边界。头文件中的测量接口属于声明，源文件尚未提供完整协议处理，不能据此认为已经具备温湿度采集功能。运行入口继续沿用裸机地址应答示例，便于将硬件通信问题与驱动实现问题分开检查。后续实现需补充测量等待、数据帧解析与校验；这些不作为本阶段已完成功能。

## 基础配置

- HSI 经 PLL 倍频至 100 MHz，HAL 使用 SysTick，阻塞延时使用 DWT。
- SDA 为 PB13，SCL 为 PB14。AHT21 的 7 位地址为 `0x38`，写地址字节为 `0x70`。
- USART1 使用 PA9/TX、PA10/RX，115200 baud、8N1。`printf` 与 EasyLogger 共用 RTT 通道 0。

## 快速使用

用 Keil MDK 5 和 ARM Compiler 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译后通过 SWD 下载。连接 J-Link，在 RTT Viewer 的通道 0 查看日志。

启动后等待 100 ms，再发送 AHT21 写地址。应答成功输出 `AHT21 ACK received.`，失败输出 `AHT21 did not acknowledge.`。主循环约每两秒输出一次示例日志。

## 代码入口

- `Core/Src/main.c` 运行地址应答测试，底层使用 `Z02_BSP/IIC` 和 `Z01_SYS/delay.c`。
- `Z02_BSP/AHT21/sw_bsp_aht21_driver` 下的 `sw_bsp_aht21_driver.h` 声明 IIC、毫秒时基和可选 RTOS 延时回调，以及温度、湿度和联合读取接口。
- `sw_bsp_aht21_register.h` 定义地址字节、测量指令及参数。`sw_bsp_aht21_driver.c` 提供待实现的源文件框架。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
