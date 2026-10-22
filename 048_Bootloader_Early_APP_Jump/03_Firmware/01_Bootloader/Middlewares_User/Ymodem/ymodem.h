/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file ymodem.h
 *
 * @par dependencies
 * - stdint.h
 * - string.h
 * - common.h
 * - flash.h
 * - bootloader.h
 * - w25qxx_Handler.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the YMODEM interface for receiving APP firmware.
 *
 * Processing flow:
 *
 * Call Ymodem_Receive() with a packet buffer to receive an image and
 * obtain its advertised size when the transfer completes.
 *
 * @version v1.2 2026-09-30
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __YMODEM_H__
#define __YMODEM_H__

/****************************** Includes ******************************/
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "flash.h"
#include "bootloader.h"
#include "w25qxx_Handler.h"
/****************************** Includes ******************************/

/****************************** Functions ******************************/
/**
 * @brief 通过 YMODEM 协议接收 APP 固件并写入外部 W25Q。
 *
 * @param[in,out] p_buffer 接收并暂存数据包的缓冲区。
 *
 * @return 成功时返回固件大小；失败时返回对应的负值或 0。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer);
/****************************** Functions ******************************/

#endif /* __YMODEM_H__ */
