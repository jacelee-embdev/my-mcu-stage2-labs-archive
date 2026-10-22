**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 函数调用栈与 HardFault 实验

## 项目说明

基于 STM32F411CEU6 的两个独立实验，用于学习函数调用、参数传递、
局部变量栈布局，以及非法访问后的 HardFault 观测。

##### 实验目录

| 子工程 | 实验内容 | 使用说明 |
|---|---|---|
| `01_FunctionCalls` | 三级嵌套调用，观察参数、局部变量、SP 和 LR | [中文 README](01_FunctionCalls/README_zh.md) |
| `02_Hardfaulthandler` | 向保留地址写入，观察 HardFault 与故障寄存器 | [中文 README](02_Hardfaulthandler/README_zh.md) |

##### 仓库结构

```text
01_FunctionCalls/     函数调用与栈观测工程
02_Hardfaulthandler/  HardFault 非法写入工程
LICENSE              总工程许可证
README_zh.md         中文说明
README.md            英文说明
```

每个子工程包含独立的 `Business`、`Core`、`Drivers`、`MDK-ARM` 和
`Middlewares_User` 目录，可分别编译、下载和调试。

## 工程使用指南

##### 环境与连接

- STM32F411CEU6 开发板，系统时钟 100 MHz。
- Keil MDK 5，ARMCC 5.06 update 7；HAL、CMSIS、SEGGER RTT 和 EasyLogger 随工程提供。
- 调试器通过 SWD 连接开发板，连接参考电压与公共地。
- 可选串口：PA9 → 转串口模块 RX、PA10 → TX、GND → GND；3.3 V TTL，115200、8N1。

##### 编译、运行与观测

1. 选择实验，打开对应子目录的 `MDK-ARM/STM32F411CEU6.uvprojx`。
2. 使用 `-O0` 并启用调试信息，编译后下载到开发板。
3. 按子工程 README 设置断点，复位运行并观察；两个实验均在启动时执行一次。

##### 移植说明

各实验逻辑位于 `Business`，由 `main` 在初始化后调用。
换芯片时调整时钟、外设与调试配置；HardFault 实验还需重新核对故障地址。
CubeMX 生成文件中的用户逻辑放在 `USER CODE` 区域。

## 作者思考

无

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
