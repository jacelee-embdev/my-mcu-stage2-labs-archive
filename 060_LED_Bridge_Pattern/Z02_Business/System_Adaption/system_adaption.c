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
 * - string.h
 * - stm32f4xx_hal.h
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - debug.h
 * - led.h
 * - bsp_led_handler.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Binds LED interfaces and manages marked BSP objects in RAM.
 *
 * Processing flow:
 *
 * Keep marked Driver and Handler initial values in the bsp_target section.
 * Copy the initial values from Flash to RAM through startup scatter loading.
 * Bind interfaces, create OS resources, and register the onboard LED.
 * Submit indexed requests and release resources on initialization failure.
 *
 * @version V1.0 2026-11-03
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "system_adaption.h"      /* Module header                            */

#include <stddef.h>               /* C standard library                       */
#include <string.h>

#include "stm32f4xx_hal.h"        /* MCU SDK / HAL                            */

#include "FreeRTOS.h"             /* Middleware                               */
#include "task.h"
#include "queue.h"
#include "debug.h"

#include "led.h"                  /* User modules                             */
#include "bsp_led_handler.h"

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#ifndef OS_SUPPORT
#error "System adaption requires OS_SUPPORT."
#endif  /* OS_SUPPORT */

/*
 * The marker locates BSP initial values: is_inited starts at 0xEC and other
 * members start at zero. Successful instantiation sets the flag to 1. The
 * marker helps observe state; interface binding and resource creation still run
 * at runtime.
 */
#define INIT_PATTERN_SYSTEM ((uint8_t)0xECU)

#define TAG "system_adaption"     /* Log tag.                                 */

/********************************** Defines ***********************************/

/**************************** Variable Definitions ****************************/

/*
 * The scatter file places both object initial values in bsp_target. Before
 * main(), startup copies them from Flash to their RAM execution region. The
 * first member starts at 0xEC and the rest at zero; runtime updates the RAM
 * objects.
 */
/* Board LED Driver: blink parameters and bound interfaces. */
__attribute__((used, section("bsp_target")))
static bsp_led_driver_t sg_led_driver = {INIT_PATTERN_SYSTEM};
/* Handler: registration table and current OS resource handles. */
__attribute__((used, section("bsp_target")))
static bsp_led_handler_t sg_led_handler = {INIT_PATTERN_SYSTEM};
/* Registered LED index. */
static led_index_t       sg_led_index   =   LED_NOT_INITIALIZED;

/**************************** Variable Definitions ****************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Convert a Handler result to the corresponding System status.
 *
 * @param[in] handler_status : Result returned by a Handler interface.
 *
 * @return system_status_t : Matching System status, or SYSTEM_ERROR for an
 *         unknown Handler value.
 */
static system_status_t System_ConvertHandlerStatus(
                                    const led_handler_status_t   handler_status
                                                                               )
{
    switch (handler_status)
    {
        case HANDLER_OK:
            return SYSTEM_OK;
        case HANDLER_ERROR:
            return SYSTEM_ERROR;
        case HANDLER_ERROR_TIMEOUT:
            return SYSTEM_ERROR_TIMEOUT;
        case HANDLER_ERROR_RESOURCE:
            return SYSTEM_ERROR_RESOURCE;
        case HANDLER_ERROR_PARAMETER:
            return SYSTEM_ERROR_PARAMETER;
        case HANDLER_ERROR_NOMEMORY:
            return SYSTEM_ERROR_NOMEMORY;
        case HANDLER_ERROR_ISR:
            return SYSTEM_ERROR_ISR;
        case HANDLER_RESERVED:
            return SYSTEM_RESERVED;
        default:
            return SYSTEM_ERROR;
    }
}

/**
 * @brief Provide the Driver millisecond delay callback.
 *
 * @param[in] delay_ms : Requested delay in milliseconds.
 *
 * @return led_status_t : LED_OK after the delay call returns.
 */
static led_status_t System_LEDDelayMs(const uint32_t delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
    return LED_OK;
}

/**
 * @brief Provide the Handler millisecond delay callback.
 *
 * @param[in] delay_ms : Requested delay in milliseconds.
 *
 * @return led_handler_status_t : HANDLER_OK after the delay call returns.
 */
static led_handler_status_t System_HandlerDelayMs(const uint32_t delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
    return HANDLER_OK;
}

/**
 * @brief Read the HAL millisecond count into the output.
 *
 * @param[out] p_time_ms : Valid pointer receiving the millisecond count.
 *
 * @return led_handler_status_t : Success or parameter error.
 */
static led_handler_status_t System_HandlerGetTimeMs(uint32_t * const p_time_ms)
{
    if (NULL == p_time_ms)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    *p_time_ms = HAL_GetTick();
    return HANDLER_OK;
}

/**
 * @brief Create a FreeRTOS queue and output its handle.
 *
 * @param[in]  item_num       : Queue capacity; must be greater than zero.
 * @param[in]  item_size      : Item size in bytes; must be greater than zero.
 * @param[out] p_queue_handle : Receives the handle; NULL on creation failure.
 *
 * @return led_handler_status_t : Success, parameter error or insufficient
 *         memory.
 */
static led_handler_status_t System_QueueCreate(
                                    const uint32_t                     item_num,
                                    const uint32_t                    item_size,
                                    void           ** const      p_queue_handle
                                                                               )
{
    QueueHandle_t p_created_queue = NULL;  /* FreeRTOS queue handle.          */

    /*************** 1. Validate parameters ***************/
    if (NULL == p_queue_handle)
    {
        return HANDLER_ERROR_PARAMETER;
    }
    *p_queue_handle = NULL;

    if (0U == item_num || 0U == item_size)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    /****************** 2. Create queue *******************/
    p_created_queue = xQueueCreate((UBaseType_t)item_num,
                                (UBaseType_t)item_size);
    if (NULL == p_created_queue)
    {
        return HANDLER_ERROR_NOMEMORY;
    }

    /************* 3. Return the queue handle *************/
    *p_queue_handle = p_created_queue;
    return HANDLER_OK;
}

/**
 * @brief Copy one message into a FreeRTOS queue.
 *
 * @param[in] p_queue_handle : Valid queue handle.
 * @param[in] p_item         : Address of the message to copy.
 * @param[in] timeout_ticks  : Maximum wait in ticks; zero does not wait.
 *
 * @return led_handler_status_t : Success, parameter error or timeout.
 */
static led_handler_status_t System_QueuePut(
                                    void           * const       p_queue_handle,
                                    const void     * const               p_item,
                                    const uint32_t                timeout_ticks
                                                                               )
{
    if (NULL == p_queue_handle || NULL == p_item)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    return (pdPASS == xQueueSend((QueueHandle_t)p_queue_handle, p_item,
                                (TickType_t)timeout_ticks))
               ? HANDLER_OK : HANDLER_ERROR_TIMEOUT;
}

/**
 * @brief Copy one queued message into the receiving buffer.
 *
 * @param[in]  p_queue_handle : Valid queue handle.
 * @param[out] p_msg          : Buffer large enough for one configured queue
 *                              item.
 * @param[in]  timeout_ticks  : Maximum wait in ticks; zero does not wait.
 *
 * @return led_handler_status_t : Success, parameter error or timeout.
 */
static led_handler_status_t System_QueueGet(
                                    void           * const       p_queue_handle,
                                    void           * const                p_msg,
                                    const uint32_t                timeout_ticks
                                                                               )
{
    if (NULL == p_queue_handle || NULL == p_msg)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    return (pdPASS == xQueueReceive((QueueHandle_t)p_queue_handle, p_msg,
                                   (TickType_t)timeout_ticks))
               ? HANDLER_OK : HANDLER_ERROR_TIMEOUT;
}

/**
 * @brief Delete a FreeRTOS queue after all users have stopped.
 *
 * @param[in] p_queue_handle : Valid queue handle.
 *
 * @return led_handler_status_t : Success or parameter error.
 *
 * @note Threads using the queue must be stopped; successful deletion
 *       invalidates the handle.
 */
static led_handler_status_t System_QueueDelete(void * const p_queue_handle)
{
    if (NULL == p_queue_handle)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    vQueueDelete((QueueHandle_t)p_queue_handle);
    return HANDLER_OK;
}

/**
 * @brief Enter the registration-table critical section in task context.
 *
 * @return led_handler_status_t : HANDLER_OK after entry.
 *
 * @note Access shared data only; pair this call with the exit operation.
 */
static led_handler_status_t System_CriticalEnter(void)
{
    taskENTER_CRITICAL();
    return HANDLER_OK;
}

/**
 * @brief Leave the registration-table critical section in task context.
 *
 * @return led_handler_status_t : HANDLER_OK after exit.
 */
static led_handler_status_t System_CriticalExit(void)
{
    taskEXIT_CRITICAL();
    return HANDLER_OK;
}

/**
 * @brief Create a FreeRTOS worker and output its handle.
 *
 * @param[in]  pf_thread_entry : Valid thread entry function.
 * @param[in]  p_thread_name   : Valid thread name.
 * @param[in]  stack_depth     : Stack element count representable by the OS
 *                               depth type.
 * @param[in]  p_argument      : Context whose lifetime is managed by the
 *                               caller.
 * @param[in]  priority        : FreeRTOS priority below the configured limit.
 * @param[out] p_thread_handle : Receives the handle; NULL on creation failure.
 *
 * @return led_handler_status_t : Success, parameter error or insufficient
 *         memory.
 */
static led_handler_status_t System_ThreadCreate(
                       const pf_handler_thread_entry_t          pf_thread_entry,
                       const char                       * const   p_thread_name,
                       const uint32_t                               stack_depth,
                       void                             * const      p_argument,
                       const uint32_t                                  priority,
                       void                            ** const p_thread_handle
                                                                               )
{
    TaskHandle_t p_created_thread =   NULL;  /* FreeRTOS thread handle.       */
    BaseType_t   ret              = pdFAIL;  /* FreeRTOS creation result.     */

    /*************** 1. Validate parameters ***************/
    if (NULL == p_thread_handle)
    {
        return HANDLER_ERROR_PARAMETER;
    }
    *p_thread_handle = NULL;

    if (NULL == pf_thread_entry || NULL == p_thread_name ||
        0U == stack_depth ||
        stack_depth != (uint32_t)(configSTACK_DEPTH_TYPE)stack_depth ||
        priority >= (uint32_t)configMAX_PRIORITIES)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    /****************** 2. Create worker ******************/
    ret = xTaskCreate(pf_thread_entry,
                      p_thread_name,
                      (configSTACK_DEPTH_TYPE)stack_depth,
                      p_argument,
                      (UBaseType_t)priority,
                      &p_created_thread);
    if (pdPASS != ret)
    {
        return HANDLER_ERROR_NOMEMORY;
    }

    /************ 3. Return the worker handle *************/
    *p_thread_handle = p_created_thread;
    return HANDLER_OK;
}

/**
 * @brief Delete the specified FreeRTOS worker thread.
 *
 * @param[in] p_thread_handle : Valid handle of the thread to delete.
 *
 * @return led_handler_status_t : Success or parameter error.
 *
 * @note The resource-construction task deletes the worker it created.
 */
static led_handler_status_t System_ThreadDelete(void * const p_thread_handle)
{
    if (NULL == p_thread_handle)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    vTaskDelete((TaskHandle_t)p_thread_handle);
    return HANDLER_OK;
}

/**
 * @brief Release resources from failed construction and clear the objects.
 *
 * @return system_status_t : Cleanup result; deletion errors are converted to
 *         System status.
 *
 * @note No business requests have been submitted; delete the worker before its
 *       queue.
 *       On deletion failure, retain the handle and return the error; clear both
 *       objects only after all cleanup succeeds.
 *       Clearing objects does not restore the INIT_PATTERN_SYSTEM marker.
 */
static system_status_t System_ReleaseLEDResources(void)
{
    led_handler_status_t ret = HANDLER_OK;  /* Resource deletion result.      */

    /*********** 1. Mark resources unavailable ************/
    sg_led_handler.is_inited =  HANDLER_NOT_INITED;
    sg_led_index             = LED_NOT_INITIALIZED;

    /************** 2. Release OS resources ***************/

    /******* 2.1 Delete worker ********/
    if (NULL != sg_led_handler.p_thread_handle)
    {
        ret = System_ThreadDelete(sg_led_handler.p_thread_handle);
        if (HANDLER_OK != ret)
        {
            return System_ConvertHandlerStatus(ret);
        }
        sg_led_handler.p_thread_handle = NULL;
    }

    /******** 2.2 Delete queue ********/
    if (NULL != sg_led_handler.p_queue_handle)
    {
        ret = System_QueueDelete(sg_led_handler.p_queue_handle);
        if (HANDLER_OK != ret)
        {
            return System_ConvertHandlerStatus(ret);
        }
        sg_led_handler.p_queue_handle = NULL;
    }

    /*************** 3. Clear both objects ****************/
    memset(&sg_led_handler, 0, sizeof(sg_led_handler));
    memset(&sg_led_driver, 0, sizeof(sg_led_driver));
    return SYSTEM_OK;
}

/**
 * @brief Construct Driver and Handler in RAM and register the board LED.
 *
 * Processing steps:
 * 1. Check initialization state and residual resources, bind the delay and
 *    instantiate Driver.
 * 2. Create the Handler queue and worker, then register Driver and save its
 *    index.
 * 3. Roll back failures; clear both objects after successful cleanup.
 *
 * @return system_status_t : Resource initialization result.
 * @retval SYSTEM_OK              LED resources were constructed and registered.
 * @retval SYSTEM_ERROR           Driver construction failed; its status is
 *                                logged.
 * @retval SYSTEM_ERROR_RESOURCE  Handler already initialized or still owns OS
 *                                resources.
 * @retval SYSTEM_ERROR_NOMEMORY  Queue or thread allocation failed.
 * @retval SYSTEM_ERROR_PARAMETER Invalid construction or registration
 *                                parameter.
 *
 * @note Startup copies the object initial values from Flash to RAM: first
 *       member 0xEC, all others zero. Both flags become 1 after successful
 *       instantiation.
 *       Call once, sequentially from the startup task, after LED hardware and
 *       logging initialization.
 *       The scheduler must be running; Driver construction includes a 600 ms
 *       task delay.
 *       This function still performs interface binding, OS resource creation
 *       and registration at runtime.
 *       Cleanup failures retain valid handles and return the cleanup error.
 *       Handler results are converted to System status before returning.
 *       Objects and interface tables have static storage and remain valid
 *       during background use.
 */
system_status_t System_InitResources(void)
{
    /*************** 1. Prepare interfaces ****************/

    /***** 1.1 Driver interfaces ******/
    static const led_ops_t s_led_ops =
    {
        .pf_led_on     =     LED_ON,
        .pf_led_off    =    LED_OFF,
        .pf_led_toggle = LED_Toggle
    };
    static const time_base_t s_driver_time =
    {
        .pf_get_timestamp_ms = HAL_GetTick
    };

    /***** 1.2 Handler interfaces *****/
    static const handler_time_base_ms_t s_handler_time =
    {
        .pf_get_time_ms = System_HandlerGetTimeMs
    };
    static const handler_os_delay_t s_os_delay =
    {
        .pf_os_delay_ms = System_HandlerDelayMs
    };
    static const handler_os_queue_t s_os_queue =
    {
        .pf_os_queue_create = System_QueueCreate,
        .pf_os_queue_put    =    System_QueuePut,
        .pf_os_queue_get    =    System_QueueGet,
        .pf_os_queue_delete = System_QueueDelete
    };
    static const handler_os_critical_t s_os_critical =
    {
        .pf_os_critical_enter = System_CriticalEnter,
        .pf_os_critical_exit  =  System_CriticalExit
    };
    static const handler_os_thread_t s_os_thread =
    {
        .pf_os_thread_create = System_ThreadCreate,
        .pf_os_thread_delete = System_ThreadDelete
    };
    /* System resource initialization result. */
    system_status_t      ret         =  SYSTEM_OK;
    /* Failure cleanup result. */
    system_status_t      cleanup_ret =  SYSTEM_OK;
    /* Handler construction or registration result. */
    led_handler_status_t handler_ret = HANDLER_OK;
    /* Driver construction result. */
    led_status_t         driver_ret  =     LED_OK;

    /**** 1.3 Check resource state ****/
    if (HANDLER_INITED == sg_led_handler.is_inited ||
        NULL != sg_led_handler.p_thread_handle ||
        NULL != sg_led_handler.p_queue_handle)
    {
        return SYSTEM_ERROR_RESOURCE;
    }

    /************ 2. Instantiate board Driver *************/
    sg_led_driver.pf_delay_ms =      System_LEDDelayMs;
    driver_ret                = LEDDriver_Instantiate(
        &sg_led_driver, &s_led_ops, &s_driver_time);
    if (LED_OK != driver_ret)
    {
        elog_e(TAG, "LED driver construction failed: status=%d.\r\n",
               (int)driver_ret);
        ret = SYSTEM_ERROR;
        goto release_resources;
    }

    /*************** 3. Instantiate Handler ***************/
    handler_ret = LEDHandler_Instantiate(
        &sg_led_handler, &s_os_delay, &s_os_queue, &s_os_critical,
        &s_os_thread, &s_handler_time);
    if (HANDLER_OK != handler_ret)
    {
        ret = System_ConvertHandlerStatus(handler_ret);
        goto release_resources;
    }

    /*************** 4. Register board LED ****************/
    handler_ret = sg_led_handler.pf_led_register(
        &sg_led_handler, &sg_led_driver, &sg_led_index);
    if (HANDLER_OK != handler_ret)
    {
        ret = System_ConvertHandlerStatus(handler_ret);
        goto release_resources;
    }

    elog_i(TAG, "LED system resources ready: index=%d.\r\n",
           (int)sg_led_index);
    return SYSTEM_OK;

release_resources:

    /*************** 5. Roll back resources ***************/
    cleanup_ret = System_ReleaseLEDResources();
    if (SYSTEM_OK != cleanup_ret)
    {
        ret = cleanup_ret;
    }
    elog_e(TAG, "LED system initialization failed: status=%d.\r\n",
           (int)ret);
    return ret;
}

/**
 * @brief Submit a board-LED blink request to the Handler.
 *
 * @param[in] cycle_time_ms : Cycle duration in ms; must be below 10000.
 * @param[in] blink_times   : Blink count; must be below 1000.
 * @param[in] blink_mode    : On/off duration ratio.
 *
 * @return system_status_t : Validation and queue submission result.
 * @retval SYSTEM_OK              Queued for background execution.
 * @retval SYSTEM_ERROR_TIMEOUT   The message queue is full.
 * @retval SYSTEM_ERROR_RESOURCE  LED resources are not ready.
 * @retval SYSTEM_ERROR_PARAMETER Invalid duration, count or blink mode.
 *
 * @note Task context only; System_InitResources() must have succeeded first.
 *       Submission does not wait; the Handler logs the execution result.
 *       Handler results are converted to System status before returning.
 *       Zero duration or count is allowed and performs no LED output
 *       operations.
 */
system_status_t System_LEDControl(
                                    const uint32_t                cycle_time_ms,
                                    const uint32_t                  blink_times,
                                    const led_blink_mode_t           blink_mode
                                                                               )
{
    /* Handler queue submission result. */
    led_handler_status_t handler_ret = HANDLER_OK;

    if (HANDLER_INITED != sg_led_handler.is_inited ||
        NULL == sg_led_handler.pf_led_control ||
        LED_NOT_INITIALIZED == sg_led_index)
    {
        return SYSTEM_ERROR_RESOURCE;
    }

    handler_ret = sg_led_handler.pf_led_control(
        &sg_led_handler, cycle_time_ms, blink_times, blink_mode, sg_led_index);
    return System_ConvertHandlerStatus(handler_ret);
}

/**************************** Function Definitions ****************************/
