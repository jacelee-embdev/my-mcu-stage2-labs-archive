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
 * - w25qxx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides common dependencies for the Bootloader entry module.
 *
 * Processing flow:
 *
 * Include this header from the Bootloader entry source to obtain device,
 * communication, logging, firmware-update, SPI, and W25Q interfaces.
 *
 * @version v1.5 2026-09-29
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H

/****************************** Includes ******************************/
#include "stm32f4xx.h"
#include "stdio.h"

#include "gpio.h"
#include "flash.h"
#include "usart.h"
#include "delay.h"

#include "SEGGER_RTT.h"
#include "elog.h"
#include "bootloader.h"
#include "ymodem.h"
#include "debug.h"
#include "spi.h"
#include "w25qxx.h"

/****************************** Includes ******************************/

#endif /* End of __MAIN_H */
