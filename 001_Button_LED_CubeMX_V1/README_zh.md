**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 按键控制 LED（CubeMX V1）

本工程记录基于 STM32CubeMX 和 HAL 库进行按键控制 LED 的学习过程，涵盖 GPIO 输入输出、串口打印和主循环轮询。系统使用 HSI 经 PLL 配置为 100 MHz，保留 CubeMX 配置文件，便于对照图形化设置与初始化代码。

## 实验内容

PA0 配置为上拉输入，按键按下时接地；PC13 配置为推挽输出并连接 LED。主循环检测到按键为低电平时翻转 LED，随后延时约一秒。因此持续按住时，LED 会随循环反复翻转；短按可能在延时期间被漏检，程序未实现独立消抖和单次按下事件识别。

USART1 使用 PA9/TX、PA10/RX，参数为 115200、8N1。每轮通过重定向的 `printf` 输出 `TEST 1142!`，用于观察程序是否持续运行。

## 编译与运行

1. 使用 Keil MDK 打开 [STM32F411CEU6.uvprojx](MDK-ARM/STM32F411CEU6.uvprojx)，工程配置为 ARMCC 5.06 update 7，芯片包为 STM32F4xx_DFP 2.17.1。
2. 完整重新编译，通过 SWD 下载；按实际 LED 接法观察翻转，并用串口助手查看周期输出。串口模块接收端连接 PA9，并与开发板共地；LED 的亮灭电平取决于实际接线。
3. 主要逻辑位于 `Core/Src/main.c`，GPIO 与串口初始化分别位于 `gpio.c` 和 `usart.c`。重新生成 CubeMX 代码时，保留用户代码区中的按键逻辑和打印重定向。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
