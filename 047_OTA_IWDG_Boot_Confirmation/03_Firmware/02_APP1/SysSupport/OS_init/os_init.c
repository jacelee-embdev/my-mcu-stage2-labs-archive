/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file os_init.c
 *
 * @par dependencies
 * - os_init.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Creates the receive-length queue and application tasks.
 *
 * Processing flow:
 *
 * Call os_init() once before starting the scheduler.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************* Includes *********************************/
#include "os_init.h"
/********************************* Includes *********************************/

/********************************* Defines **********************************/
#define TAG  "os_init"  /* 日志模块标识。 */
/********************************* Defines **********************************/

/******************************** Variables *********************************/
/* 串口中断向接收任务传递数据长度的队列。 */
QueueHandle_t g_YmodemReclength_queue = NULL;
/******************************** Variables *********************************/

/******************************** Functions *********************************/
/**
 * @brief 创建串口接收长度队列和两个应用线程。
 *
 * 1. 创建容量为一、元素为 uint16_t 的接收长度队列。
 * 2. 依次创建接收线程和业务演示线程。
 * 3. 任一步创建失败时记录日志并返回，保留已创建的资源。
 *
 * @return 无。
 */
void os_init(void)
{
    // 1. 创建接收长度队列，数据本身保存在接收缓冲区中。
    g_YmodemReclength_queue = xQueueCreate(1U, sizeof(uint16_t));
    if (NULL == g_YmodemReclength_queue)
    {
        elog_e(TAG, "Queue creation failed.\r\n");
        return;
    }

    // 2. 创建接收线程，栈深度按 StackType_t 元素数计。
    if (pdPASS != xTaskCreate(swc_ota_task,
                              "swc_ota",
                              512U,
                              NULL,
                              1U,
                              NULL))
    {
        elog_e(TAG, "Swc ota task creation failed.\r\n");
        return;
    }

    // 3. 创建业务演示线程。
    if (pdPASS != xTaskCreate(app_task,
                              "app",
                              128U,
                              NULL,
                              1U,
                              NULL))
    {
        elog_e(TAG, "App task creation failed.\r\n");
        return;
    }

    elog_i(TAG, "OS resource initialized successfully.\r\n");
}
/******************************** Functions *********************************/
