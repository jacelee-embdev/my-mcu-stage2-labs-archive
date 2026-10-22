/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file main.h
 *
 * @par dependencies
 * - stm32f4xx.h
 * - stdio.h
 * - gpio.h
 * - flash.h
 * - usart.h
 * - delay.h
 * - SEGGER_RTT.h
 * - elog.h
 * - bootloader.h
 * - ymodem.h
 * - debug.h
 * - spi.h
 * - w25qxx_Handler.h
 * - at24cxx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides common Bootloader services and AT24C02 dependencies.
 *
 * Processing flow:
 *
 * Include this header from the Bootloader entry source to obtain device,
 * communication, logging, firmware-update, SPI, W25Q, and EEPROM interfaces.
 *
 * @version v1.7 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H

/****************************** Includes ******************************/
/* 设备定义及标准输入输出接口。 */
#include "stm32f4xx.h"
#include "stdio.h"

/* 板级引脚、内部闪存、串口及延时接口。 */
#include "gpio.h"
#include "flash.h"
#include "usart.h"
#include "delay.h"

/* 日志、固件升级及调试接口。 */
#include "SEGGER_RTT.h"
#include "elog.h"
#include "bootloader.h"
#include "ymodem.h"
#include "debug.h"

/* 外部闪存及串行存储器访问接口。 */
#include "spi.h"
#include "w25qxx_Handler.h"
#include "at24cxx.h"
/****************************** Includes ******************************/

#endif /* 结束头文件重复包含保护。 */
