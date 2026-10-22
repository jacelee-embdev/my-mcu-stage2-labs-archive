**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS 串口打印

本工程记录将按键与 LED 基础功能放入 FreeRTOS 线程运行的过程。工程由 STM32CubeMX 生成，使用 HAL 完成外设初始化，通过 CMSIS V2 初始化内核、创建线程并启动调度器，适合观察程序从初始化阶段进入线程循环的基本流程。

## 运行内容

`main.c` 初始化 GPIO 和 USART1 后调用 `MX_FREERTOS_Init()`；实际循环位于 `Core/Src/freertos.c` 的默认线程中。每轮读取按键，低电平时翻转 LED，再输出 `TEST!1142`，最后调用 `osDelay(1000)` 让出处理器。持续按住会随线程循环反复翻转，短按可能在等待期间被漏检。

USART1 采用 115200、8N1，通过串口重定向实现 `printf`。读取日志时，应将串口模块接收端连接到工程配置的 TX 引脚，并与开发板共地。

## 编译与观察

使用 Keil MDK 和 ARM Compiler 5 打开 [工程文件](MDK-ARM/STM32F411CEU6.uvprojx)，完整重新编译后通过 SWD 下载。观察串口周期输出和按键对应的 LED 变化；可在默认线程入口设置断点，查看调度器启动后的执行位置。扩展代码时保留初始化与线程创建入口，耗时等待优先使用线程延时接口。运行前核对按键有效电平、引脚定义和串口连接。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
