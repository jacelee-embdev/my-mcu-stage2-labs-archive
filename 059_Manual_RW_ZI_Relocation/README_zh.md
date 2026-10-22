**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 手动重定位实验

本工程基于 STM32F411CEU6 模板，绕过 `__main`，在 `main()` 开头手动复制 RW、清零 ZI，用于学习分散加载与启动流程。保留 HAL、SEGGER RTT、EasyLogger 和 FreeRTOS 框架。

## 基础配置

- Keil MDK 5 / ARM Compiler 5，启用 MicroLIB；Flash 从 `0x08000000` 开始，RAM 从 `0x20000000` 开始。
- HSI 经 PLL 倍频至 100 MHz；HAL 使用 TIM1 时基，FreeRTOS 节拍频率为 1000 Hz。

## 实验实现

- `MDK-ARM/startup_stm32f411xe.s` 导入 `main`，执行路径为 `Reset_Handler → SystemInit → main → RelocateData`。
- `Core/Src/main.c` 通过链接符号取得边界，以地址差计算字节数，调用 `MemoryCopy()`、`MemoryZero()` 初始化 RW/ZI。
- `g_test_rw` 初值为 `0x12345678`；`g_test_zi` 使用 ARMCC5 的 `zero_init` 属性明确放入 ZI。

## 调试验证

打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译并通过 J-Link/SWD 下载。在 Watch 中添加两个测试变量及其地址：

1. 在 `RelocateData()` 调用前暂停，将 `g_test_rw`、`g_test_zi` 分别改为 `0x11111111`、`0x22222222`。
2. 停在 `MemoryZero()` 调用前，确认 RW 已恢复为 `0x12345678`，ZI 仍为修改值。
3. 停在清零完成、`RelocateData()` 返回之前，确认 ZI 变为 `0`。地址以本次 MAP 和反汇编为准。

调试复位不会自动清空 SRAM，之前的结果可能保留；验证前手动改值，避免把残留数据当成初始化效果。

## 实验范围与维护

本实验绕过库启动，按未压缩的 RW 初值复制。现有 SCT 的 ZI 包含活动栈，清零可能影响函数返回；验证范围限于 RW 复制和 ZI 清零。

新增 C 代码位于 CubeMX 用户区。重新生成工程后，核对启动文件的 `IMPORT main` 与跳转目标，保留重定位调用及原有外设、日志和 RTOS 配置。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
