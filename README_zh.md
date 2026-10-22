**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# MCU 嵌入式学习第二阶段工程归档

本仓库归档了我在嵌入式学习第二阶段的一些工程，涵盖 FreeRTOS、DMA、Bootloader、OTA 升级、嵌入式调试等内容。

## 归档说明

本仓库保存嵌入式学习第二阶段积累的工程和笔记，用于个人学习记录、代码备份，以及后续回顾和复现实验。

学习内容包括任务调度与同步、数据传输与缓冲、固件升级、加密、故障诊断、链接脚本等。工程按学习顺序编号，保留对应学习阶段的实现方式和实验状态。

各实验分别打开和编译，部分实验包含 Bootloader 与 APP 等相互配合的固件工程。具体硬件、开发环境、编译顺序和预期现象，请以对应实验的说明为准。

## 工程索引

单击工程名称可查看对应的中文说明。

| 编号 | 工程 | 实现内容 |
|---:|---|---|
| 001 | [Button_LED_CubeMX_V1](001_Button_LED_CubeMX_V1/README_zh.md) | 在 CubeMX 工程中轮询按键并翻转 LED，串口周期输出测试信息。 |
| 002 | [Button_LED_Template_V2](002_Button_LED_Template_V2/README_zh.md) | 基于自建 HAL 模板封装按键和 LED 驱动，通过主循环轮询控制 LED 翻转。 |
| 003 | [FreeRTOS_UART_Printf](003_FreeRTOS_UART_Printf/README_zh.md) | FreeRTOS 默认线程轮询按键、翻转 LED，并通过 UART 输出测试信息。 |
| 004 | [FreeRTOS_Printf_Thread_Safety](004_FreeRTOS_Printf_Thread_Safety/README_zh.md) | 通过多个线程并发打印，观察字符级调度保护与整行输出的差异。 |
| 005 | [Keil_Debug_Basics](005_Keil_Debug_Basics/README_zh.md) | 使用周期串口输出练习 Keil 断点、单步及变量观察。 |
| 006 | [FreeRTOS_Queue_LED_Control](006_FreeRTOS_Queue_LED_Control/README_zh.md) | 通过按键队列、中继线程和 LED 队列解耦按键与灯控制。 |
| 007 | [FreeRTOS_Button_Short_Long_Press](007_FreeRTOS_Button_Short_Long_Press/README_zh.md) | 通过 FreeRTOS 线程和队列实现短按翻转、长按闪烁。 |
| 008 | [FreeRTOS_Software_Interrupt](008_FreeRTOS_Software_Interrupt/README_zh.md) | 通过 NVIC 软件挂起中断，并在线程上下文输出测试日志。 |
| 009 | [FreeRTOS_Button_EXTI_V1](009_FreeRTOS_Button_EXTI_V1/README_zh.md) | 使用 EXTI 边沿事件与线程状态机识别单个按键的短按、长按。 |
| 010 | [FreeRTOS_Button_EXTI_V2](010_FreeRTOS_Button_EXTI_V2/README_zh.md) | 为两个按键维护独立 EXTI 状态机，并通过队列控制 LED。 |
| 011 | [FreeRTOS_Button_Timer_LED_Control](011_FreeRTOS_Button_Timer_LED_Control/README_zh.md) | 将按键事件映射为 LED 命令，并通过 TIM2 中断驱动翻转。 |
| 012 | [FreeRTOS_Interrupt_Response_Comparison](012_FreeRTOS_Interrupt_Response_Comparison/README_zh.md) | 比较队列按值复制与传递指针对中断响应路径的影响。 |
| 013 | [SEGGER_RTT_Demo](013_SEGGER_RTT_Demo/README_zh.md) | 移植 SEGGER RTT，使用 RTT 接口和标准 printf 输出调试信息。 |
| 014 | [EasyLogger_UART_Port](014_EasyLogger_UART_Port/README_zh.md) | 移植 EasyLogger，通过 UART 输出分级彩色日志。 |
| 015 | [RTT_EasyLogger_Template_V1](015_RTT_EasyLogger_Template_V1/README_zh.md) | 组合 RTT 和 EasyLogger，形成裸机调试输出模板。 |
| 016 | [FreeRTOS_RTT_EasyLogger_Template_V2](016_FreeRTOS_RTT_EasyLogger_Template_V2/README_zh.md) | 集成 FreeRTOS、RTT 和 EasyLogger，统一线程与日志入口。 |
| 017 | [UART_Interrupt_DMA_RX_Comparison](017_UART_Interrupt_DMA_RX_Comparison/README_zh.md) | 比较 UART 中断与 DMA 接收的 CPU 开销及业务耗时。 |
| 018 | [DMA_Memory_To_GPIO](018_DMA_Memory_To_GPIO/README_zh.md) | 通过 DMA 将内存数据写入 GPIO，并观察输出波形。 |
| 019 | [UART_DMA_RX_To_SRAM](019_UART_DMA_RX_To_SRAM/README_zh.md) | 将 UART 数据通过 DMA 接收到 SRAM，并在回调中重新启动接收。 |
| 020 | [UART_DMA_FIFO_Burst](020_UART_DMA_FIFO_Burst/README_zh.md) | 观察 UART DMA 的 FIFO、Burst 和缓冲区传输行为。 |
| 021 | [PWM_DMA_WT588F02B_Control](021_PWM_DMA_WT588F02B_Control/README_zh.md) | 使用 PWM 与 DMA 生成 WT588F02B 语音模块的控制波形。 |
| 022 | [FreeRTOS_ADC_DMA_Double_Buffer](022_FreeRTOS_ADC_DMA_Double_Buffer/README_zh.md) | 在 FreeRTOS 中使用 ADC DMA 双缓冲采集与处理数据。 |
| 023 | [FreeRTOS_ADC_DMA_Double_Buffer_Mutex](023_FreeRTOS_ADC_DMA_Double_Buffer_Mutex/README_zh.md) | 为 ADC DMA 双缓冲处理加入互斥锁，协调缓冲区访问。 |
| 024 | [UART_Interrupt_Double_Buffer_Parser](024_UART_Interrupt_Double_Buffer_Parser/README_zh.md) | 通过 UART 中断和双缓冲接收定长数据并解析帧。 |
| 025 | [UART_DMA_Fixed_Length_Double_Buffer_Parser](025_UART_DMA_Fixed_Length_Double_Buffer_Parser/README_zh.md) | 使用 UART DMA 定长双缓冲接收并解析数据帧。 |
| 026 | [UART_DMA_HT_TC_Idle_Test](026_UART_DMA_HT_TC_Idle_Test/README_zh.md) | 测试 UART DMA 的半传输、全传输与空闲事件。 |
| 027 | [UART_DMA_Idle_Frame_Parser](027_UART_DMA_Idle_Frame_Parser/README_zh.md) | 使用 UART DMA 循环接收与空闲检测解析变长数据帧。 |
| 028 | [HAL_Bootloader_APP_Jump](028_HAL_Bootloader_APP_Jump/README_zh.md) | 在 HAL Bootloader 与 APP 之间验证启动跳转。 |
| 029 | [SPL_RTT_EasyLogger_Template](029_SPL_RTT_EasyLogger_Template/README_zh.md) | 搭建基于 SPL、RTT 和 EasyLogger 的基础工程。 |
| 030 | [SPL_Bootloader_APP_Jump](030_SPL_Bootloader_APP_Jump/README_zh.md) | 验证 SPL Bootloader 向独立 APP 的基本跳转。 |
| 031 | [Bootloader_Peripheral_Drivers](031_Bootloader_Peripheral_Drivers/README_zh.md) | 集成 Bootloader 所需的串口、按键及状态指示驱动。 |
| 032 | [Bootloader_YMODEM_Test](032_Bootloader_YMODEM_Test/README_zh.md) | 在 Bootloader 中测试 YMODEM 文件传输协议。 |
| 033 | [Bootloader_YMODEM_APP_Install](033_Bootloader_YMODEM_APP_Install/README_zh.md) | 通过 YMODEM 接收 APP，写入内部 Flash 并跳转运行。 |
| 034 | [Bootloader_Internal_Flash_Staging](034_Bootloader_Internal_Flash_Staging/README_zh.md) | 将新固件暂存于内部 Flash，再复制到执行分区并启动。 |
| 035 | [AES_Encryption_Decryption_Test](035_AES_Encryption_Decryption_Test/README_zh.md) | 验证 AES 加密、解密及数据往返一致性。 |
| 036 | [Bootloader_AES_Firmware_Upgrade](036_Bootloader_AES_Firmware_Upgrade/README_zh.md) | 在 Bootloader 中接收、解密并安装加密固件。 |
| 037 | [Bootloader_W25Q64_Driver_Test](037_Bootloader_W25Q64_Driver_Test/README_zh.md) | 移植 W25Q64 驱动，验证外部 Flash 读写与擦除。 |
| 038 | [Bootloader_External_Flash_Upgrade](038_Bootloader_External_Flash_Upgrade/README_zh.md) | 将固件暂存迁移到外部 Flash，完成解密安装与启动。 |
| 039 | [APP_AT24C02_EEPROM_Test](039_APP_AT24C02_EEPROM_Test/README_zh.md) | 在 APP 中集成 AT24C02 EEPROM 并测试读写。 |
| 040 | [APP_W25Q_Flash_Test](040_APP_W25Q_Flash_Test/README_zh.md) | 在 APP 中集成 W25Q 外部 Flash 并测试访问。 |
| 041 | [APP_YMODEM_Receive_Test](041_APP_YMODEM_Receive_Test/README_zh.md) | 在 APP 中集成 YMODEM，验证数据包接收流程。 |
| 042 | [APP_OTA_Workflow_Test](042_APP_OTA_Workflow_Test/README_zh.md) | 验证 APP 侧固件接收、状态记录与复位升级流程。 |
| 043 | [Bootloader_AT24C02_EEPROM_Test](043_Bootloader_AT24C02_EEPROM_Test/README_zh.md) | 在 Bootloader 中集成并验证 AT24C02 EEPROM 驱动。 |
| 044 | [Encrypted_OTA_Upgrade](044_Encrypted_OTA_Upgrade/README_zh.md) | 组合 Bootloader、APP1 和 APP2，完成加密 OTA 与备份恢复。 |
| 045 | [Bootloader_Compiler_Optimization](045_Bootloader_Compiler_Optimization/README_zh.md) | 利用构建分析工具比较编译优化等级对 Bootloader 的影响。 |
| 046 | [OTA_YMODEM_Frame_CRC](046_OTA_YMODEM_Frame_CRC/README_zh.md) | 为加密 OTA 的 YMODEM 数据帧加入 CRC-16/XMODEM 校验。 |
| 047 | [OTA_IWDG_Boot_Confirmation](047_OTA_IWDG_Boot_Confirmation/README_zh.md) | 通过 IWDG 和首次启动确认验证新固件及失败恢复。 |
| 048 | [Bootloader_Early_APP_Jump](048_Bootloader_Early_APP_Jump/README_zh.md) | 通过保留 RAM 请求与软件复位提前跳转 APP。 |
| 049 | [OTA_SFUD_Flash_Integration](049_OTA_SFUD_Flash_Integration/README_zh.md) | 移植 SFUD，减少 OTA 业务对具体 Flash 驱动的依赖。 |
| 050 | [Bootloader_Resource_Optimization](050_Bootloader_Resource_Optimization/README_zh.md) | 裁剪 EasyLogger、SFUD 等功能，降低 Bootloader 资源占用。 |
| 051 | [Hardware_Crypto_Chip_Test](051_Hardware_Crypto_Chip_Test/README_zh.md) | 测试硬件加密芯片的密钥写入、认证、加解密和存储访问。 |
| 052 | [Encrypted_OTA_Integrated_Project](052_Encrypted_OTA_Integrated_Project/README_zh.md) | 集成并整理完整的加密固件升级、备份和恢复工程。 |
| 053 | [Function_Call_Stack_HardFault_Test](053_Function_Call_Stack_HardFault_Test/README_zh.md) | 通过两个独立实验观察函数调用栈和 HardFault 现场。 |
| 054 | [FreeRTOS_CmBacktrace_Template](054_FreeRTOS_CmBacktrace_Template/README_zh.md) | 集成 CmBacktrace、FreeRTOS、RTT 与日志，建立故障诊断模板。 |
| 055 | [CmBacktrace_Divide_By_Zero_Test](055_CmBacktrace_Divide_By_Zero_Test/README_zh.md) | 触发除零异常，通过 CmBacktrace 输出现场并定位源码。 |
| 056 | [SCT_Flash_SRAM_Execution_Test](056_SCT_Flash_SRAM_Execution_Test/README_zh.md) | 通过 SCT 将函数放入不同 Flash 区域及 SRAM，检查布局与耗时。 |
| 057 | [Ozone_HardFault_Debug_Test](057_Ozone_HardFault_Debug_Test/README_zh.md) | 复现故障场景，使用 Ozone 或 Keil 观察异常现场。 |

## 使用说明

1. 克隆或下载本仓库，根据工程索引选择实验。
2. 阅读对应实验的 `README_zh.md`，核对 MCU、硬件接线、所需组件和开发环境。
3. 使用 Keil MDK 打开相应的 `.uvprojx` 文件，并安装该工程所需的编译器和芯片支持包。
4. 完整重新编译工程，生成固件。涉及多个配套固件的实验，按其说明中的顺序编译和烧录。
5. 使用工程配置的调试器烧录固件，检查串口、LED 或其他说明中列出的实验现象。

可重新生成的编译产物不纳入版本管理。单独使用某个实验时，请一并保留其源代码、工程配置、`LICENSE` 和 `.gitignore`。不同实验的硬件和工具链配置可能有所差异，请结合对应说明与源代码进行核对。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
