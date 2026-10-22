/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file os_init.h
 *
 * @par dependencies
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - debug.h
 * - swc_ota.h
 * - app.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the RTOS initializer and receive-length queue.
 *
 * Processing flow:
 *
 * Call os_init() once during RTOS initialization.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __OS_INIT_H__
#define __OS_INIT_H__

/********************************* Includes *********************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "debug.h"
#include "swc_ota.h"
#include "app.h"
/********************************* Includes *********************************/

/**************************** External Variables ****************************/
/* 串口接收长度通知队列，由初始化函数创建。 */
extern QueueHandle_t g_YmodemReclength_queue;
/**************************** External Variables ****************************/

/************************** Function Declarations ***************************/
/**
 * @brief 创建接收长度队列、接收线程和业务演示线程。
 *
 * 在调度器启动前调用一次；创建失败时记录日志并返回。
 *
 * @return 无。
 */
void os_init(void);
/************************** Function Declarations ***************************/

#endif /* 结束头文件保护。 */
