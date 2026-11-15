/*******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aht21_task.c
 *
 * @par dependencies
 * - aht21_task.h
 * - stdint.h
 * - stddef.h
 * - main.h
 * - FreeRTOS.h
 * - task.h
 * - cmsis_os2.h
 * - debug.h
 * - sw_bsp_aht21_handler.h
 * - system_adaption.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Submits joint AHT21 measurement requests and logs callback results.
 *
 * Processing flow:
 *
 * Explicitly configure and initialize resources, then create the worker.
 * Submit joint requests and wait two seconds after each submission.
 * Log readings, scale the stored temperature and update diagnostic counters.
 *
 * @version v1.0 2026-11-14
 *
 * @note 1 tab == 4 spaces!
 *       Ported from userTaskFunction and temp_humi_callback in freertos.c.
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "aht21_task.h"            /* Module header                           */

#include <stdint.h>                /* C standard library                      */
#include <stddef.h>

#include "main.h"                  /* Board configuration                     */

#include "FreeRTOS.h"              /* Middleware                              */
#include "task.h"
#include "cmsis_os2.h"
#include "debug.h"

#include "sw_bsp_aht21_handler.h"  /* User modules                            */
#include "system_adaption.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "aht21_task"          /* Log tag.                                 */

static float    sg_temperature __attribute__((unused)) =   0.0f;
static uint32_t sg_dwt_count                           =     1U;
static uint32_t sg_dwt_test __attribute__((unused))    = 0x123U;

static TaskHandle_t sg_handler_task_handle = NULL;

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Log the delivered readings and scale temperature.
 *
 * @param[in] p_temperature : Temperature delivered by the worker.
 * @param[in] p_humidity    : Relative humidity delivered by the worker.
 *
 * @note Stores 1.5 times the delivered temperature in sg_temperature.
 */
static void AHT21_TemperatureHumidityCallback(
                                                          float * p_temperature,
                                                          float *    p_humidity
                                                                               )
{
    elog_d(TAG, "callback:temperature=%f, humidity=%f",
           (double)*p_temperature, (double)*p_humidity);

    sg_temperature = (*p_temperature) * 1.5f;
}

/**
 * @brief Submit periodic joint temperature and humidity reading requests.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @note Initialize resources before creating the worker with NULL input.
 *       Requests use a 10000 ms freshness threshold and a 2000 ms delay.
 */
void AHT21_TaskEntry(void *p_argument)
{
    (void)p_argument;

    /*************** 1. Configure resources ***************/
    const system_aht21_config_t config =
    {
        .iic_bus =
        {
            .p_sda_port = SDA_GPIO_Port,
            .p_scl_port = SCL_GPIO_Port,
            .sda_pin    =       SDA_Pin,
            .scl_pin    =       SCL_Pin,
        },
        .timebase_interface =
        {
            .pf_get_tick_count = HAL_GetTick,
        },
        .rtos_delay_interface =
        {
            .pf_rtos_delay = vTaskDelay,
        },
        .pf_critical_enter = vPortEnterCritical,
        .pf_critical_exit  =  vPortExitCritical,
        .pf_os_delay_ms    =         vTaskDelay,
    };

    temp_humi_status_t result = System_AHT21ResourcesInit(&config);
    if (TEMP_HUMI_OK != result)
    {
        elog_e(TAG, "AHT21 resource initialization failed (%d).", (int)result);
        vTaskDelete(NULL);
        return;
    }

    /************ 2. Create the handler worker ************/
    if (pdPASS != xTaskCreate(AHT21_HandlerThreadEntry,
                              "tempHandlerTask",
                              192U,  /* Stack depth in elements.              */
                              NULL,
                              (UBaseType_t)osPriorityNormal,
                              &sg_handler_task_handle))
    {
        elog_e(TAG, "Failed to create AHT21 handler task.");
        vTaskDelete(NULL);
        return;
    }

    /************ 3. Submit periodic requests *************/
    temp_humi_event_t event =
    {
        .lifetime   =                            10000U,
        .type       =              BOTH_TEMP_HUMI_EVENT,
        .pfcallback = AHT21_TemperatureHumidityCallback,
    };

    for (;;)
    {
        elog_d(TAG, "AHT21_TaskEntry start");

        AHT21_HandlerRequestReadTemperatureHumidity(&event);
        sg_dwt_count++;
        if (3U == sg_dwt_count)
        {
            sg_dwt_test = 0x234U;
        }
        vTaskDelay(pdMS_TO_TICKS(2000U));
    }
}

/**************************** Function Definitions ****************************/
