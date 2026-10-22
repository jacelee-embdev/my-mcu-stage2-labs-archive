**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 集成 AT24C02

## 项目简介

本阶段将 AT24C02 驱动从 HAL 移植到 STM32F4 标准外设库（SPL）的
Bootloader，完成 PB8/PB9 软件 I²C、设备应答及最后一页读写测试。
2026-10-04，上板测试通过；随后整理驱动和入口文件的排版与注释。

既有 HAL + FreeRTOS APP 保留命令触发 YMODEM、W25Q64 数据处理、
EEPROM 元数据及按键软复位流程。Bootloader 保留加密接收、解密安装
和跳转流程。**本阶段完成 EEPROM 驱动接入，尚未将 APP 元数据与
Bootloader 安装流程联动；跨复位自动安装仍待实现。**

```text
本阶段验证：Bootloader（0x08000000）→ SPL 软件 I²C / EEPROM ACK
            → 最后一页 0xF8–0xFF 写入 → 写完成轮询 → 读回比较
既有加密升级：PA0 → YMODEM 密文至 W25Q64 → 解密安装 APP（0x08008000）
              → 切换 VTOR、MSP 并跳转
既有独立 APP：11 22 33 → C / YMODEM → W25Q64 / EEPROM 元数据
              → 44 55 66（DUf）→ 77 88 99 → PA0 软复位 → 原 APP
```

## 存储布局

Bootloader 联合运行时的 MCU 内部 Flash 布局：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP 运行区 | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| 接收阶段擦除区 | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

Bootloader 从 W25Q64 偏移 `0x000000` 顺序存放密文，接收时仍擦除内部
Sector 5。APP 当前独立链接地址为 `0x08000000`、配置大小为 96 KB，
占用 Bootloader 区域；联合运行需改回 `0x08008000`。APP 使用两个
1030 字节 SRAM 包缓冲区，不把当前下载文件安装到内部 Flash。

AT24C02 为 256 字节、每页 8 字节。APP 在 `0x00` 保存 OTA 状态，
`0x01–0x04` 保存四字节小端序新 APP 大小，`0x05–0x08` 预留当前 APP
大小；启动写 `0x00`、下载写 `0x11`，尚未写更新请求状态 `0x22`。
Bootloader 的读写测试仅覆盖 `0xF8–0xFF`，不访问上述元数据地址。

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM、W25Q64 密文暂存、解密安装及跳转；本阶段新增 AT24C02。
  `at24cxx.c/.h` 提供设备操作，`iic.c/.h` 提供软件 I²C；`main.c`
  启动时检测设备，并在执行到达驻留前的位置时测试最后一页。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，
  业务线程与 OTA 状态机并行运行；双缓冲、队列/信号量握手、完成应答、
  EEPROM 写完成等待与回读、更新命令及按键软复位已在上一阶段验证。

## 编译与运行

1. 在 Keil 打开
   `03_Firmware/01_Bootloader/MDK-ARM/STM32F411CEU6_SPL.uvprojx`，
   编译并通过 J-Link/SWD 下载，启动地址为 `0x08000000`。
2. EEPROM 使用 PB8（SCL）/PB9（SDA）、开漏输出和外部上拉；W25Q64
   使用 PA5/PA6/PA7、PA4 片选，串口 PA9/PA10 为 115200、8N1。
3. 在 RTT Viewer Terminal 0 检查七位地址 `0x50` 的 ACK、`0xF8–0xFF`
   每字节写入/读回值及 `EEPROM read/write test successful.`。
   `ee_test()` 执行一轮并覆盖最后一页原有数据。
4. 原有加密升级仍通过复位时按住 PA0、SecureCRT **Send Ymodem** 触发；
   APP 链接地址、密钥/IV 和包格式须匹配，详见
   [Bootloader 说明](03_Firmware/01_Bootloader/README_zh.md)。
5. 复现既有独立 APP 测试时，使用 `02_APP/MDK-ARM/STM32F411CEU6.uvprojx`
   并直接烧录明文 APP；依次发送 HEX `11 22 33`、YMODEM BIN、
   HEX `77 88 99`，最后按 PA0 软复位，详见
   [APP 说明](03_Firmware/02_APP/README_zh.md)。

本阶段 AT24C02 设备应答、写完成等待与最后一页读写测试通过。上一阶段
22,668 字节 BIN 传输达到 100%、0 Errors，完成应答、EEPROM 状态/大小
回读和按键软复位通过；`8C 58 00 00` 为对应文件大小。两项结果均不代表
APP 与 Bootloader 的跨复位安装协议或完整外部 Flash 镜像验证已完成。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/       SPL Bootloader，起始地址 0x08000000
│   ├── BSP/AT24Cxx/     SPL 软件 I²C、AT24C02 驱动与读写测试
│   └── BSP/W25QXX/      外部 Flash 驱动与缓冲读写接口
└── 02_APP/              HAL + FreeRTOS APP，当前独立地址 0x08000000
    ├── BSP/            UART_Handler、W25Qxx Handler、AT24Cxx
    ├── SysSupport/     OS_init 队列/线程创建、Debug 日志初始化
    ├── Tasks/          APP 业务线程、SWC_OTA 状态机及数据处理线程
    └── Middlewares_User/ Ymodem、EasyLogger、SEGGER RTT
```

## 当前实现说明

- Bootloader GPIO 使用 SPL 开漏接口，输出高表示释放总线；写完成轮询
  使用每毫秒递增的 `SysTickUpdateTime`，窗口为 10 ms，依赖已初始化
  且持续运行的 SysTick。AT24C02 没有可读芯片 ID，相关提示不是故障。
- 本次仅整理四个驱动文件及 `main.c` 的排版与注释：文件头、分区间隔线
  为英文，其他说明为中文；不改变标识符、宏内容、函数逻辑或日志字符串。
  版本号统一递增 0.1：四个驱动文件为 `V1.1`，`main.c` 为 `v1.7`，
  日期为 2026-10-04。
  APP 既有业务自测保持注释，Bootloader 的 `ee_test()` 当前启用。
- EEPROM 写接口返回成功表示数据发送完成，后续访问须等待内部写完成。
  APP 启动会清零 OTA 状态；Bootloader 尚未消费状态和大小，跨复位协议
  未对接。外部 Flash 索引仍在 RAM，既有接收与安装需在同次上电中完成。
- APP 接收及处理完成等待均为 2000 ms，线程栈各 512 个字；完成应答前
  延时 200 ms，更新命令后的按键窗口约 20 秒。每次空闲事件按完整包处理，
  不累计拆分包；APP 和 Bootloader 均未校验包尾 CRC16，也未完整回读
  外部 Flash 镜像。本阶段未修改 APP 流程。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
