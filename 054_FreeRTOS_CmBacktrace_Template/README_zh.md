**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS 与 CmBacktrace 开发模板

本工程已完成 FreeRTOS、CmBacktrace、SEGGER RTT 和 EasyLogger 移植，作为后续项目的开发基线。HAL 使用 TIM1 时基，`printf`、日志及故障信息通过 RTT 通道 0 输出，支持 HardFault 诊断与任务栈回溯。

## 任务组织

- 为使用更熟悉的 FreeRTOS 原生接口，将资源创建独立到 `Z02_Business/Init/os_init.c` 和 `os_init.h`，由 `freertos.c` 调用 `OS_Init()`，替代默认的 CMSIS 风格任务创建；后续任务及其他 RTOS 资源统一在此创建。
- 每个任务使用独立目录及配套 `.c/.h`。`Z02_Business/Status_Task` 仅测试 LED 闪烁和日志，正式开发时可删除，并同步移除 `os_init.c` 中的包含、创建代码及 Keil 引用。

## 使用方式

使用 Keil MDK-ARM / ARMCC 5 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译下载后，通过 J-Link RTT Viewer 查看通道 0。

## CubeMX 重新生成代码

**每次重新配置前务必备份工程。** 生成代码可能恢复或覆盖手动修改，完成后逐项处理：

1. 重新注释 `Core/Src/freertos.c` 中默认线程的 `osThreadNew(...)` 创建语句，保留 `OS_Init()` 调用。
2. 重新注释 `Core/Src/stm32f4xx_it.c` 中的 `HardFault_Handler()` 实现，由 `cmb_fault.S` 接管异常入口。
3. 提前备份 `Middlewares/Third_Party/FreeRTOS` 整个目录。同一 FreeRTOS 版本下，生成后将备份目录整体覆盖回原路径，恢复 CmBacktrace 的栈深度适配，包括 `Source/tasks.c` 和 `Source/include/FreeRTOS.h` 中的修改。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
