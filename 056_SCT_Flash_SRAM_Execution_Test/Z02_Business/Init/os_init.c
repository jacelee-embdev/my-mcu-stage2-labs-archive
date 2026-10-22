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
 * - sct_test_task.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Creates application tasks.
 *
 * Processing flow:
 *
 * Create status_task and sct_test_task once before scheduler startup.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "os_init.h"      /* 业务层：任务初始化 */

#include <stddef.h>       /* C 标准库：NULL */

#include "FreeRTOS.h"     /* 中间件层：FreeRTOS、日志 */
#include "task.h"
#include "debug.h"

#include "status_task.h"  /* 业务层：状态任务、SCT 测试任务 */
#include "sct_test_task.h"

/****************************** Includes ******************************/

/****************************** Defines ******************************/

#define TAG  "os_init"  /* 日志标识 */

/****************************** Defines ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 创建业务任务。
 *
 * 处理步骤：
 * 1. 创建状态任务并记录结果，失败时返回。
 * 2. 创建 SCT 测试任务并记录结果，失败时返回。
 *
 * @return 无。
 *
 * @note LED、日志初始化后，调度器启动前调用一次。
 *       栈深度按 StackType_t 元素计。
 */
void OS_Init(void)
{
    /* 1. 创建状态任务；移除时同步删除包含和创建代码。 */
    if (pdPASS != xTaskCreate(Status_TaskEntry,
                             "status_task",
                             128U,  /* 栈深度，单位：元素 */
                             NULL,
                             1U,    /* 任务优先级 */
                             NULL))
    {
        elog_e(TAG, "Failed to create status task.\r\n");
        return;
    }

    /* 2. 创建 SCT 测试任务，具体测试代码在独立任务目录中维护。 */
    if (pdPASS != xTaskCreate(SCT_Test_TaskEntry,
                             "sct_test_task",
                             128U,  /* 栈深度，单位：元素 */
                             NULL,
                             1U,    /* 任务优先级 */
                             NULL))
    {
        elog_e(TAG, "Failed to create SCT test task.\r\n");
        return;
    }

    elog_i(TAG, "OS resources created successfully.\r\n");
}

/************************ Function Definitions ************************/
