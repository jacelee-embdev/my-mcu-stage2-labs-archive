**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AHT21 温湿度驱动

本工程在 STM32F411CEU6 上实现 AHT21 驱动和 FreeRTOS 采样任务。驱动通过软件 I²C 触发测量，等待 Busy 位清零后读取 7 字节并校验 CRC，再将同一帧中的两个 20 位原始值换算为温度和相对湿度。

## 学习记录

本工程记录从接口定义到可运行传感器协议驱动的实现过程。业务线程向 Driver 注入 GPIO 软件 IIC、毫秒时基及 RTOS 延时回调，驱动负责初始化、测量等待、通信状态检查、CRC-8 校验和原始值换算。温湿度来自同一数据帧，成功时一起打印，失败时输出状态码；读取后再等待两秒，因此完整采样周期还包括测量耗时。本阶段直接调用同步 Driver，尚未使用 Handler 队列、缓存或异步回调。连接前应核对模块供电、通信电平和共地条件。

## 接线与运行

- SDA 接 PB13，SCL 接 PB14，传感器与开发板共地。AHT21 的 7 位地址为 `0x38`。
- 用 Keil MDK 5 和 ARM Compiler 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译后通过 SWD 下载。
- 连接 J-Link，在 RTT Viewer 通道 0 查看 EasyLogger 日志。任务每次读取后等待 2 秒，已在实板验证连续输出。

## 代码入口

- `os_init.c` 注册 `aht21_task`。任务绑定软件 I²C 总线和 FreeRTOS 回调，初始化传感器并打印读数。
- `sw_bsp_aht21_driver.c/.h` 提供构造、状态查询和温湿度联合读取。驱动头文件启用 `OS_SUPPORTING`，`sw_bsp_aht21_register.h` 定义地址和测量命令。
- `Z02_BSP/IIC` 提供 GPIO 软件 I²C 接口。驱动的反初始化、睡眠和唤醒为占位接口。

## 归档验证说明

本文保留原工程的学习阶段与既有运行记录。本次归档仅核对代码、工程引用、文档链接和保留文件，未重新编译、烧录或进行硬件测试；文中的实机结果属于导入工程的历史记录。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
