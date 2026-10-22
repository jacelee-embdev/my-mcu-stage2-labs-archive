**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 APP 集成 YMODEM 接收

## 项目简介

本项目在 STM32F411CEU6 的 HAL + FreeRTOS APP 中集成 YMODEM 接收。
通过 USART1 DMA、串口空闲事件和队列逐包接收到 SRAM；业务线程同时执行
LED、AT24C02 EEPROM 读写测试及 W25Q 器件 ID 读取。接收线程当前只处理
协议应答，不保存完整文件，也不写入 Flash。

配套 SPL Bootloader 保留原加密升级流程：通过 YMODEM 接收加密固件，
暂存到 W25Q64，再解密安装到 MCU 内部 APP 运行区并跳转。

```text
当前独立测试：复位 → FreeRTOS APP（0x08000000）
              ├─ 业务线程：LED、EEPROM 测试、W25Q ID / RTT 日志
              └─ 接收线程：USART1 DMA → 空闲事件 → 队列 → SRAM 包解析及应答
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

Bootloader 从 W25Q64 偏移 `0x000000` 开始顺序存放密文，接收时仍擦除
内部 Sector 5。APP 当前独立链接地址为 `0x08000000`，配置大小为 96 KB；
APP 的 YMODEM 接收仅复用 1030 字节 SRAM 包缓冲区，不使用上述存储区。

## 固件工程

- [01_Bootloader](03_Firmware/01_Bootloader/README_zh.md)：SPL Bootloader、
  YMODEM 接收、W25Q64 密文暂存、解密安装及 APP 跳转。
- [02_APP](03_Firmware/02_APP/README_zh.md)：HAL + FreeRTOS APP，
  通过 `os_init()` 创建业务和接收线程，复用 W25Qxx、AT24C02 驱动。
  本阶段已完成 YMODEM 接收移植、注释排版整理及上板回归测试。

## 编译与运行

**当前独立测试直接烧录起始地址为 `0x08000000` 的明文 APP，接收线程
上电后自动等待 YMODEM 文件；发送 BIN 用于通信测试，不会安装或运行它。
恢复 Bootloader 联合运行时，APP 必须改回 `0x08008000`。**

1. 在 Keil 打开 `03_Firmware/02_APP/MDK-ARM/STM32F411CEU6.uvprojx`，
   确认 IROM1 起始地址为 `0x08000000`，编译并通过 J-Link/SWD 下载。
2. 串口适配器 TX/RX 分别接 PA10/PA9，并共地；SecureCRT 使用
   115200、8N1、无流控，复位后可见周期输出的 `C`。
3. 选择 **Transfer → Send Ymodem** 发送 BIN，确认传输完成及错误计数；
   结束后再次出现 `C` 表示设备进入下一轮接收等待。
4. RTT Viewer Terminal 0 查看业务日志。W25Q 的 SCK/MISO/MOSI 为
   PA5/PA6/PA7，PA4 为片选，W25Q64 预期 ID 为 `EF16`。
   EEPROM 使用 PB8/PB9，`ee_test()` 已启用，写读最后一页 `0xF8–0xFF`。

已通过 SecureCRT 传输 22,668 字节 BIN 文件，显示 100%、0 Errors；
排版及注释更新后回归测试正常。该结果验证当前条件下的接收流程。

联合升级时，将 APP 地址恢复为 `0x08008000`，重新编译不超过 96 KB 的
APP 并制作匹配的加密包，重新下载 Bootloader；复位时按住 PA0，
通过 YMODEM 发送加密文件。具体步骤见 Bootloader README。

## 工程目录

```text
03_Firmware/
├── 01_Bootloader/       SPL Bootloader，起始地址 0x08000000
│   └── BSP/W25QXX/      外部 Flash 驱动与缓冲读写接口
└── 02_APP/              HAL + FreeRTOS APP，当前独立地址 0x08000000
    ├── BSP/            UART_Handler、W25Qxx、AT24Cxx
    ├── SysSupport/     OS_init 队列/线程创建、Debug 日志初始化
    ├── Tasks/          APP 业务线程、SWC_OTA 接收线程
    └── Middlewares_User/ Ymodem、EasyLogger、SEGGER RTT
```

## 当前实现说明

- APP 接收等待超时为 2000 ms，应答前准备下一包接收；超时或会话退出时
  停止接收并清理通知。接收线程栈为 512 个字，会话返回后延时 10 ms。
- APP 每次空闲事件按一个完整包处理，不累计拆分包；APP 和 Bootloader
  当前均未校验包尾 CRC16，成功传输不等于逐字节完整性验证。
- 业务线程每轮执行后延时约 500 ms；EEPROM 最后一页读写测试已启用，
  W25Qxx 仍只测试器件 ID。初始化顺序见 [APP 说明](03_Firmware/02_APP/README_zh.md)。
- APP 的 `ymodem.c` 已移除 Flash 接收依赖；Bootloader 的同名文件
  仍执行 Sector 5 擦除和 W25Q64 写入，二者流程不同。
- Bootloader 的外部 Flash 读写索引保存在 RAM，固件接收与安装需在同一次
  上电运行中完成；完整流程见 [Bootloader 说明](03_Firmware/01_Bootloader/README_zh.md)。

文中涉及的固件样本和编译产物需按对应工程说明在本地重新生成。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
