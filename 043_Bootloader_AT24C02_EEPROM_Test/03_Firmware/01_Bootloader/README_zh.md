**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Bootloader

## 简介

本工程是基于 STM32F4 标准外设库（SPL）的 Bootloader，起始地址为
`0x08000000`。它通过 USART1 和 YMODEM 接收加密固件，将密文暂存到外部
W25Q64 Flash；随后读取、解密并写入内部 APP 运行区（`0x08008000`），
回读校验后尝试跳转。未请求升级或安装失败时，可通过 PA0 再次发起升级。
工程 043 新增 AT24C02 的 SPL 软件 I²C 驱动，设备应答及最后一页读写
已通过上板验证；EEPROM 元数据尚未接入安装流程。

## 主要功能

- 通过 SecureCRT YMODEM 接收加密固件，处理文件信息包、
  128/1024 字节数据包及 ACK、NAK、CA 等控制字符。
- 使用 SPI1 访问 W25Q64，按 4 KB 扇区缓冲写入，结束时写入剩余数据。
- 检查加密包和 APP 大小，逐块解密，按 32 位字写入内部 APP 运行区并回读。
- 检查 APP 初始栈地址，切换 VTOR 和 MSP 后跳转到 APP 入口。
- USART1 使用 PA9/PA10、115200 波特率；SPI1 使用 PA5/PA6/PA7，
  PA4 为片选；PA0 为升级键，PC13 为 LED。
- AT24C02 使用 PB8（SCL）/PB9（SDA）开漏软件 I²C，七位地址为
  `0x50`，写/读地址字节为 `0xA0/0xA1`；提供检测、读写及擦除接口。
- 提供 DWT/SysTick 延时、EasyLogger 和 SEGGER RTT 日志。

## 存储布局

MCU 内部 Flash：

| 区域 | Sector | 起始地址 | 大小 | 地址范围 |
|---|---|---:|---:|---:|
| Bootloader | 0–1 | `0x08000000` | 32 KB | `0x08000000 ~ 0x08007FFF` |
| APP 运行区 | 2–4 | `0x08008000` | 96 KB | `0x08008000 ~ 0x0801FFFF` |
| 接收阶段擦除区 | 5 | `0x08020000` | 128 KB | `0x08020000 ~ 0x0803FFFF` |

密文从 W25Q64 偏移 `0x000000` 顺序写入，接收阶段仍擦除内部 Sector 5。
AT24C02 容量为 256 字节、每页 8 字节；`ee_test()` 仅覆盖最后一页
`0xF8–0xFF`，不访问 APP 现有的低地址 OTA 元数据区。

## 编译与验证

1. 使用 Keil 打开 `MDK-ARM/STM32F411CEU6_SPL.uvprojx`，编译并通过
   J-Link/SWD 下载 Bootloader。SCL、SDA 需要外部上拉电阻。
2. 在 RTT Viewer Terminal 0 检查 `EEPROM ACK: addr7=0x50`、测试地址
   `0xF8–0xFF` 的写入/读回值及 `EEPROM read/write test successful.`。
   `main()` 启动时检测 EEPROM；若执行到达驻留前的位置，则测试一次。
3. 验证原有加密升级时，生成链接地址为 `0x08008000`、明文不超过 96 KB
   的 APP BIN，再使用匹配的密钥、IV 和包格式生成加密文件。
4. 复位时按住 PA0，在 SecureCRT 中选择 **Transfer → Send Ymodem**。
   接收后从 W25Q64 解密安装，校验通过后尝试跳转；未触发升级或安装失败
   时，等待 PA0 再次触发。

2026-10-04，移植后的设备应答、写完成等待及最后一页读写测试通过。
随后仅整理排版与注释；四个驱动文件更新为 `V1.1`，`main.c` 为 `v1.7`。

## 当前实现说明

- GPIO 访问使用 SPL 接口；`ee_test()` 使用每毫秒递增的
  `SysTickUpdateTime`，以 10 ms 窗口轮询写完成。须先调用 `delay_init()`
  并保持 SysTick 中断运行；写接口返回成功仅表示数据发送完成。
- AT24C02 没有芯片 ID 读取命令，相关提示不是检测失败。
  `There is no valid app...` 是驻留循环提示，与 EEPROM 测试结果分开判断。
- Bootloader 尚未读取或处理 APP 的 OTA 状态及文件大小；W25Q64 读写
  索引仍在 RAM，原有接收和安装需在同一次上电中完成。
- 首个加密 16 字节块包含 APP 明文大小；接收端未校验包尾 CRC16。
  文件信息包接收后仍调用 `Flash_erase(APP_BACKUP_BASE_ADDR, size)`，
  密文实际由 `W25Q64_WriteData()` 写入外部 Flash。

## 主要目录

```text
Core/                            程序入口及 GPIO、Flash、USART、SPI1
BSP/AT24Cxx/                     SPL 软件 I²C 与 AT24C02 驱动、读写测试
BSP/W25QXX/                      W25Q 驱动与缓冲读写接口
SYSTEM/delay/                    DWT 与 SysTick 延时模块
Drivers/                         CMSIS 与 STM32F4 SPL
Middlewares_User/Ymodem/          YMODEM 接收及串口辅助代码
Middlewares_User/Algorithms/AES/  AES 解密
Middlewares_User/                EasyLogger 与 SEGGER RTT
Tasks/Bootloader/                固件解密安装、APP 跳转逻辑
Tasks/Debug/                     EasyLogger 初始化
MDK-ARM/                         Keil 工程文件
```

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
