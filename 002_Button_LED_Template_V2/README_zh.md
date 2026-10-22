**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 按键控制 LED（自建模板 V2）

本工程记录在自建 HAL 模板中实现按键控制 LED 的过程，学习将应用入口、板级驱动和系统支持代码分开组织。主程序位于 `Users/main.c`，按键与 LED 封装在 `Drivers/BSP`，时钟、延时和串口基础驱动位于 `Drivers/SYSTEM`。

## 实现与现象

PA0 为上拉输入，按键按下时接地；PC13 为推挽输出。主循环调用 `key_is_pressed()` 检测低电平，满足条件后调用 `led_toggle()` 翻转 LED，再阻塞延时约一秒。持续按住会反复翻转，短按可能在延时期间被漏检；独立消抖与单次按下识别可作为后续扩展。

系统直接使用内部 HSI，时钟为 16 MHz。延时函数通过查询 SysTick 实现，并按 16 MHz 配置计数；修改时钟后应同步核对延时参数。当前入口只初始化时钟、LED 和按键，串口驱动作为基础模块保留。

## 编译与使用

1. 使用 Keil MDK 打开 [STM32F411CEU6.uvprojx](Projects/MDK-ARM/STM32F411CEU6.uvprojx)，采用 ARMCC 5.06 update 7 和 STM32F4xx_DFP 2.17.1。
2. 完整重新编译，通过 SWD 下载。将按键接至 PA0 与地之间，确认 PC13 LED 接线，再观察按住和短按时的响应。
3. 主要修改入口和 BSP 驱动即可继续扩展；`Output` 中保留当前 SCT，编译产物由重新构建生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
