/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file led_task.c
 *
 * @par dependencies
 * - led_task.h
 * - stddef.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 * - system_adaption.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Repeatedly tests board-LED blink periods and on/off ratios.
 *
 * Processing flow:
 *
 * Initialize system LED resources after scheduler startup.
 * Submit requests with varying periods and ratios for the handler to execute.
 * Wait between test groups, or end this task on failure.
 *
 * @version V1.0 2026-11-03
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "led_task.h"             /* Module header                            */

#include <stddef.h>               /* C standard library                       */

#include "FreeRTOS.h"             /* Middleware                               */
#include "task.h"
#include "debug.h"

#include "system_adaption.h"      /* User modules                             */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "led_task"            /* Log tag.                                 */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Initialize LED resources and repeat asynchronous blink tests.
 *
 * Processing steps:
 * 1. Construct and register LED resources through the system adapter.
 * 2. Use a loop factor to test 400, 800 and 1200 ms blink periods.
 * 3. For each period, test on/off ratios 1:1, 1:2 and 1:3, three blinks each.
 * 4. Wait for each group's nominal duration plus 2000 ms before submitting
 *    another request; log errors and delete this task on failure.
 *
 * @param[in] p_argument : Unused task argument; may be NULL.
 *
 * @return None; repeat tests continuously, or delete this task on failure.
 *
 * @note Created by OS_Init() and executed after scheduler startup.
 *       LED hardware and logging must already be initialized.
 *       Submission success only means queued; check Handler logs for execution
 *       results.
 *       The system adapter owns LED resources; this business task does not
 *       release background resources.
 */
void LED_TaskEntry(void * const p_argument)
{
    /* Test each on/off ratio through the existing System control API. */
    static const led_blink_mode_t s_blink_modes[] =
    {
        LED_BLINK_1_1,
        LED_BLINK_1_2,
        LED_BLINK_1_3
    };
    const uint32_t blink_times = 3U;  /* Blinks per test group.               */
    const uint32_t mode_count =
        sizeof(s_blink_modes) / sizeof(s_blink_modes[0]);
    /* Initialization or submission result. */
    system_status_t ret = SYSTEM_OK;

    (void)p_argument;             /* Task argument is unused.                 */

    /*********** 1. Initialize system resources ***********/
    ret = System_InitResources();
    if (SYSTEM_OK != ret)
    {
        elog_e(TAG, "LED resource initialization failed: status=%d.\r\n",
               (int)ret);
        vTaskDelete(NULL);
        return;
    }

    /************ 2. Repeat blink test groups *************/
    for (;;)
    {
        for (uint32_t cycle_factor = 1U; cycle_factor <= 3U; cycle_factor++)
        {
            /* Scale the period while keeping each request within API limits. */
            const uint32_t cycle_time_ms = cycle_factor * 400U;

            for (uint32_t mode_index = 0U; mode_index < mode_count;
                 mode_index++)
            {
                /***** 2.1 Submit *****/
                ret = System_LEDControl(cycle_time_ms, blink_times,
                                        s_blink_modes[mode_index]);
                if (SYSTEM_OK != ret)
                {
                    elog_e(TAG, "LED request submission failed: status=%d.\r\n",
                           (int)ret);
                    vTaskDelete(NULL);
                    return;
                }
                elog_i(TAG, "LED blink test queued: cycle=%lu ms, "
                       "times=%lu, mode=%d.\r\n",
                       (unsigned long)cycle_time_ms,
                       (unsigned long)blink_times,
                       (int)s_blink_modes[mode_index]);

                /****** 2.2 Wait ******/
                /* Allow for execution and the Handler's polling interval. */
                vTaskDelay(pdMS_TO_TICKS(cycle_time_ms * blink_times + 2000U));
            }
        }
    }
}

/**************************** Function Definitions ****************************/
