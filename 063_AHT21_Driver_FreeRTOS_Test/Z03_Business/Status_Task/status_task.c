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

/********************************** Includes **********************************/

#include "status_task.h"           /* Module header                           */

#include "FreeRTOS.h"              /* Middleware                              */
#include "task.h"
#include "debug.h"

#include "led.h"                   /* User modules                            */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG  "status_task"         /* Log tag.                                */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Run the status LED and logging test.
 *
 * Processing steps:
 * 1. Toggle the status LED.
 * 2. Log a status message.
 * 3. Delay for 500 RTOS ticks and repeat.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @return None. The task runs indefinitely.
 *
 * @note Initialize the LED and logging before starting this task.
 *       Keep each task in its own directory with its matching .c and .h files.
 */
void Status_TaskEntry(void *p_argument)
{
    (void)p_argument;

    for (;;)
    {
        /************ 1. Toggle the status LED ************/
        LED_Toggle();

        /************ 2. Log a status message *************/
        elog_i(TAG, "Status_Task running...\r\n");

        /************* 3. Delay for 500 ticks *************/
        vTaskDelay(500);
    }
}

/**************************** Function Definitions ****************************/
