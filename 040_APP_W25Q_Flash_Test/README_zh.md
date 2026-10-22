**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 APP 集成 W25Qxx

## 项目简介

本项目在 STM32F411CEU6 的 HAL + FreeRTOS APP 中集成 W25Qxx SPI Flash，
当前通过 SPI1 周期读取器件 ID；上一工程的 AT24C02 EEPROM 驱动及测试代码
继续保留。配套 Bootloader 基于 STM32F4 标准外设库（SPL），通过 YMODEM
接收加密固件，暂存到外部 W25Q64，
再解密安装到 MCU 内部 Flash 的 APP 运行区并跳转。

```text
本次独立测试：上电/复位 → FreeRTOS APP（0x08000000）
              → SPI1 读取 W25Q 器件 ID → EasyLogger/RTT 输出结果
配套加密升级：Bootloader（0x08000000）→ PA0 触发 YMODEM 接收至 W25Q64
              → 解密安装 APP（0x08008000）→ 切换 VTOR、MSP 并跳转
```

## 存储布局

Bootloader 联合运行时，MCU 内部 Flash 布局如下：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP 运行区 | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| 接收阶段擦除区 | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

加密固件实际从外部 W25Q64 偏移地址 `0x000000` 开始顺序存放。
接收阶段会擦除 MCU 内部 Flash 的 Sector 5；固件密文存放在 W25Q64。

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM 接收、W25Q64 密文暂存、解密安装及 APP 跳转。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，
  集成 W25Qxx 和 AT24C02 驱动，默认任务约每 2 秒读取 W25Q 器件 ID。

## 编译与运行

**作为 Bootloader 配套 APP 时，链接及烧录地址为 `0x08008000`。
本次独立测试为省去制作加密固件和通过上位机发送的过程，临时将 APP
起始地址改为 `0x08000000`，直接烧录明文测试 W25Q。**

1. 使用 Keil 打开 `03_Firmware/02_APP/MDK-ARM/STM32F411CEU6.uvprojx`。
2. 确认 IROM1 起始地址为 `0x08000000`，重新编译。
3. 连接 W25Q：PA5/PA6/PA7 分别为 SCK/MISO/MOSI，PA4 为软件片选；通过 J-Link/SWD 下载。
4. 复位后连接 RTT Viewer Terminal 0，查看 `w25q_id` 日志。

默认任务调用 `W25Qx_Read_ID()` 并以十六进制打印结果；连接 W25Q64 时，
预期标识为 `EF16`。当前循环仅执行 ID 读取，未调用数据读写或擦除测试。
EEPROM 的 `ee_test()` 保留在 `#if 0` 分支中，当前不执行。

独立测试占用原 Bootloader 区域。恢复联合升级时，先将 APP 地址改回
`0x08008000`，重编译不超过 96 KB 的明文 BIN 并生成匹配的加密包，
重新下载 Bootloader；复位时按住 PA0，通过 SecureCRT 的
**Transfer → Send Ymodem** 发送加密文件。未升级或安装失败时等待再次触发。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，起始地址 0x08000000
│   └── BSP/W25QXX/   外部 W25Q Flash 驱动与缓冲读写接口
└── 02_APP/            HAL + FreeRTOS APP，当前独立测试地址 0x08000000
    ├── BSP/W25Qxx/  SPI Flash 驱动，当前测试器件 ID 读取
    └── BSP/AT24Cxx/  保留的软件 I2C、EEPROM 驱动与最后一页测试
```

## 当前实现说明

- APP 的 W25Qxx 驱动提供复位、参数获取、读取、分页写入和擦除接口；
  默认任务只读取 ID。初始化调用顺序见 [APP 工程说明](03_Firmware/02_APP/README_zh.md)。
- 保留的 AT24C02 共 256 字节、每页 8 字节；`ee_test()` 测试最后一页，
  写入完成的 ACK 轮询超时为 10 ms，当前测试分支已关闭。
- YMODEM 接收端目前不计算或比对数据包末尾的 CRC16。
- `ymodem.c` 接收文件信息包后会擦除 MCU 内部 Flash 的 Sector 5，
  并将密文数据写入外部 W25Q64。
- 外部 Flash 读取依赖 RAM 中的读写索引，固件接收与安装需在同一次
  上电运行中完成。具体流程见
  [Bootloader 工程说明](03_Firmware/01_Bootloader/README_zh.md)。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
