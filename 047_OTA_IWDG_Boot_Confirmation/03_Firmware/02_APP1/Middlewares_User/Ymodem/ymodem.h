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
 * @brief Declares the synchronized dual-buffer YMODEM receive interface.
 *
 * Processing flow:
 *
 * Create the payload queue and completion semaphore before reception.
 * Pass two distinct packet buffers and wait for processing completion
 * before the receiver switches buffers and acknowledges file data.
 *
 * @version v1.2 2026-10-06
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

/************************** External Variables **************************/
extern int32_t packet_length;  /* 数据区长度或控制包标记，供处理线程使用。 */
/************************** External Variables **************************/

/************************ Function Declarations ************************/
/**
 * @brief 使用双缓冲接收一个完整加密文件并交给写入线程。
 *
 * 处理步骤：
 * 1. 准备 DMA 接收，发送请求字符并解析文件信息。
 * 2. 校验包长度，发送长度指针并等待写入线程准备完成。
 * 3. 逐包检查类型和序号，将有效数据指针交给写入线程。
 * 4. 最多等待 2 秒确认处理结果，再累计有效长度、切换缓冲区并应答。
 * 5. 数据结束和空文件名结束时检查完整长度，再停止接收。
 * 加密文件须大于 16 字节、按 16 字节对齐且不超过 A 区容量。
 * 末包填充不写入；上一数据包的重传只确认，不重复写入。
 * 当前不计算或验证 CRC16，写入由独立线程执行。
 *
 * @param[in,out] p_buffer_1 第一个接收缓冲区，至少 1030 字节。
 * @param[in,out] p_buffer_2 第二个接收缓冲区，至少 1030 字节且与第一个不同。
 *
 * @return 会话结果；正值为完整接收的加密文件字节数。
 * @retval 0 发送端取消，或会话开始后连续接收错误超过阈值。
 * @retval -1 参数、格式、大小、长度、队列交接、处理等待或写入失败。
 * @retval -3 收到用户终止字符。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer_1, uint8_t *p_buffer_2);
/************************ Function Declarations ************************/

#endif /* 结束头文件保护。 */
