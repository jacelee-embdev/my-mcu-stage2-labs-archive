**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 DMA 内存到 GPIO 实验

## 项目说明

##### 项目简介

本项目使用 STM32F411 的 DMA 将内存数据搬运至 GPIOA 的 ODR，通过 PA6 波形学习 DMA 的轮询、中断及回调流程。

##### 主要功能

- 循环搬运 `0xFF` 和 `0x00`；
- 提供轮询与中断两种实现；
- 使用逻辑分析仪验证 PA6 输出。

##### 仓库结构

```text
Core/       应用代码与中断处理
Drivers/    HAL 与 CMSIS 驱动
MDK-ARM/    Keil 工程
*.ioc       CubeMX 配置
```

## 工程使用指南

##### 环境与依赖

STM32F411CEU6、STM32CubeMX 6.16.1、STM32Cube FW_F4 V1.28.3、Keil MDK 和逻辑分析仪。

##### 硬件连接与配置

逻辑分析仪连接 PA6，并与开发板共地。DMA2 Stream0 按字节向 ODR 搬运一个数据。

##### 编译、运行与观测

使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，编译、下载并复位。PA6 应连续交替输出高低电平。

##### 移植说明

更换芯片时，需在 CubeMX 中重新配置 GPIO、DMA Stream/Channel 和中断。

## 作者思考

1. 采用中断方式时，DMA 回调函数的注册: HAL_DMA_RegisterCallback 这个函数的使用，必须在DMA初始化:HAL_DMA_Init 之后，且必须在启动 DMA :HAL_DMA_Start_IT之前。

2. 在已被注释的方案一中，如果没有调用 HAL_DMA_PollForTransfer()，硬件可能已经完成传输，但HAL的DMA 状态还没有被更新为 ready。所以下一次 HAL_DMA_Start() 会返回 HALBUSY，延迟再久也不会更新 HAL 状态，**这可能会导致没有形成连续波形**。

3. <img src="Docs/Images/image-20260826165859959.png" style="zoom:33%;" />

   1.  为什么 `0xFF` 能给 `uint32_t` 类型的元素赋值？

      >0xFF 表示数值 255，并不代表它只能占一个字节。1**6 进制位数描述的是数值，不是变量的存储空间。**
      >
      >赋值给 uint32_t 后，数值会转变成 32 位存储，即 0xFF 会转换为 0x000000FF；而 0x00 会转换成 0x00000000。每个元素仍占 4 字节，只是高 24 位为 0

   2. 取地址得到的指针是什么类型？为什么需要强转？

      >得到的类型是 volatile uint32_t *，它指向的是一个 32 位的 uint32_t 元素。
      >
      >在 STM32F411 中，指针本身保存的地址也是 32 位。即使是 uint8_t * 的指针，本身也是 32 位，只是它指向的是 8 位数据；而我们这里的指针是 32 位的地址，并且指向的也是 32 位的数据。
      >
      >但是，HAL_DMA_Start_IT 的地址参数要**求的是一个 32 位的 uint32_t 整数地址，而不是指针**，所以我们需要对它进行强转。

   3. 传输长度 "1" 表示什么？是指一个字节吗？

      >长度以表示一个 DMA 传输单位，单位大小由 DMA 对齐配置决定：
      >• 如果配置为 Byte，则一个单位是一字节
      >• Half-Word 则为两字节
      >• Word 则为四字节
      >
      >这个配置在我们 CubeMX 当中配置 DMA 时有选择。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
