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
 * @brief Declares the dual-buffer YMODEM receive interface.
 *
 * Processing flow:
 *
 * Pass two packet buffers of at least 1030 bytes to Ymodem_Receive().
 * Create the payload queue and completion semaphore before reception.
 * The worker processes each payload before the receiver acknowledges.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __YMODEM_H__
#define __YMODEM_H__

/********************************* Includes *********************************/
#include <stdint.h>
#include <string.h>

#include "common.h"
/********************************* Includes *********************************/

/**************************** External Variables ****************************/
extern int32_t packet_length;  /* 数据区长度或控制包标记，供处理线程使用。 */
/**************************** External Variables ****************************/

/************************** Function Declarations ***************************/
/**
 * @brief 通过双缓冲接收文件会话，并交给处理线程逐包处理。
 *
 * 1. 初始化串口接收状态，发送请求字符并解析文件信息。
 * 2. 发送文件大小指针，等待处理线程完成准备握手。
 * 3. 逐包检查格式和序号，发送数据指针并等待处理完成。
 * 4. 切换接收缓冲区并应答，收到结束信息后停止接收。
 * 当前不计算或验证包尾校验值；数据写入由独立处理线程执行。
 *
 * @param[in,out] p_buffer_1 第一个接收缓冲区，至少可容纳 1030 字节。
 * @param[in,out] p_buffer_2 第二个接收缓冲区，至少可容纳 1030 字节。
 *
 * @return int32_t 会话结果；正值为发送端声明的文件字节数。
 * @retval 0 发送端取消、连续错误超过阈值，或声明大小为零。
 * @retval -1 参数、文件信息、大小或线程交接无效。
 * @retval -3 收到用户终止字符。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer_1, uint8_t *p_buffer_2);
/************************** Function Declarations ***************************/

#endif /* 结束头文件保护。 */
