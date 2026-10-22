/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file fault_test_task.c
 *
 * @par dependencies
 * - fault_test_task.h
 * - stdio.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the divide-by-zero test.
 *
 * Processing flow:
 *
 * Created by OS_Init().
 * Log start, then run fault_test().
 * On return, log end and delay 1000 ticks.
 *
 * @version V1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "fault_test_task.h"  /* 业务层：故障测试 */

#include <stdio.h>            /* C 标准库 */

#include "FreeRTOS.h"         /* 中间件层：FreeRTOS、日志 */
#include "task.h"
#include "debug.h"

/****************************** Includes ******************************/

/****************************** Defines ******************************/

#define TAG  "fault_test_task"  /* 日志标识 */

/****************************** Defines ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 整数除零测试。
 *
 * 处理步骤：
 * 1. 使能 DIV_0_TRP。
 * 2. 执行 10 / 0。
 * 3. 若继续执行，输出结果。
 *
 * @return 无。
 *
 * @note 异常未恢复时不返回。
 */
static void fault_test(void)
{
    /* SCB_CCR 寄存器地址 */
    volatile int *SCB_CCR = (volatile int *)0xE000ED14;

    int x, y, z;  /* 被除数、除数、结果 */

    /* 1. 使能除零陷阱（bit 4）。 */
    *SCB_CCR |= (1 << 4);

    /* 2. 执行 10 / 0。 */
    x = 10;
    y = 0;
    z = x / y;

    /* 3. 若继续执行，打印结果。 */
    elog_i(TAG, "z=%d\r\n", z);
}

/**
 * @brief 除零异常测试任务。
 *
 * 处理步骤：
 * 1. 打印开始日志。
 * 2. 执行除零测试。
 * 3. 若返回，打印结束日志并延时。
 *
 * @param[in] p_argument : 未使用，可为 NULL。
 *
 * @return 无；不返回。
 *
 * @note 启动前初始化日志和 CmBacktrace。
 *       异常未恢复时，后续流程不执行。
 */
void Fault_Test_TaskEntry(void *p_argument)
{
    (void)p_argument;

    for (;;)
    {
        /* 1. 打印开始日志。 */
        elog_i(TAG, "Test begin.\r\n");

        /* 2. 执行 10 / 0。 */
        fault_test();

        /* 3. 若返回，打印日志并延时。 */
        elog_i(TAG, "Test over.\r\n");
        vTaskDelay(1000U);
    }
}

/************************ Function Definitions ************************/
