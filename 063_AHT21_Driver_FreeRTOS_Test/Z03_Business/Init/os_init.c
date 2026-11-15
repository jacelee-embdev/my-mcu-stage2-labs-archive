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
 * - aht21_task.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Creates application tasks.
 *
 * Processing flow:
 *
 * Create status_task and aht21_task before scheduler startup.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "os_init.h"               /* Module header                           */

#include <stddef.h>                /* C standard library                      */

#include "FreeRTOS.h"              /* Middleware                              */
#include "task.h"
#include "debug.h"

#include "status_task.h"           /* User modules                            */
#include "aht21_task.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG  "os_init"             /* Log tag.                                */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Create the status and AHT21 tasks.
 *
 * @par Processing steps
 * 1. Create the status task.
 * 2. Create the AHT21 task.
 * 3. Log successful creation.
 *
 * @return None.
 *
 * @note Call once after LED and logging initialization and before starting
 *       the scheduler.
 *       Log the error and return if either task fails to be created.
 *       The task stack depth is measured in StackType_t elements.
 */
void OS_Init(void)
{
    /************* 1. Create the status task **************/
    if (pdPASS != xTaskCreate(Status_TaskEntry,
                              "status_task",
                              128U,  /* Stack depth in elements. */
                              NULL,
                              1U,    /* Task priority. */
                              NULL))
    {
        elog_e(TAG, "Failed to create status task.\r\n");
        return;
    }

    /************** 2. Create the AHT21 task **************/

    if (pdPASS != xTaskCreate(AHT21_TaskEntry,
                              "aht21_task",
                              384U,  /* Stack depth in elements. */
                              NULL,
                              1U,    /* Task priority. */
                              NULL))
    {
        elog_e(TAG, "Failed to create AHT21 task.\r\n");
        return;
    }

    /*********** 3. Report successful creation ************/
    elog_i(TAG, "OS resources created successfully.\r\n");
}

/**************************** Function Definitions ****************************/
