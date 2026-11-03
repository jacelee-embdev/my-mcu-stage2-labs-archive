/*******************************************************************************
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
 * - led_task.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Creates application tasks.
 *
 * Processing flow:
 *
 * Create status_task and led_task once before scheduler startup.
 *
 * @version v1.0 2026-10-28
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "os_init.h"              /* Module header                            */

#include <stddef.h>               /* C standard library                       */

#include "FreeRTOS.h"             /* Middleware                               */
#include "task.h"
#include "debug.h"

#include "status_task.h"          /* User modules                             */
#include "led_task.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "os_init"             /* Log tag.                                 */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Create the application tasks before scheduler startup.
 *
 * Processing steps:
 * 1. Create the status task without retaining its handle.
 * 2. Create the LED test task without retaining its handle.
 * 3. Log and return on failure, or report that both tasks are ready.
 *
 * @return None.
 *
 * @note Call once after LED and logging initialization, before scheduler start.
 *       Stack depth is measured in StackType_t elements.
 *       A creation failure leaves previously created tasks in place.
 */
void OS_Init(void)
{
    /*************** 1. Create status task ****************/
    if (pdPASS != xTaskCreate(Status_TaskEntry,
                             "status_task",
                             128U,   /* Stack: 128 words, 512 bytes.          */
                             NULL,   /* No task argument.                     */
                             1U,     /* Task priority.                        */
                             NULL))  /* Do not retain the task handle.        */
    {
        /* Report the failure without rolling back earlier tasks. */
        elog_e(TAG, "Failed to create status task.\r\n");
        return;
    }

    /***************** 2. Create LED task *****************/
    if (pdPASS != xTaskCreate(LED_TaskEntry,
                             "led_task",
                             256U,   /* Stack: 256 words, 1024 bytes.         */
                             NULL,   /* No task argument.                     */
                             1U,     /* Task priority.                        */
                             NULL))  /* Do not retain the task handle.        */
    {
        elog_e(TAG, "Failed to create LED task.\r\n");
        return;
    }

    /**************** 3. Report readiness *****************/
    elog_i(TAG, "OS resources created successfully.\r\n");
}

/**************************** Function Definitions ****************************/
