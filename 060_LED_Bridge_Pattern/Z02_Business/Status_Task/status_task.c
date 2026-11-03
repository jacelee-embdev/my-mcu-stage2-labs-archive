/*******************************************************************************
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
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the periodic status logging task.
 *
 * Processing flow:
 *
 * Run Status_TaskEntry() as a FreeRTOS task.
 * Log the task status, then wait 5000 ticks before repeating.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "status_task.h"          /* Module header                            */

#include "FreeRTOS.h"             /* Middleware                               */
#include "task.h"
#include "debug.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG                     "status_task"  /* Log tag.                    */
#define STATUS_TASK_DELAY_TICKS (5000U)        /* Status interval in ticks.   */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Log the task status at a fixed tick interval.
 *
 * Processing steps:
 * 1. Output the status message.
 * 2. Wait 5000 ticks, then repeat.
 *
 * @param[in] p_argument : Unused task argument; may be NULL.
 *
 * @return None; the task runs continuously.
 *
 * @note Initialize logging before this task starts.
 */
void Status_TaskEntry(void * const p_argument)
{
    (void)p_argument;

    for (;;)
    {
        /*************** 1. Log task status ***************/
        elog_i(TAG, "Status_Task running...\r\n");

        /************** 2. Wait for next log **************/
        vTaskDelay(STATUS_TASK_DELAY_TICKS);
    }
}

/**************************** Function Definitions ****************************/
