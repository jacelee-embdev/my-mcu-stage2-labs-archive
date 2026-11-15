/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aht21_task.c
 *
 * @par dependencies
 * - aht21_task.h
 * - stddef.h
 * - main.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 * - iic.h
 * - sw_bsp_aht21_driver.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Reads and logs AHT21 measurements from a FreeRTOS task.
 *
 * Processing flow:
 *
 * Configure the software IIC bus from the board's SDA and SCL definitions.
 * Bind HAL_GetTick, the FreeRTOS critical-section functions, and vTaskDelay.
 * Initialize the driver once, then read and log temperature and humidity.
 * Wait two seconds after each read attempt.
 *
 * @version v1.0 2026-11-09
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "aht21_task.h"           /* Module header                            */

#include <stddef.h>               /* C standard library                       */

#include "main.h"                 /* Board pin definitions                    */

#include "FreeRTOS.h"             /* Middleware                               */
#include "task.h"
#include "debug.h"

#include "iic.h"                  /* User modules                             */
#include "sw_bsp_aht21_driver.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "aht21_task"

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

static iic_bus_t sg_iic_bus =
{
    .p_sda_port = SDA_GPIO_Port,
    .p_scl_port = SCL_GPIO_Port,
    .sda_pin    =       SDA_Pin,
    .scl_pin    =       SCL_Pin,
};

/********************************* Declaring **********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Initialize the AHT21 and log periodic temperature and humidity.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @note Start after GPIO, the bus delay counter, and logging are initialized.
 *       On initialization failure, the task logs the error and deletes itself.
 *       A failed measurement is logged before the next two-second wait.
 */
void AHT21_TaskEntry(void *p_argument)
{
    (void)p_argument;

    /*********** 1. Bind the platform interfaces **********/

    static const aht21_iic_interface_t iic_interface =
    {
        .p_bus             =        &sg_iic_bus,
        .pf_critical_enter = vPortEnterCritical,
        .pf_critical_exit  =  vPortExitCritical,
    };
    static const aht21_timebase_interface_t timebase =
    {
        .pf_get_tick_count = HAL_GetTick,
    };
    static const aht21_rtos_delay_interface_t rtos_delay =
    {
        .pf_rtos_delay = vTaskDelay,
    };

    /************** 2. Instantiate the AHT21 **************/

    bsp_aht21_driver_t driver = {0};
    aht21_status_t result =
        AHT21_Instantiate(&iic_interface, &driver, &timebase, &rtos_delay);
    if (AHT21_OK != result)
    {
        elog_e(TAG, "AHT21 initialization failed (%d).", (int)result);
        vTaskDelete(NULL);
        return;
    }

    /************ 3. Read and log measurements ************/

    for (;;)
    {
        float temperature_c         = 0.0f;
        float relative_humidity_pct = 0.0f;

        result = driver.pf_aht21_read_temperature_humidity(
            &driver, &temperature_c, &relative_humidity_pct);
        if (AHT21_OK == result)
        {
            elog_i(TAG, "Temperature %.2f C, humidity %.2f %%RH.",
                   (double)temperature_c, (double)relative_humidity_pct);
        }
        else
        {
            elog_e(TAG, "AHT21 measurement failed (%d).", (int)result);
        }

        vTaskDelay(pdMS_TO_TICKS(2000U));
    }
}

/**************************** Function Definitions ****************************/
