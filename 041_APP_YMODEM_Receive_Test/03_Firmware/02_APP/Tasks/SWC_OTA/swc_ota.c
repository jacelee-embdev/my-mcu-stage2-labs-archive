/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file swc_ota.c
 *
 * @par dependencies
 * - swc_ota.h
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the YMODEM receive task with an SRAM packet buffer.
 *
 * Processing flow:
 *
 * Receive a file session into the shared packet buffer.
 * Wait 10 ms after each session and start another receive attempt.
 *
 * @version v1.0 2026-10-02
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "swc_ota.h"

#include "stdint.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define BUFFER_SIZE  1030  /* 单个最大数据包的接收缓冲区容量，单位为字节。 */
/******************************* Defines *******************************/

/****************************** Variables ******************************/
/* 接收缓冲区位于 SRAM，下一包会复用并覆盖上一包的数据。 */
uint8_t g_au8_YmodemRec[BUFFER_SIZE];
/****************************** Variables ******************************/

/****************************** Functions ******************************/
/**
 * @brief 运行 YMODEM 接收线程。
 *
 * 使用全局缓冲区逐包接收，每次会话结束后延时 10 毫秒。
 * 当前只接收并完成协议应答，不写入 Flash，也不保留完整文件。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；线程持续运行。
 */
void swc_ota_task(void *p_argument)
{
    (void)p_argument;

    for (;;)
    {
        // 1. 接收一个文件会话，接收过程中通过队列等待让出处理器。
        Ymodem_Receive(g_au8_YmodemRec);

        // 2. 会话返回后短暂延时，再开始下一轮接收。
        vTaskDelay(pdMS_TO_TICKS(10U));
    }
}
/****************************** Functions ******************************/
