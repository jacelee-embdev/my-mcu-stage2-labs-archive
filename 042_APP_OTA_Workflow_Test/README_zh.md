**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 APP 无感升级流程验证

## 项目简介

本项目在 STM32F411CEU6 的 HAL + FreeRTOS APP 中实现命令触发的 YMODEM
接收、外部 W25Q64 数据处理、AT24C02 元数据写读及按键软复位。业务线程
同时切换 LED。当前已跑通应用内无感升级的流程验证，并通过上板回归测试。

**本阶段只运行独立 APP，不涉及 Bootloader 安装。下载文件不写入内部
APP 运行区；按键触发软复位后仍运行原 APP。** 配套 SPL Bootloader 保留
原有加密升级流程：YMODEM 接收密文至 W25Q64，解密安装后跳转。

```text
当前独立测试：复位 → FreeRTOS APP（0x08000000）
              ├─ 业务线程：PC13 LED，每轮延时约 500 ms
              └─ 11 22 33 → C / YMODEM 双缓冲接收 → 外部数据处理
                 → 44 55 66（DUf）/ EEPROM 元数据 → 77 88 99
                 → 约 20 秒内按下 PA0 → 软复位 → 原 APP
配套加密升级：Bootloader（0x08000000）→ PA0 触发接收至 W25Q64
              → 解密安装 APP（0x08008000）→ 切换 VTOR、MSP 并跳转
```

## 存储布局

以下为 Bootloader 联合运行时的 MCU 内部 Flash 布局：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP 运行区 | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| 接收阶段擦除区 | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

APP 当前独立链接地址为 `0x08000000`，配置大小为 96 KB，占用 Bootloader
区域。接收使用两个 1030 字节 SRAM 包缓冲区及 W25Q64 扇区缓冲接口，
不将下载文件安装到 MCU 内部 Flash。配套 Bootloader 从 W25Q64 偏移
`0x000000` 顺序存放密文，接收时仍擦除内部 Sector 5。

AT24C02 的 `0x00` 保存 1 字节 OTA 状态，`0x01–0x04` 保存 4 字节小端序
新 APP 文件大小；`0x05–0x08` 的当前 APP 大小预留。当前启动写 `0x00`，
下载阶段写 `0x11`，尚未写入更新请求状态 `0x22` 或对接 Bootloader。

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM 接收、W25Q64 密文暂存、解密安装及 APP 跳转；本阶段未改动。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，包含业务
  线程、OTA 状态机及动态创建的数据处理线程。双缓冲、队列/信号量握手、
  完成应答、EEPROM 写完成等待与回读、更新命令及按键软复位已验证。

## 编译与运行

**当前独立测试直接烧录起始地址为 `0x08000000` 的明文 APP；上电后先
等待下载命令，再启动 YMODEM。恢复 Bootloader 联合运行需改回
`0x08008000` 并完成安装流程对接。**

1. 在 Keil 打开 `03_Firmware/02_APP/MDK-ARM/STM32F411CEU6.uvprojx`，
   确认 IROM1 起始地址为 `0x08000000`，编译并通过 J-Link/SWD 下载。
2. 串口适配器 TX/RX 分别接 PA10/PA9，并共地；串口助手设置为
   115200、8N1、无流控，按 HEX 发送 `11 22 33`，确认收到 `C`。
3. 关闭串口助手端口，用 SecureCRT 连接并选择 **Transfer → Send Ymodem**
   发送 BIN；确认传输完成和 `44 55 66`（ASCII `DUf`）应答。
4. RTT Viewer Terminal 0 检查接收大小和 EEPROM 回读：22,668 字节文件
   对应 `8C 58 00 00`，下载状态为 `11`。W25Q 使用 PA5/PA6/PA7、
   PA4 片选，EEPROM 使用 PB8/PB9。
5. 断开 SecureCRT，重新打开串口助手，按 HEX 发送 `77 88 99`，约 20 秒内
   按下 PA0；启动日志重新出现表示软复位成功。

2026-10-04，22,668 字节 BIN 传输达到 100%、0 Errors，完成应答、EEPROM
状态/大小回读及按键软复位成功；排版、注释、日志清理和版本更新后回归通过。
该结果验证本阶段的应用内流程，未验证新固件安装或完整外部 Flash 镜像。

配套加密升级的链接地址、固件包及操作要求见 [Bootloader 说明](03_Firmware/01_Bootloader/README_zh.md)。
该工程的既有流程与本阶段 APP 元数据尚未联动。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/       SPL Bootloader，起始地址 0x08000000
│   └── BSP/W25QXX/      外部 Flash 驱动与缓冲读写接口
└── 02_APP/              HAL + FreeRTOS APP，当前独立地址 0x08000000
    ├── BSP/            UART_Handler、W25Qxx Handler、AT24Cxx
    ├── SysSupport/     OS_init 队列/线程创建、Debug 日志初始化
    ├── Tasks/          APP 业务线程、SWC_OTA 状态机及数据处理线程
    └── Middlewares_User/ Ymodem、EasyLogger、SEGGER RTT
```

## 当前实现说明

- YMODEM 接收及处理完成等待均为 2000 ms；应答前准备下一包接收，
  超时或会话退出时停止接收并清理通知。接收、处理线程栈各为 512 个字。
- 完成应答前的 200 ms 延时用于让上位机退出 YMODEM 处理；按键等待
  使用 400 次扫描、50 ms 间隔和 10 ms 消抖，超时返回等待下载状态。
- EEPROM 写接口返回不代表内部写周期结束；成功分支轮询 ACK 后再访问，
  等待窗口为 10 ms。启动清零状态，当前元数据尚不构成跨复位安装协议。
- APP 每次空闲事件按一个完整包处理，不累计拆分包；APP 和 Bootloader
  均未验证包尾 CRC16，外部 Flash 也未做完整镜像回读比对。
- `ee_test()` 和 W25Q ID 业务测试已注释；保留接收结果、EEPROM 回读及
  错误/警告日志。相关手写文件版本更新为 `1.1`（2026-10-04）；
  CubeMX 生成文件不纳入注释排版整理。
- Bootloader 的外部 Flash 索引仍保存在 RAM，其既有接收与安装流程需在
  同一次上电中完成；APP 初始化顺序等细节见 [APP 说明](03_Firmware/02_APP/README_zh.md)。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
