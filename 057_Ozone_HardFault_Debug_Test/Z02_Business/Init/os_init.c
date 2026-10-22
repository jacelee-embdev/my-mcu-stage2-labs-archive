/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file os_init.c
 *
 * @par dependencies
 * - os_init.h
 * - stddef.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 * - status_task.h
 * - fault_test_task.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Creates application tasks.
 *
 * Processing flow:
 *
 * Create status_task and fault_test_task once before scheduler startup.
 * Stop on task creation failure; otherwise log successful resource creation.
 *
 * @version 1.0 2026-10-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "os_init.h"          /* 业务层：任务初始化 */

#include <stddef.h>           /* C 标准库：NULL */

#include "FreeRTOS.h"         /* 中间件层：FreeRTOS、日志 */
#include "task.h"
#include "debug.h"

#include "status_task.h"      /* 业务层：应用任务 */
#include "fault_test_task.h"

/****************************** Includes ******************************/

/****************************** Defines ******************************/

#define TAG  "os_init"  /* 日志标识 */

/****************************** Defines ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 创建业务任务。
 *
 * 处理步骤：
 * 1. 创建状态任务；失败时记录错误并返回。
 * 2. 创建故障测试任务；失败时记录错误并返回。
 * 3. 两个任务均创建成功时，输出资源创建成功日志。
 *
 * @return 无返回值。
 *
 * @note LED、日志初始化后，调度器启动前调用一次。
 *       两个任务的栈深度均为 128 个 StackType_t 元素，优先级均为 1。
 *       创建失败时保留此前已创建的任务，不执行回滚。
 */
void OS_Init(void)
{
    /* 1. 创建状态任务，并检查创建结果。 */
    if (pdPASS != xTaskCreate(Status_TaskEntry,
                              "status_task",
                              128U,  /* 栈深度：StackType_t 元素数 */
                              NULL,  /* 无任务参数 */
                              1U,    /* 任务优先级 */
                              NULL)) /* 不保存任务句柄 */
    {
        /* 创建失败时记录错误并返回。 */
        elog_e(TAG, "Failed to create status task.\r\n");
        return;
    }

    /* 2. 创建故障测试任务，并检查创建结果。 */
    if (pdPASS != xTaskCreate(Fault_Test_TaskEntry,
                              "fault_test_task",
                              128U,  /* 栈深度：StackType_t 元素数 */
                              NULL,  /* 无任务参数 */
                              1U,    /* 任务优先级 */
                              NULL)) /* 不保存任务句柄 */
    {
        /* 创建失败时记录错误并返回。 */
        elog_e(TAG, "Failed to create fault test task.\r\n");
        return;
    }

    /* 3. 资源创建成功，等待调度器启动。 */
    elog_i(TAG, "OS resources created successfully.\r\n");
}

/************************ Function Definitions ************************/
