**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS APP — 应用内 OTA 接收与软复位

## 简介

本工程基于 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS，通过 USART1 DMA 接收
YMODEM 文件，串联 W25Q64 数据处理、AT24C02 元数据写读及按键软复位，
验证应用内无感升级流程；业务线程同时运行。

**当前独立链接及烧录地址为 `0x08000000`，配置大小为 96 KB。
本阶段测试不涉及 Bootloader，不安装新固件到内部 Flash；软复位后仍运行原 APP。**

## 主要功能

- 初始化 HAL、100 MHz 系统时钟、GPIO、DMA、USART1、SPI1 和 FreeRTOS。
  `os_init()` 创建接收长度队列及 `app_task`、`swc_ota_task` 两个线程。
- 业务线程每 500 个节拍（约 500 ms）切换 PC13 LED；`ee_test()` 和
  W25Q 器件 ID 测试已注释。
- USART1 使用 PA9/PA10、115200、8N1、无流控；接收使用 DMA2 Stream2
  Channel4 普通模式、空闲事件及 `g_YmodemReclength_queue` 长度通知。
- YMODEM 支持 128/1024 字节包，交替使用两个 1030 字节 SRAM 缓冲区。
  `app_data_position_queue` 传递大小及数据指针；处理线程调用
  `W25Q64_WriteData()`，通过信号量通知接收线程切换缓冲区并应答。
- 会话结束后等待 200 ms，再发送 `44 55 66`（ASCII 为 `DUf`），让上位机
  先退出 YMODEM 处理；随后调用 `W25Q64_WriteData_End()` 并释放下载资源。
- 收到 `77 88 99` 后等待 PA0 低电平按键；10 个节拍消抖，400 次扫描、
  50 个节拍间隔（约 20 秒）。按下触发 `NVIC_SystemReset()`，超时返回
  等待下载状态；状态机每轮末尾延时 10 ms。

## EEPROM 元数据

AT24C02 使用 PB8/PB9 软件 I2C。当前地址及处理方式如下：

| 数据 | 起始地址 | 长度 | 当前实现 |
|---|---:|---:|---|
| OTA 状态 | `0x00` | 1 字节 | 启动写 `0x00`，下载阶段写 `0x11` |
| 新 APP 文件大小 | `0x01` | 4 字节 | 接收成功后写入声明大小，按小端序保存 |
| 当前 APP 大小 | `0x05` | 4 字节 | 预留，当前未更新 |

下载成功分支检查写入结果，每 1 ms 轮询 ACK，等待窗口为 10 ms；就绪后
继续访问并通过 EasyLogger/RTT 输出回读。22,668 字节对应 `0x0000588C`，
按地址递增读取为 `8C 58 00 00`。状态 `0x22` 尚未写入，APP 启动会清零状态。

## Flash 分区

| 运行方式 | 链接及烧录地址 | 配置大小 |
|---|---:|---:|
| 独立测试（当前） | `0x08000000` | `0x18000`（96 KB） |
| Bootloader 配套 APP | `0x08008000` | `0x18000`（96 KB） |

独立测试占用 Bootloader 区域。W25Q64 使用 4 KB 扇区缓冲接口；本阶段
验证流程，未验证完整外部 Flash 镜像。联合运行需恢复 APP 地址并对接安装逻辑。

## 编译与测试

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，确认 IROM1 为
   `0x08000000`，编译后通过 J-Link/SWD 下载。
2. 串口适配器 TX/RX 分别连接 PA10/PA9，并共地；串口助手使用
   115200、8N1、无流控，按 HEX 发送 `11 22 33`，设备开始周期回复 `C`。
3. 关闭串口助手端口，使用相同设置连接 SecureCRT，选择
   **Transfer → Send Ymodem** 发送 BIN；完成后观察 `DUf` 应答。
4. RTT Viewer Terminal 0 查看接收大小及 EEPROM 回读：状态为 `11`，
   22,668 字节文件的大小为 `8C 58 00 00`。W25Q 使用 PA5/PA6/PA7、
   PA4 片选（SPI 模式 0、256 分频），EEPROM 使用 PB8/PB9。
5. 断开 SecureCRT，重新打开串口助手，按 HEX 发送 `77 88 99`，约 20 秒内
   按下 PA0；观察启动日志重新出现，确认软复位。

2026-10-04 已完成 22,668 字节 BIN 的 100%、0 Errors 传输、完成应答、
EEPROM 回读及按键复位；整理及版本更新后回归通过。临时流程诊断日志已清理，
保留接收结果、EEPROM 回读及错误/警告日志。

当前一次空闲事件按一包处理，不累计拆分包，也未校验 CRC16。
`W25Q64_Init()` 仍位于 GPIO/SPI 初始化之前；现有初始化顺序保持不变。

## 主要目录

```text
Core/                    CubeMX 外设、FreeRTOS 与程序入口
BSP/UART_Handler/        串口接收事件回调
BSP/W25Qxx/              SPI Flash 驱动与扇区缓冲读写接口
BSP/AT24Cxx/             软件 I2C、EEPROM 驱动及独立测试接口
SysSupport/              OS_init 队列/线程创建、Debug 日志初始化
Tasks/                   APP 业务线程、SWC_OTA 状态机及数据处理线程
Middlewares_User/        Ymodem、EasyLogger 与 SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil 工程文件
```

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
