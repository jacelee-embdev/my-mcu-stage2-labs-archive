/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file swc_ota.h
 *
 * @par dependencies
 * - FreeRTOS.h
 * - task.h
 * - ymodem.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the YMODEM receive task.
 *
 * Processing flow:
 *
 * Create swc_ota_task() as an RTOS task during initialization.
 *
 * @version v1.0 2026-10-02
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SWC_OTA_H
#define __SWC_OTA_H

/******************************* Includes *******************************/
#include "FreeRTOS.h"
#include "task.h"
#include "ymodem.h"
/******************************* Includes *******************************/

/************************ Function Declarations ************************/
/**
 * @brief 运行使用 SRAM 包缓冲区的 YMODEM 接收线程。
 *
 * 循环处理接收会话，并在每次会话返回后延时 10 毫秒。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；线程持续运行。
 */
void swc_ota_task(void *p_argument);
/************************ Function Declarations ************************/

#endif /* __SWC_OTA_H */
