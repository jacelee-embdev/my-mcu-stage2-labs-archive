**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS、RTT 与 EasyLogger 开发模板

本工程已完成 FreeRTOS、SEGGER RTT 和 EasyLogger 移植，作为后续项目的开发基线。HAL 使用 TIM1 时基，`printf` 和 EasyLogger 日志通过 RTT 通道 0 输出。FreeRTOS 使用 CMSIS V2 完成内核初始化与启动，业务任务通过 FreeRTOS 原生接口创建。

## 任务组织

- 资源创建统一放在 `Z02_Business/Init/os_init.c` 和 `os_init.h`，由 `Core/Src/freertos.c` 调用 `OS_Init()`。默认的 `osThreadNew(...)` 创建语句已注释；后续任务及其他 RTOS 资源统一在 `OS_Init()` 中创建。
- 每个任务使用独立目录及配套 `.c/.h`。`Z02_Business/Status_Task` 用于测试 LED 闪烁和日志，通过 `Z01_BSP/LED` 控制 LED，每隔 500 个节拍翻转 LED 并输出状态日志；当前节拍频率为 1000 Hz，即间隔约 500 ms。
- 正式开发时，如不再需要状态测试任务，可删除其目录，并同步移除 `os_init.c` 中的包含、创建代码及 Keil 引用。

## 使用方式

使用 Keil MDK-ARM / ARMCC 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译下载后，通过 J-Link RTT Viewer 查看通道 0，确认 LED 周期翻转并输出状态日志。

当前基线已使用 ARMCC V5.06 update 7（build 960）编译通过，结果为 **0 错误、0 警告**，并已完成运行测试。

## CubeMX 重新生成代码

**每次重新配置前务必备份工程。** 当前配置使用 STM32CubeMX 6.16.1 和 STM32Cube FW_F4 V1.28.3。生成代码可能恢复或覆盖手动修改，完成后逐项检查：

1. 重新注释 `Core/Src/freertos.c` 中默认线程的 `osThreadNew(...)` 创建语句，保留 `os_init.h` 包含和 `OS_Init()` 调用。
2. 保留 HAL 的 TIM1 时基，以及 CMSIS V2 内核初始化与启动。
3. 确认 `main.c` 中的 `debug_init()`、`LED_Init()` 和 `usart.c` 中的 RTT `fputc()` 重定向保留；核对 RTT、EasyLogger、Z01 和 Z02 的 Keil 分组、源文件及头文件搜索路径。
4. FreeRTOS 内核使用 CubeMX 生成的版本，生成后无需覆盖回旧的内核目录。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
