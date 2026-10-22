/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file ymodem.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the YMODEM receive interface used by the Bootloader.
 *
 * @version V1.0 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __YMODEM_H__
#define __YMODEM_H__

/****************************** Includes ******************************/
#include <stdint.h>
/****************************** Includes ******************************/

/****************************** Functions ******************************/
/**
 * @brief 通过 YMODEM 协议接收并写入一个 APP 固件。
 *
 * @param[in] p_buffer 接收数据时使用的临时缓冲区。
 *
 * @return 成功时返回固件大小；失败时返回对应的负值或 0。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer);
/****************************** Functions ******************************/

#endif /* __YMODEM_H__ */
