**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS printf 并发输出

本工程记录多个 FreeRTOS 线程共享串口输出时的行为，用于理解调度切换、字符发送与日志完整性之间的关系。工程基于 STM32CubeMX 和 HAL，使用 CMSIS V2 启动内核，HAL 时间基准由 TIM1 提供。

## 实验实现

`Core/Src/freertos.c` 创建三个循环打印的线程，分别输出 `TEST!1142`、`TEST!1143` 和 `TEST!1144`。循环中的延时调用已注释，多个线程持续竞争 USART1 输出，可通过串口终端观察字符串是否交错。

打印重定向位于 `Core/Src/usart.c`。每次发送一个字符前调用 `vTaskSuspendAll()`，发送后调用 `xTaskResumeAll()`。这保护的是单次字符发送，不能据此认为一整次 `printf` 或整行日志已经获得互斥保护；需要整行一致性时，可进一步比较互斥锁或集中日志线程方案。

## 编译与观察

使用 Keil MDK、ARM Compiler 5 打开 [工程文件](MDK-ARM/STM32F411CEU6.uvprojx)，重新编译并下载，串口设置为 115200、8N1。对照三个线程和重定向函数检查输出，再观察加入线程延时后现象的变化。线程级调度接口应在线程上下文使用，避免从中断中直接调用该打印路径。保存串口输出，便于对照不同调度条件下的现象。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
