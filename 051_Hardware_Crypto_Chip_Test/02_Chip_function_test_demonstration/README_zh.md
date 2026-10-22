**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# 外设功能测试演示工程

## 项目说明

##### 项目简介

本工程在裸机环境下验证外设通信、认证和数据处理功能，通过 RTT 输出测试结果。

##### 主要功能

- 软件 I2C 通信与外设初始化。
- 认证、加解密回环及存储用户区写读／只读比对。
- 记录底层错误，区分通过、失败、跳过、未确认及库范围限制。

##### 仓库结构

```text
Core/               基础初始化与程序入口
Drivers/            平台驱动依赖
Bsp/                外设通信适配
Tasks/              功能测试与日志模块
Middlewares_User/   RTT 与日志组件
MDK-ARM/            Keil 工程
```

## 工程使用指南

##### 环境与依赖

配套开发板与外设、工程驱动和静态库、Keil MDK 5（ARM Compiler 5）、J-Link 及 RTT Viewer。

##### 硬件连接与配置

通过 SWD 连接调试器；外设按工程 GPIO 配置接线并共地，I2C 信号线需上拉。日志使用 RTT 通道 0。

##### 编译、运行与观测

1. 包含 `keros_demo.h`，初始化后运行 `keros_demo_test(&report)`；各页写入不同的 `pattern=v2` 数据，全部写完后统一读回。
2. 保存完整写读日志，切换为 `keros_demo_test_read_only(&report)`，重新编译并烧录。
3. 烧录只读版本后真正断电，再上电运行只读测试并保存日志，期间不要再次写入。
4. 核对两份日志的序列号、模式、页号和完整 `READ` 数据，再查看汇总。

两个测试入口都需要先调用 `keros_demo_init()`，报告使用 `keros_demo_report_t`。在 RTT Viewer Terminal 0 保存完整日志；缺行或中间再次写入会影响两轮对比和断电保持判断。

每页 64 字节，请求页号 0～35，共 36 页、2304 字节。现有库允许实际访问 0～29 页，共 30 个用户页、1920 字节；30～35 页请求被库内部拒绝，打印 `SDK_LIMIT` 且 I2C 次数为零，没有有效读回数据。36 是请求数量，不是芯片总页数；2 KB 等于 2048 字节，也不能仅凭换算认定有 32 个可访问页。

最近两段完整的写读与只读日志中，0～29 页的 1920 字节全部符合期望且逐字节一致，两轮均为：

```text
EEPROM pages: requested=36 match=30 fail=0 sdk_limit=6
```

`MATCH_UNVERIFIED` 表示完整读回数据匹配，`SDK_LIMIT` 表示库拒绝，`FAIL` 表示实际访问或数据校验失败。程序无法确认外部断电过程；匹配字节数也不是实测物理容量，不能据此确定具体型号或丝印是否正确。

写读测试覆盖全部可写用户页，包括 01 工程存储密钥的页 0。只读入口不写 EEPROM，但初始化、认证和读取仍发送 I2C 控制命令。接入及报告细节见 [测试模块说明](Tasks/Keros_Test/README.md)。

##### 移植说明

移植时核对 BSP 引脚、延时和接口适配，更新头文件搜索路径，并保留配套静态库的链接配置。

## 作者思考

无

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
