**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# 应用 AES 密钥与向量烧录工具

## 项目说明

##### 项目简介

本工程通过 I2C 向外部器件的用户 EEPROM 写入一组应用 AES-256 密钥和初始向量 IV，并读回校验。这组应用数据用于后续 AES 解密，与器件身份认证使用的密钥分开。

##### 主要功能

- 初始化芯片、读取序列号，完成认证及响应数据校验。
- 默认向第 0 页写入 16 字节 IV、32 字节密钥、CRC8、反码及 14 字节保留区，共 64 字节。
- 读回数据，检查 CRC 并比对整页内容，通过 RTT 输出结果。

##### 仓库结构

```text
Bsp/                    软件 I2C 与平台适配
Tasks/Key_Programmer/   烧录流程与密钥配置
Core/                   基础初始化与程序入口
MDK-ARM/               Keil 工程
```

## 工程使用指南

##### 环境与依赖

配套开发板与外设、工程驱动和静态库、Keil MDK 5（ARM Compiler 5）、J-Link 及 RTT Viewer。

##### 硬件连接与配置

通过 SWD 连接调试器；按 BSP 配置连接 I2C 引脚、共地并提供上拉。日志使用 RTT 通道 0。

密钥、IV、页号和密码在 [key_programmer_config.h](Tasks/Key_Programmer/key_programmer_config.h) 配置：`KEY_PROGRAMMER_IV_BYTES` 为 16 字节 IV，`KEY_PROGRAMMER_AES_KEY_BYTES` 为 32 字节密钥。默认 `KEY_PROGRAMMER_DIAGNOSTIC_MODE=0` 执行烧录；设为 1 仅做功能诊断，不写 EEPROM，也不能替代密钥的断电只读验证。

##### 编译、运行与观测

1. 打开 `MDK-ARM/` 中的 Keil 工程，编译下载并运行。
2. 启动时调用 `KeyProgrammer_Init()`、`KeyProgrammer_Run()`，初始化和认证通过后写入页 0，再读取同一页。
3. `Readback CRC and 64-byte comparison PASSED` 及最终 `SUCCESS` 表示本次 CRC、反码和完整 64 字节校验通过；最近上板日志已通过这些检查。

页内布局为：偏移 0～15 存 IV，16～47 存密钥，48 存 CRC8，49 存反码，50～63 清零。`Write PASSED` 仅表示写入阶段通过，最终读回校验才完成本次烧录验证。

每次启动都会再次尝试写入并覆盖页 0，复位后再次显示 `SUCCESS` 不能证明断电保存。本工程尚未提供独立的密钥只读保持验证入口；02 工程的范围写入测试也会覆盖此页。

##### 移植说明

更换平台时调整 GPIO、延时和接口适配，核对头文件搜索路径与静态库链接配置；外设需支持所用用户 EEPROM 接口。

## 作者思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
