**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 FreeRTOS APP — YMODEM 接收

## 简介

本工程基于 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS，通过 USART1 DMA、
串口空闲事件和队列将 YMODEM 数据逐包接收到 SRAM，同时运行业务线程。

**当前独立烧录地址为 `0x08000000`，配套 Bootloader 时恢复 `0x08008000`。
发送 BIN 仅测试通信；当前不写 Flash、不解密安装，也不保存完整文件。**

## 主要功能

- 初始化 HAL、100 MHz 系统时钟、GPIO、DMA、USART1、SPI1 和 FreeRTOS。
- `os_init()` 创建接收长度队列及 `app_task`、`swc_ota_task` 两个线程。
- 业务线程切换 PC13 LED、执行 `ee_test()`，并通过 EasyLogger/RTT
  输出 W25Q 器件 ID；每轮业务后延时 500 个节拍（当前约 500 ms）。
- USART1 使用 PA9/PA10、115200、8N1、无流控；接收使用
  DMA2 Stream2 Channel4 普通模式及 `HAL_UARTEx_ReceiveToIdle_DMA()`。
- 中断回调通过 `g_YmodemReclength` 传递长度，数据留在 1030 字节全局缓冲区。
  处理 128/1024 字节数据包、文件信息和结束应答；接收等待超时为 2000 ms。
- 应答前准备下一包接收；超时及会话退出时停止接收、清理通知。
  会话返回后延时 10 ms，再等待下一次传输。

SecureCRT 传输 22,668 字节 BIN 达到 100%、0 Errors，整理后的回归测试也已通过。
当前一次空闲事件按一包处理，不累计拆分包、未校验 CRC16；测试确认接收流程。

## Flash 分区

| 运行方式 | 链接及烧录地址 | 当前配置大小 |
|---|---:|---:|
| Bootloader 配套 APP | `0x08008000` | `0x18000`（96 KB） |
| 独立测试（当前） | `0x08000000` | `0x18000`（96 KB） |

独立测试占用 Bootloader 区域。联合运行时由 Bootloader 安装 APP、设置
VTOR/MSP 并跳转；APP 启动时重新开启中断。

## 编译与测试

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6.uvprojx`，确认 IROM1 为
   `0x08000000`，编译后通过 J-Link/SWD 下载。
2. 串口适配器 TX/RX 分别连接 PA10/PA9，并共地；SecureCRT 设置为
   115200、8N1、无流控。复位后可见周期发送的 `C`。
3. 选择 **Transfer → Send Ymodem** 发送 BIN，确认传输完成；
   结束后再次出现 `C` 表示进入下一轮接收等待。
4. RTT Viewer Terminal 0 查看业务日志。W25Q 使用 PA5/PA6/PA7、
   PA4 片选（SPI 模式 0、256 分频），W25Q64 预期 ID 为 `EF16`；
   EEPROM 使用 PB8/PB9，测试写读最后一页 `0xF8–0xFF`。
5. 恢复联合升级时，将 APP 地址改回 `0x08008000`，重新编译不超过
   96 KB 的 APP，下载 Bootloader，再发送匹配的加密升级包。

W25Qxx 驱动保留读写和擦除接口，APP 接收流程未调用它们；
`W25Qx_Init()` 仍位于 GPIO/SPI 初始化之前，当前 ID 读取测试不验证初始化结果。

## 主要目录

```text
Core/                    HAL 外设、FreeRTOS 与程序入口
BSP/UART_Handler/        串口接收事件回调
BSP/W25Qxx/              SPI Flash 驱动
BSP/AT24Cxx/             软件 I2C、EEPROM 驱动及最后一页测试
SysSupport/              OS_init 队列/线程创建、Debug 日志初始化
Tasks/                   APP 业务线程、SWC_OTA 接收线程与 SRAM 缓冲区
Middlewares_User/        Ymodem、EasyLogger 与 SEGGER RTT
Middlewares/Third_Party/ FreeRTOS
MDK-ARM/                 Keil 工程文件
```

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
