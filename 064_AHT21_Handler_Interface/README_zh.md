**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 AHT21 温湿度 Handler

本阶段在已有 AHT21 Driver 基线上定义 Handler 层接口。Driver 负责通过软件 I²C 触发测量、校验 CRC，并将原始数据换算为温度和相对湿度。Handler 的设计是由调用方提交测量事件，再由工作线程从队列取出并处理。当前交付为接口声明和已纳入 Keil 工程的源文件骨架，队列处理、回调和应用接入仍待实现。

## 学习记录

本工程用于记录异步请求处理层的接口设计，重点区分同步 Driver 与上层 Handler 的职责。事件结构描述希望读取的物理量、数据有效时长及结果交付方式，平台接口表则预留队列与调度相关依赖。当前这些属于设计契约，不能把头文件中的描述视为已经运行的队列流程。编译入口保留已有 Driver 采样基线，用于继续观察温湿度日志；若要验证 Handler，仍需完成事件处理、缓存更新、工作线程及业务接入。连接前应核对传感器模块供电与电平。

## Handler 接口

- `bsp_temp_humi_handler_inst()` 用于绑定 I²C、毫秒时基、OS 队列和 RTOS 延时接口，为 Driver 和事件队列的初始化提供入口。
- `bsp_temp_humi_read()` 用于提交测量事件。事件可请求温度、相对湿度或两者，并携带读数的有效时长、事件时间戳、输出指针和回调。接口返回的是请求提交状态。
- `temp_humi_handler_thread()` 是处理队列请求的线程入口。工作线程根据请求类型和允许的读数时长决定是否重新采样。

接口声明见 [sw_bsp_aht21_handler.h](Z02_BSP/AHT21/sw_bsp_aht21_handler/Inc/sw_bsp_aht21_handler.h)，实现预留在 [sw_bsp_aht21_handler.c](Z02_BSP/AHT21/sw_bsp_aht21_handler/Src/sw_bsp_aht21_handler.c)。

## 接线与 Driver 基线

SDA 接 PB13，SCL 接 PB14，传感器与开发板共地，AHT21 的 7 位地址为 `0x38`。使用 Keil MDK 5 和 ARM Compiler 5 打开 [STM32F411CEU6.uvprojx](MDK-ARM/STM32F411CEU6.uvprojx)，编译后通过 SWD 下载。

现有 FreeRTOS 采样任务由 `os_init.c` 注册。任务绑定 GPIO 软件 I²C 和 FreeRTOS 时基与延时回调，直接调用 Driver，每次读取后等待 2 秒。连接 J-Link 后，可在 RTT Viewer 通道 0 查看该任务的 EasyLogger 输出。采样入口见 [aht21_task.c](Z03_Business/AHT21_Task/aht21_task.c)。

## 归档验证说明

本文保留原工程的学习阶段与既有运行记录。本次归档仅核对代码、工程引用、文档链接和保留文件，未重新编译、烧录或进行硬件测试；文中的实机结果属于导入工程的历史记录。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
