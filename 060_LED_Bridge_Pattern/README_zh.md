**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 LED 桥接模式

本项目记录我使用 C 语言实践嵌入式面向对象设计的过程：以结构体封装对象状态，以函数指针提供操作接口，通过桥接模式分离 LED 行为与硬件、操作系统实现。业务层提交闪烁请求，不直接操作 GPIO 或 FreeRTOS 队列，便于替换底层接口和扩展多个 LED 实例。

## 模块设计

| 模块 | 职责 |
|---|---|
| `led.c/.h` | 初始化 PC13，提供 `LED_ON()`、`LED_OFF()` 和翻转操作 |
| `bsp_led_driver.c/.h` | 封装单灯状态、闪烁参数，以及硬件、时基和延时接口 |
| `bsp_led_handler.c/.h` | 注册 Driver 对象，通过消息队列调度异步闪烁 |
| `system_adaption.c/.h` | 绑定硬件与 FreeRTOS 服务，构造对象并注册板载 LED |
| `os_init`、`led_task`、`status_task` | 创建任务、运行闪烁测试及输出状态日志 |

Driver 与 Handler 均由调用方提供对象内存；系统层使用静态对象和只读接口表，确保后台访问期间地址有效。默认开启 OS 支持并关闭 Driver 同步控制，异步闪烁由 Handler 执行。

## 调用与测试

先初始化 LED 硬件和日志，在调度器启动后调用 `System_InitResources()`。Driver 构造包含 600 ms 延时，随后创建 Handler 队列与线程，完成 Driver 注册。

`System_LEDControl()` 接受周期、次数和亮灭比例。Handler 最多注册 10 个 Driver，消息队列容量为 10，支持 1:1、1:2、1:3 三种比例，校验后以零等待方式复制请求。`SYSTEM_OK` 表示请求已入队；工作线程按顺序执行，结果另行记录到日志。线程每轮处理后延时 1000 ms 再轮询；注册表访问受临界区保护，延时和 LED 操作在临界区外执行。

当前 `led_task` 循环测试 400、800、1200 ms 三种周期，各周期遍历三种比例，每组闪烁 3 次，组间等待该组名义执行时长加 2000 ms。`status_task` 每 5000 ticks 输出日志，不参与 LED 控制。

## 启动标记

工程保留 `bsp_target` 独立对象段及自定义链接布局，以 `0xEC` 标记初始状态，实例化成功后更新为 `0x01`。启动拷贝只准备 RAM 初值；接口绑定、OS 资源创建和 Driver 延时仍在运行时完成。

## 编译与验证

目标平台为 STM32F411CEU6，PC13 低电平点亮 LED。使用 Keil MDK 5 / ARM Compiler 5 打开 [工程文件](MDK-ARM/STM32F411CEU6.uvprojx)，编译并通过 J-Link/SWD 下载。RTT Viewer 通道 0 用于观察 EasyLogger 日志，SystemView 辅助跟踪任务。

烧录后的预期现象为 PC13 LED 按上述周期与亮灭比例循环闪烁，每组 3 次。通过 RTT 日志检查资源初始化、请求入队、执行结果和周期状态信息；仅有入队成功日志不能说明闪烁已执行完成。

工程原有验证记录说明：同步控制开启、关闭两种配置均编译链接通过，**0 错误、0 警告**，更新后的 LED 功能已通过实物验证。这些属于此前的工程记录。本次归档检查文件完整性、工程引用、忽略规则及文档链接，未重新执行编译、烧录或硬件测试。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
