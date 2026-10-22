**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS 按键短按与长按

本工程记录在 FreeRTOS 中识别按键短按和长按，并通过消息队列控制 LED 的过程。工程将按键检测、事件转换和灯的执行分别放在线程及 BSP 模块中，便于从简单输入输出进一步学习状态检测和异步通信。

## 功能与组织

短按使 LED 状态翻转，长按使 LED 闪烁三次。按键线程扫描输入，完成消抖与按压时长判断，再将识别出的事件写入按键队列。默认线程接收事件，将其映射为 LED 命令并发送到灯的队列，由 LED 线程执行实际操作。

按键接口、状态和事件定义位于 `BSP/key/inc/bsp_key.h`，检测实现位于 `BSP/key/src/bsp_key.c`。LED 接口与命令处理位于 `BSP/led`，线程创建和消息转换位于 `Core/Src/freertos.c`。按键模块生成事件，灯模块处理命令，修改功能时应保持两侧消息类型一致。

## 编译与观察

使用 Keil MDK、ARM Compiler 5 打开 [工程文件](MDK-ARM/STM32F411CEU6.uvprojx)，完整重新编译并下载。根据 GPIO 配置连接按键与 LED，分别尝试短按和持续按住，观察翻转与三次闪烁。检测阈值和扫描节奏以按键驱动为准；调整时同时检查释放处理、重复触发和队列收发结果，避免将不同按压过程误判为同一事件。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
