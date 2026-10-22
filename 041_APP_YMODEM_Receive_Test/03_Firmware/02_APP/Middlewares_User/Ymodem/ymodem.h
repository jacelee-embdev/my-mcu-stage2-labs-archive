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
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the SRAM-based YMODEM receive interface.
 *
 * Processing flow:
 *
 * Pass a packet buffer of at least 1030 bytes to Ymodem_Receive().
 * The buffer is reused for each packet rather than holding the full file.
 *
 * @version v1.0 2026-10-02
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __YMODEM_H__
#define __YMODEM_H__

/******************************* Includes *******************************/
#include <stdint.h>
#include <string.h>

#include "common.h"
/******************************* Includes *******************************/

/************************ Function Declarations ************************/
/**
 * @brief 通过串口 DMA、空闲事件和队列接收文件会话。
 *
 * 缓冲区逐包复用；当前检查包格式和序号并应答，不保存完整文件。
 * 当前不写入 Flash，也不计算或验证包尾 CRC16。
 *
 * @param[in,out] p_buffer 接收缓冲区，至少可容纳 1030 字节。
 *
 * @return 正值为发送端声明的文件大小。
 * @retval 0 发送端取消、连续错误超过阈值，或声明大小为零。
 * @retval -1 参数、文件信息或文件大小无效。
 * @retval -3 收到用户终止字符。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer);
/************************ Function Declarations ************************/

#endif /* __YMODEM_H__ */
