/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file system_adaption.c
 *
 * @par dependencies
 * - system_adaption.h
 * - stddef.h
 * - FreeRTOS.h
 * - queue.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Binds the AHT21 handler to the board and FreeRTOS interfaces.
 *
 * Processing flow:
 *
 * Keep the IIC bus and injected interface tables in static storage.
 * Adapt queue creation, sending and receiving to handler callbacks.
 * Copy caller configuration and instantiate resources through the setup API.
 *
 * @version v1.0 2026-11-14
 *
 * @note 1 tab == 4 spaces!
 *       Interface integration ported from the reference freertos.c.
 *       The business task uses vTaskDelay with a 1 kHz FreeRTOS tick.
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "system_adaption.h"  /* Module header                                */

#include <stddef.h>           /* C standard library                           */

#include "FreeRTOS.h"         /* Middleware                                   */
#include "queue.h"
#include "debug.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "system_adaption"     /* Log tag.                                 */

static iic_bus_t                                sg_iic_bus              = {0};
static aht21_iic_interface_t                    sg_iic_interface        = {0};
static aht21_rtos_delay_interface_t             sg_rtos_delay_interface = {0};
static aht21_timebase_interface_t               sg_timebase_interface   = {0};
static temp_humi_handler_os_interface_t         sg_os_interface         = {0};
static temp_humi_handler_dependency_injection_t sg_input_arg            = {0};

static bsp_aht21_driver_t      sg_aht21_driver  = {0};
static bsp_temp_humi_handler_t sg_aht21_handler = {0};

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Create a FreeRTOS queue and output its handle.
 *
 * @param[in]  item_num       : Maximum number of queued events.
 * @param[in]  item_size      : Size of one event in bytes.
 * @param[out] p_queue_handle : Receives the created queue handle.
 *
 * @return temp_humi_status_t : Queue creation result.
 * @retval TEMP_HUMI_OK              The queue was created.
 * @retval TEMP_HUMI_ERROR_PARAMETER The queue could not be created.
 */
static temp_humi_status_t System_QueueCreate(
                                         uint32_t const                item_num,
                                         uint32_t const               item_size,
                                         void           ** const p_queue_handle
                                                                               )
{
    *p_queue_handle = xQueueCreate(item_num, item_size);
    elog_d(TAG, "queue_handle addr=%p", *p_queue_handle);
    if (NULL == *p_queue_handle)
    {
        return TEMP_HUMI_ERROR_PARAMETER;
    }
    else
    {
        return TEMP_HUMI_OK;
    }
}

/**
 * @brief Adapt the handler's queue send operation to FreeRTOS.
 *
 * @param[in] p_queue_handle : Previously created queue.
 * @param[in] p_item         : One event buffer.
 * @param[in] timeout        : Maximum wait in FreeRTOS ticks.
 *
 * @return temp_humi_status_t : Queue send result.
 * @retval TEMP_HUMI_OK       The send operation succeeded.
 * @retval TEMP_HUMI_RESERVED The send operation failed.
 */
static temp_humi_status_t System_QueueSend(
                                                void     * const p_queue_handle,
                                                void     * const         p_item,
                                                uint32_t                timeout
                                                                               )
{
    if (pdPASS == xQueueSend((QueueHandle_t)p_queue_handle,
                            p_item, (TickType_t)timeout))
    {
        return TEMP_HUMI_OK;
    }
    else
    {
        return TEMP_HUMI_RESERVED;
    }
}

/**
 * @brief Adapt the handler's queue receive operation to FreeRTOS.
 *
 * @param[in]  p_queue_handle : Previously created queue.
 * @param[out] p_msg          : One event buffer.
 * @param[in]  timeout        : Maximum wait in FreeRTOS ticks.
 *
 * @return temp_humi_status_t : Queue receive result.
 * @retval TEMP_HUMI_OK       The receive operation succeeded.
 * @retval TEMP_HUMI_RESERVED The receive operation failed.
 */
static temp_humi_status_t System_QueueReceive(
                                                      void     * p_queue_handle,
                                                      void     *          p_msg,
                                                      uint32_t          timeout
                                                                               )
{
    if (pdPASS == xQueueReceive((QueueHandle_t)p_queue_handle,
                               p_msg, (TickType_t)timeout))
    {
        return TEMP_HUMI_OK;
    }
    else
    {
        return TEMP_HUMI_RESERVED;
    }
}

/**
 * @brief Copy configuration and initialize the handler's queue and driver.
 *
 * @param[in] p_config : Caller-supplied platform configuration.
 *
 * @return temp_humi_status_t : Configuration or handler initialization result.
 * @retval TEMP_HUMI_OK              The mounted handler is ready.
 * @retval TEMP_HUMI_ERROR_PARAMETER The configuration pointer is NULL.
 *
 * @note All injected objects use static storage. The caller's configuration
 *       object is copied and is not retained after this call.
 *       Call once from a running task before creating the worker.
 */
temp_humi_status_t System_AHT21ResourcesInit(
                                   const system_aht21_config_t * const p_config
                                                                               )
{

    /*************** 1. Check configuration ***************/
    if (NULL == p_config)
    {
        return TEMP_HUMI_ERROR_PARAMETER;
    }

    /************ 2. Bind platform interfaces *************/
    sg_iic_bus                         =              p_config->iic_bus;
    sg_iic_interface.p_bus             =                    &sg_iic_bus;
    sg_iic_interface.pf_critical_enter =    p_config->pf_critical_enter;
    sg_iic_interface.pf_critical_exit  =     p_config->pf_critical_exit;
    sg_timebase_interface              =   p_config->timebase_interface;
    sg_rtos_delay_interface            = p_config->rtos_delay_interface;

    sg_os_interface.os_delay_ms      = p_config->pf_os_delay_ms;
    sg_os_interface.os_queue_create  =       System_QueueCreate;
    sg_os_interface.os_queue_send    =         System_QueueSend;
    sg_os_interface.os_queue_receive =      System_QueueReceive;

    sg_input_arg.iic_driver_interface =        &sg_iic_interface;
    sg_input_arg.timebase_interface   =   &sg_timebase_interface;
    sg_input_arg.os_interface         =         &sg_os_interface;
    sg_input_arg.yield_interface      = &sg_rtos_delay_interface;

    /************** 3. Instantiate and mount **************/
    sg_aht21_handler.p_aht21_instance = &sg_aht21_driver;
    return AHT21_HandlerInstantiate(&sg_aht21_handler, &sg_input_arg);
}

/**************************** Function Definitions ****************************/
