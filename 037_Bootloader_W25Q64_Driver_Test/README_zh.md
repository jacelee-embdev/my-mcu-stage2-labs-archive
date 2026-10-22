**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader 与 APP 示例

## 项目简介

本项目包含 STM32F411CEU6 Bootloader 和独立 APP。Bootloader 使用 STM32F4
SPL，通过 YMODEM 将加密固件下载到 B 备份区，解密安装到 A 运行区并
完成程序跳转；APP 使用 STM32 HAL、CMSIS-RTOS2 和 FreeRTOS。当前
Bootloader 还启用了 SPI1 与 W25Q64 的擦写回归测试。

```text
上电/复位
  → Bootloader（0x08000000）初始化 SPI1 与 W25Q，检测 PA0 升级键
  → 按键触发：YMODEM 接收密文到 APP Backup，解密安装到 APP Run
  → 校验 APP 栈顶，切换 VTOR 与 MSP，跳转到 FreeRTOS APP
  → 未触发升级：打印 W25Q ID，循环擦除、写入、读回并输出结果
```

## Flash 分区

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP Run | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| APP Backup | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

以上为 MCU 内部 Flash；W25Q 测试使用外部芯片 `0x007FF000` 起始的 4 KB 扇区。

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、YMODEM 加密升级，以及 SPI1/W25Q 擦写测试。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，作为加密升级和跳转运行的目标固件。

## 编译与运行

1. 编译并通过 J-Link/SWD 下载 `01_Bootloader`，连接 SEGGER RTT Viewer。
2. 复位后检查 W25Q ID：W25Q64 为 `61206`（`0xEF16`）；循环日志中
   `write` 每轮加 2，且应与 `read` 相等并显示 `OK`。
3. 每轮测试会擦除外部 W25Q 测试扇区，清除原有数据；随后延时 1 秒。
4. 如需升级，编译 `02_APP` 为地址 `0x08008000`、最大 96 KB 的 BIN，
   按 Bootloader 使用的密钥、IV 和包格式加密。
5. 复位时按住 PA0，在 SecureCRT 中选择 **Transfer → Send Ymodem**
   发送加密文件；成功后 Bootloader 安装 APP 并跳转运行。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/     SPL Bootloader，地址 0x08000000
│   └── BSP/W25QXX/   外部 W25Q Flash 驱动
└── 02_APP/            HAL + FreeRTOS APP，地址 0x08008000
```

## 协议说明

当前 YMODEM 实现面向固件接收，尚未计算并比对接收数据包末尾的 CRC16。
当前 Bootloader 启用了 W25Q 测试循环，原回退循环暂由条件编译关闭。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
