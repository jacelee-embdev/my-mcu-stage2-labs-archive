**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 函数调用与栈观测实验

## 项目说明

基于 STM32F411CEU6，通过三级嵌套函数调用，观察参数传递、局部变量、
栈指针 SP 和返回地址 LR，以及函数返回时的栈变化。

调用链为 `main → CallStackDemo_EnterLevel1 → CallStackDemo_EnterLevel2 → CallStackDemo_EnterLevel3`。
各级局部变量使用固定数值标记，并通过 `volatile` 和禁止内联属性保留观测对象。

##### 工程结构

```text
Business/          函数调用栈演示模块
Core/              main 入口、时钟和外设配置
Drivers/           HAL 与 CMSIS
MDK-ARM/           Keil 工程与启动文件
Middlewares_User/  SEGGER RTT 与 EasyLogger
```

## 工程使用指南

##### 环境与连接

- STM32F411CEU6 开发板，系统时钟 100 MHz。
- Keil MDK 5，ARMCC 5.06 update 7；HAL 与 CMSIS 源码随工程提供。
- 通过 SWD 接口连接调试器，并连接参考电压与公共地。
- 可选串口：PA9 → 转串口模块 RX、PA10 → TX、GND → GND；
  使用 3.3 V TTL 电平，参数为 115200、8N1。

##### 编译、运行与观测

1. 打开 [Keil 工程](MDK-ARM/STM32F411CEU6.uvprojx)，使用 `-O0` 优化等级并启用调试信息，编译、下载后进入调试。
2. 在 `CallStackDemo_EnterLevel3()` 的 `level3_marker = level3_marker;` 处设置断点，复位后运行。
3. 暂停后查看 Call Stack、Registers 和 Memory，切换调用层级观察参数与局部变量。
4. 单步返回各级函数，观察 SP、LR 和调用栈变化；返回 `main()` 后，主函数标记更新为 `0x00456789`。

| 观测对象 | 标记值 |
|---|---|
| `main_stack_marker`（调用前） | `0x12345678` |
| `level1_marker` | `0xA1A2A3A4` |
| `level2_marker` | `0xB1B2B3B4` |
| `level3_marker` | `0xC1C2C3C4` |

参数可能位于寄存器或栈中，具体位置以反汇编为准。
程序启动时执行一次调用测试，随后每 2 秒输出调试消息；重新测试需复位。

##### 移植说明

移植演示模块时，将 `Business/call_stack_demo.c` 加入目标工程，并配置头文件路径。
根据目标芯片调整时钟、串口和调试配置，并确认编译器支持禁止内联属性。
修改 CubeMX 生成的 `main.c` 时，将用户逻辑放在 `USER CODE` 区域。

## 作者思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
