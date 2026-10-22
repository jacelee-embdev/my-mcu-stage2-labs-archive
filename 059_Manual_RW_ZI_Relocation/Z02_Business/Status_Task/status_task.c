/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file status_task.c
 *
 * @par dependencies
 * - status_task.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 * - led.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the LED and logging test.
 *
 * Processing flow:
 *
 * Run Status_TaskEntry() as a FreeRTOS task.
 * Toggle LED, log status, then delay 500 ticks.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "status_task.h"  /* 业务层：状态任务 */

#include "FreeRTOS.h"     /* 中间件层：FreeRTOS、日志 */
#include "task.h"
#include "debug.h"

#include "led.h"          /* 用户 BSP 层：LED 控制 */

/****************************** Includes ******************************/

/****************************** Defines ******************************/

#define TAG  "status_task"  /* 日志标识 */

/****************************** Defines ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 运行状态测试任务。
 *
 * 处理步骤：
 * 1. 翻转 LED。
 * 2. 输出状态日志。
 * 3. 延时 500 个节拍后重复。
 *
 * @param[in] p_argument : 任务参数，未使用，可为空。
 *
 * @return 无；任务不返回。
 *
 * @note 运行前完成 LED_Init() 和日志初始化。
 *       任务模板：每个任务使用独立目录及配套 .c/.h。
 */
void Status_TaskEntry(void *p_argument)
{
    (void)p_argument;

    for (;;)
    {
        /* 1. 翻转 LED。 */
        LED_Toggle();

        /* 2. 输出状态日志。 */
        elog_i(TAG, "Status_Task running...\r\n");

        /* 3. 延时 500 个节拍。 */
        vTaskDelay(500);
    }
}

/************************ Function Definitions ************************/
