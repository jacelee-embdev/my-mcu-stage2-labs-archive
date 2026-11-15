**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 裸机 IIC 驱动与 AHT21 连通性测试

基于 STM32 HAL，通过 GPIO 模拟 IIC，使用 DWT 周期计数器提供微秒、毫秒延时，并通过 SEGGER RTT + EasyLogger 输出 AHT21 地址 ACK 检测结果。原工程说明记录已完成硬件连通性测试；本次归档未重新编译、烧录或实机复测。

## 学习记录

本工程记录从 GPIO 电平操作到 IIC 通信原语的学习过程。总线对象保存 SDA、SCL 的端口与引脚信息，驱动封装起始、停止、字节收发及 ACK/NACK，并提供寄存器访问接口。当前主程序仅发送一次地址探测，不触发温湿度测量，也不做 CRC 校验；串口初始化并不代表日志从串口输出，观察结果应使用 RTT。连接前应按模块要求供电并确认电平兼容。ACK 只说明地址得到应答，不能证明测量数据或传感器精度正常。

## 基础配置

- HSI 经 PLL 倍频至 100 MHz；HAL 使用 SysTick，阻塞延时使用 DWT。
- SDA：PB13；SCL：PB14。AHT21 的 7 位地址为 `0x38`，发送的写地址字节为 `0x70`。
- USART1：PA9/TX、PA10/RX，115200 baud、8N1。
- `printf` 与 EasyLogger 共用 RTT 通道 0，采用同步输出，支持日志等级、颜色、时间和模块标签。

## 快速使用

使用 Keil MDK 5 / ARM Compiler 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并通过 SWD 下载。连接 J-Link，在 RTT Viewer 通道 0 查看日志。

启动时先初始化 DWT 和 GPIO，等待 100 ms 后发送一次设备写地址。收到 ACK 输出 `AHT21 ACK received.`，失败输出 `AHT21 did not acknowledge.`；主循环约每两秒输出示例日志。当前示例用于地址应答检测。

## 代码入口

- `Z01_SYS/delay.c`、`delay.h`：`Delay_Init`、`Delay_Us`、`Delay_Ms`；调用延时前须先初始化 DWT。
- `Z02_BSP/IIC/iic.c`、`iic.h`：总线结构体、GPIO 模拟 IIC 与寄存器读写接口。
- `Core/Src/main.c`：总线端口配置和 AHT21 连通性测试。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
