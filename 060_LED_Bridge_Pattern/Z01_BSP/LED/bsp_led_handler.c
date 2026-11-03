/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_handler.c
 *
 * @par dependencies
 * - bsp_led_handler.h
 * - stddef.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements LED registration and asynchronous request handling.
 *
 * Processing flow:
 *
 * Bind interfaces, clear the registration table, and create OS resources.
 * Register LED drivers and submit indexed control requests to the queue.
 * Receive requests in the worker thread and execute indexed LED blinking.
 * Access LED hardware through the callbacks bound to each driver object.
 *
 * @version v1.0 2026-11-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "bsp_led_handler.h"      /* Module header                            */

#include <stddef.h>               /* C standard library                       */

#include "debug.h"                /* Middleware                               */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "bsp_led_handler"     /* Log tag.                                 */
#ifndef DEBUG
#define DEBUG (1U)                /* Enable debug output.                     */
#endif  /* DEBUG */

#ifdef OS_SUPPORT
#define LED_HANDLER_QUEUE_LENGTH       (10U)          /* Queue item count.    */
#define LED_HANDLER_POLL_INTERVAL_MS   (1000U)        /* Poll interval (ms).  */
#define LED_HANDLER_THREAD_NAME        "led_handler"  /* Worker thread name.  */
#define LED_HANDLER_THREAD_STACK_DEPTH (256U)         /* Worker stack depth.  */
#define LED_HANDLER_THREAD_PRIORITY    (1U)           /* Worker priority.     */

typedef struct
{
    uint32_t         cycle_time_ms;  /* Cycle duration in ms.                 */
    uint32_t           blink_times;  /* Blink count.                          */
    led_blink_mode_t    blink_mode;  /* On/off duration ratio.                */
    led_index_t          led_index;  /* Registered LED index.                 */
} led_handler_event_t;  /* Queue-copied control request */
#endif  /* OS_SUPPORT */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Clear every slot in an LED driver pointer array.
 *
 * @param[out] p_array    : Non-NULL pointer array to clear.
 * @param[in]  array_size : Element count, not a byte count.
 *
 * @return led_handler_status_t : Array initialization result.
 * @retval HANDLER_OK              Every slot was set to NULL.
 * @retval HANDLER_ERROR_PARAMETER The array pointer is NULL.
 *
 * @note Allocates no memory and does not initialize the referenced drivers.
 *       The caller provides space for at least array_size elements.
 */
static led_handler_status_t LEDHandler_ArrayInit(
                                    bsp_led_driver_t *                p_array[],
                                    const uint32_t                   array_size
                                                                               )
{
    /**************** 1. Clear array slots ****************/
    if (NULL == p_array)
    {
#if DEBUG
        elog_e(TAG, "Invalid parameter.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    for (uint32_t i = 0U; i < array_size; i++)
    {
        p_array[i] = NULL;
    }

    return HANDLER_OK;
}

/**
 * @brief Register an initialized LED driver and assign its index.
 *
 * Processing steps:
 * 1. Check pointers, object states and critical-section callbacks.
 * 2. Check capacity, store the driver address and assign a slot inside the
 *    critical section.
 * 3. Leave the critical section before logging the result.
 *
 * @param[in,out] p_self       : Initialized Handler object.
 * @param[in]     p_led_driver : Initialized driver that remains valid after
 *                               registration.
 * @param[out]    p_index      : Assigned slot, starting at LED_1 (zero).
 *
 * @return led_handler_status_t : Registration result.
 * @retval HANDLER_OK              Driver registered; critical-section callbacks
 *                                 succeeded.
 * @retval HANDLER_ERROR_PARAMETER Invalid pointer, Handler state or critical
 *                                 interface.
 * @retval HANDLER_ERROR_RESOURCE  Driver not initialized or registration table
 *                                 full.
 *
 * @note OS_SUPPORT builds also propagate critical-section callback errors.
 *       If leaving the critical section fails, the table and output index may
 *       already be updated.
 *       Slots are assigned sequentially; duplicate addresses receive new
 *       indexes.
 */
static led_handler_status_t LEDHandler_Register(
                                    bsp_led_handler_t * const            p_self,
                                    bsp_led_driver_t  * const      p_led_driver,
                                    led_index_t       * const           p_index
                                                                               )
{
    led_handler_status_t ret = HANDLER_OK;  /* Operation result.              */
#ifdef OS_SUPPORT
    /* Critical-section exit result. */
    led_handler_status_t critical_ret = HANDLER_OK;
#endif  /* OS_SUPPORT */

#if DEBUG
    elog_i(TAG, "LED registration starting.\r\n");
#endif  /* DEBUG */

    /************** 1. Validate registration **************/
    if (NULL == p_self || NULL == p_led_driver || NULL == p_index)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED registration parameter.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    if (HANDLER_INITED != p_self->is_inited)
    {
#if DEBUG
        elog_e(TAG, "LED handler is not initialized.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    if (INITIALIZED != p_led_driver->is_inited)
    {
#if DEBUG
        elog_e(TAG, "LED driver is not initialized.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_RESOURCE;
    }

#ifdef OS_SUPPORT
    if (NULL == p_self->p_os_critical ||
        NULL == p_self->p_os_critical->pf_os_critical_enter ||
        NULL == p_self->p_os_critical->pf_os_critical_exit)
    {
#if DEBUG
        elog_e(TAG, "Invalid registration critical-section interface.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    ret = p_self->p_os_critical->pf_os_critical_enter();
    if (HANDLER_OK != ret)
    {
#if DEBUG
        elog_e(TAG,
               "Failed to enter the LED registration critical section.\r\n");
#endif  /* DEBUG */
        return ret;
    }
#endif  /* OS_SUPPORT */

    /******** 2. Register inside critical section *********/
    if (MAX_INSTANCE_NUMBER <= p_self->instances.led_instance_num)
    {
        ret = HANDLER_ERROR_RESOURCE;
    }
    else
    {
        p_self->instances.led_instance_group[
            p_self->instances.led_instance_num] = p_led_driver;
        *p_index = (led_index_t)p_self->instances.led_instance_num;
        p_self->instances.led_instance_num++;
    }

#ifdef OS_SUPPORT
    critical_ret = p_self->p_os_critical->pf_os_critical_exit();
    if (HANDLER_OK != critical_ret)
    {
#if DEBUG
        elog_e(TAG,
               "Failed to exit the LED registration critical section.\r\n");
#endif  /* DEBUG */
        return critical_ret;
    }
#endif  /* OS_SUPPORT */

    /*********** 3. Report registration result ************/
    if (HANDLER_OK != ret)
    {
#if DEBUG
        elog_e(TAG, "LED registration table is full.\r\n");
#endif  /* DEBUG */
        return ret;
    }

#if DEBUG
    elog_i(TAG, "LED registration successful, index=%d.\r\n", (int)*p_index);
#endif  /* DEBUG */
    return HANDLER_OK;
}

/**
 * @brief Submit one indexed LED control request without waiting.
 *
 * Processing steps:
 * 1. Check Handler state, queue resources and control parameters.
 * 2. Verify the registered index inside the critical section.
 * 3. Build the complete event and enqueue it outside the critical section.
 *
 * @param[in,out] p_self        : Initialized Handler object.
 * @param[in]     cycle_time_ms : Cycle duration in ms; must be below 10000.
 * @param[in]     blink_times   : Blink count; must be below 1000.
 * @param[in]     blink_mode    : On/off duration ratio: 1:3, 1:2 or 1:1.
 * @param[in]     led_index     : Index returned by this Handler during
 *                                registration.
 *
 * @return led_handler_status_t : Validation and queue submission result.
 * @retval HANDLER_OK              The complete request was copied into the
 *                                 queue.
 * @retval HANDLER_ERROR_PARAMETER Invalid pointer, control parameter or LED
 *                                 index.
 * @retval HANDLER_ERROR_RESOURCE  Handler or required OS resources are not
 *                                 ready.
 *
 * @note Zero duration or count is allowed and performs no LED output
 *       operations.
 *       Success means queued; the worker executes requests in queue order.
 *       Execution results are logged by the worker, not returned synchronously.
 *       Critical-section and queue errors are propagated from the adapters.
 *       Without OS_SUPPORT, asynchronous submission returns a resource error.
 *       Registration, submission and resource cleanup run in task context.
 */
static led_handler_status_t LEDHandler_Control(
                                   bsp_led_handler_t      * const        p_self,
                                   const uint32_t                 cycle_time_ms,
                                   const uint32_t                   blink_times,
                                   const led_blink_mode_t            blink_mode,
                                   const led_index_t                  led_index
                                                                               )
{
#ifdef OS_SUPPORT
    /* Request submission result. */
    led_handler_status_t ret          = HANDLER_OK;
    /* Critical-section exit result. */
    led_handler_status_t critical_ret = HANDLER_OK;
    led_handler_event_t  event        =        {0};  /* Outgoing queue event. */
#endif  /* OS_SUPPORT */

    /************* 1. Check Handler resources *************/
    if (NULL == p_self)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED handler control target.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    if (HANDLER_INITED != p_self->is_inited)
    {
#if DEBUG
        elog_e(TAG, "LED handler is not initialized.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_RESOURCE;
    }

#ifdef OS_SUPPORT
    if (NULL == p_self->p_queue_handle ||
        NULL == p_self->p_os_queue_interface ||
        NULL == p_self->p_os_queue_interface->pf_os_queue_put ||
        NULL == p_self->p_os_critical ||
        NULL == p_self->p_os_critical->pf_os_critical_enter ||
        NULL == p_self->p_os_critical->pf_os_critical_exit)
    {
#if DEBUG
        elog_e(TAG, "LED control resources are not ready.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_RESOURCE;
    }
#endif  /* OS_SUPPORT */

    /*********** 2. Validate control parameters ***********/
    if (cycle_time_ms >= 10000U || blink_times >= 1000U ||
        !(LED_BLINK_1_3 == blink_mode ||
          LED_BLINK_1_2 == blink_mode ||
          LED_BLINK_1_1 == blink_mode) ||
        (uint32_t)led_index >= (uint32_t)MAX_INSTANCE_NUMBER)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED control parameter.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

#ifdef OS_SUPPORT

    /************* 3. Check registered index **************/
    ret = p_self->p_os_critical->pf_os_critical_enter();
    if (HANDLER_OK != ret)
    {
#if DEBUG
        elog_e(TAG, "Failed to enter the control critical section.\r\n");
#endif  /* DEBUG */
        return ret;
    }

    if ((uint32_t)led_index >= p_self->instances.led_instance_num ||
        NULL == p_self->instances.led_instance_group[led_index])
    {
        ret = HANDLER_ERROR_PARAMETER;
    }

    critical_ret = p_self->p_os_critical->pf_os_critical_exit();
    if (HANDLER_OK != critical_ret)
    {
#if DEBUG
        elog_e(TAG, "Failed to exit the control critical section.\r\n");
#endif  /* DEBUG */
        return critical_ret;
    }

    if (HANDLER_OK != ret)
    {
#if DEBUG
        elog_e(TAG, "LED control index is not registered.\r\n");
#endif  /* DEBUG */
        return ret;
    }

    /************* 4. Build and enqueue event *************/

    /****** 4.1 Copy parameters *******/
    event.cycle_time_ms = cycle_time_ms;
    event.blink_times   =   blink_times;
    event.blink_mode    =    blink_mode;
    event.led_index     =     led_index;

    /******** 4.2 Submit event ********/
    ret = p_self->p_os_queue_interface->pf_os_queue_put(
        p_self->p_queue_handle, &event, 0U);
    if (HANDLER_OK != ret)
    {
#if DEBUG
        elog_e(TAG, "LED request enqueue failed, status=%d.\r\n", (int)ret);
#endif  /* DEBUG */
        return ret;
    }

#if DEBUG
    elog_i(TAG, "LED request queued: index=%d, cycle=%lu ms, "
           "times=%lu, mode=%d.\r\n",
           (int)led_index, (unsigned long)cycle_time_ms,
           (unsigned long)blink_times, (int)blink_mode);
#endif  /* DEBUG */
    return HANDLER_OK;
#else
    return HANDLER_ERROR_RESOURCE;
#endif  /* OS_SUPPORT */
}

#ifdef OS_SUPPORT
/**
 * @brief Execute one blink request through the driver callbacks.
 *
 * Processing steps:
 * 1. Check the driver, initialization state and required callbacks.
 * 2. Snapshot the parameters and calculate on-time using integer division.
 * 3. Delay and update the LED output for each millisecond of every cycle.
 *
 * @param[in,out] p_led_driver : Driver with validated blink parameters already
 *                               stored.
 *
 * @return led_handler_status_t : Blink execution result.
 * @retval HANDLER_OK              All requested cycles completed.
 * @retval HANDLER_ERROR_PARAMETER Invalid object, callback or blink mode.
 * @retval HANDLER_ERROR_RESOURCE  The driver is not initialized.
 * @retval HANDLER_ERROR           An LED output or delay callback failed.
 *
 * @note Called only by the Handler worker in task context; execution includes
 *       delays.
 *       The event processor validates duration and count; either may be zero.
 *       Uses Driver hardware and delay callbacks, independently of its
 *       synchronous control method.
 *       Blinking runs outside the registration critical section.
 */
static led_handler_status_t LEDHandler_Blink(
                                    const bsp_led_driver_t * const p_led_driver
                                                                               )
{
    /* Cycle duration in ms. */
    uint32_t         cycle_time_ms  =                    0U;
    uint32_t         blink_times    =                    0U;  /* Blink count. */
    /* On/off duration ratio. */
    led_blink_mode_t blink_mode     = LED_BLINK_UNSPECIFIED;
    /* On-time within one cycle. */
    uint32_t         led_on_time_ms =                    0U;
    /* Driver callback result. */
    led_status_t     driver_ret     =                LED_OK;

    /************* 1. Check Driver callbacks **************/
    if (NULL == p_led_driver)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    if (INITIALIZED != p_led_driver->is_inited)
    {
        return HANDLER_ERROR_RESOURCE;
    }

    if (NULL == p_led_driver->pf_delay_ms ||
        NULL == p_led_driver->p_led_ops_inst ||
        NULL == p_led_driver->p_led_ops_inst->pf_led_on ||
        NULL == p_led_driver->p_led_ops_inst->pf_led_off)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    /************** 2. Prepare blink timing ***************/

    /**** 2.1 Snapshot parameters *****/
    cycle_time_ms = p_led_driver->one_iteration_time_ms;
    blink_times   =           p_led_driver->blink_times;
    blink_mode    =        p_led_driver->led_blink_mode;

    /***** 2.2 Calculate on-time ******/
    if (LED_BLINK_1_1 == blink_mode)
    {
        led_on_time_ms = cycle_time_ms / 2U;
    }
    else if (LED_BLINK_1_2 == blink_mode)
    {
        led_on_time_ms = cycle_time_ms / 3U;
    }
    else if (LED_BLINK_1_3 == blink_mode)
    {
        led_on_time_ms = cycle_time_ms / 4U;
    }
    else
    {
        return HANDLER_ERROR_PARAMETER;
    }

    /******** 3. Execute outside critical section *********/
    for (uint32_t i = 0U; i < blink_times; i++)
    {
        for (uint32_t j = 0U; j < cycle_time_ms; j++)
        {
            driver_ret = p_led_driver->pf_delay_ms(1U);
            if (LED_OK != driver_ret)
            {
#if DEBUG
                elog_e(TAG, "LED delay failed, status=%d.\r\n",
                       (int)driver_ret);
#endif  /* DEBUG */
                return HANDLER_ERROR;
            }

            if (j < led_on_time_ms)
            {
                driver_ret = p_led_driver->p_led_ops_inst->pf_led_on();
            }
            else
            {
                driver_ret = p_led_driver->p_led_ops_inst->pf_led_off();
            }

            if (LED_OK != driver_ret)
            {
#if DEBUG
                elog_e(TAG, "LED output failed, status=%d.\r\n",
                       (int)driver_ret);
#endif  /* DEBUG */
                return HANDLER_ERROR;
            }
        }
    }

    return HANDLER_OK;
}

/**
 * @brief Validate an event, find its driver and execute the request.
 *
 * Processing steps:
 * 1. Check the event, index bounds and critical-section callbacks.
 * 2. Look up the registered driver inside the critical section.
 * 3. Leave the critical section, check Driver state, then save and run the
 *    request.
 *
 * @param[in,out] p_self  : Initialized Handler object.
 * @param[in]     p_event : Non-NULL complete control event read from the queue.
 *
 * @return led_handler_status_t : Event validation and execution result.
 * @retval HANDLER_OK              The request completed.
 * @retval HANDLER_ERROR_PARAMETER Invalid pointer, parameter or registration
 *                                 index.
 * @retval HANDLER_ERROR_RESOURCE  Handler, Driver or critical interface is not
 *                                 ready.
 * @retval HANDLER_ERROR           An LED output or delay callback failed.
 *
 * @note Critical-section callback errors are propagated; no logging or delays
 *       occur inside it.
 *       Duration must be below 10000 ms and count below 1000; both may be zero.
 */
static led_handler_status_t LEDHandler_ProcessEvent(
                                    bsp_led_handler_t         * const    p_self,
                                    const led_handler_event_t * const   p_event
                                                                               )
{
    /* Driver for this request. */
    bsp_led_driver_t     * p_led_driver =       NULL;
    /* Critical-section and lookup result. */
    led_handler_status_t   ret          = HANDLER_OK;
    /* Critical-section exit result. */
    led_handler_status_t   critical_ret = HANDLER_OK;

    /************ 1. Validate event and index *************/
    if (NULL == p_self || NULL == p_event)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    if (HANDLER_INITED != p_self->is_inited)
    {
        return HANDLER_ERROR_RESOURCE;
    }

    if (p_event->cycle_time_ms >= 10000U || p_event->blink_times >= 1000U ||
        !(LED_BLINK_1_3 == p_event->blink_mode ||
          LED_BLINK_1_2 == p_event->blink_mode ||
          LED_BLINK_1_1 == p_event->blink_mode) ||
        (uint32_t)p_event->led_index >= (uint32_t)MAX_INSTANCE_NUMBER)
    {
        return HANDLER_ERROR_PARAMETER;
    }

    if (NULL == p_self->p_os_critical ||
        NULL == p_self->p_os_critical->pf_os_critical_enter ||
        NULL == p_self->p_os_critical->pf_os_critical_exit)
    {
        return HANDLER_ERROR_RESOURCE;
    }

    /***************** 2. Look up Driver ******************/
    ret = p_self->p_os_critical->pf_os_critical_enter();
    if (HANDLER_OK != ret)
    {
        return ret;
    }

    if ((uint32_t)p_event->led_index >= p_self->instances.led_instance_num ||
        NULL == p_self->instances.led_instance_group[p_event->led_index])
    {
        ret = HANDLER_ERROR_PARAMETER;
    }
    else
    {
        p_led_driver = p_self->instances.led_instance_group[p_event->led_index];
    }

    critical_ret = p_self->p_os_critical->pf_os_critical_exit();
    if (HANDLER_OK != critical_ret)
    {
        return critical_ret;
    }

    if (HANDLER_OK != ret)
    {
        return ret;
    }

    /*********** 3. Update and execute request ************/
    if (NULL == p_led_driver || INITIALIZED != p_led_driver->is_inited)
    {
        return HANDLER_ERROR_RESOURCE;
    }

    p_led_driver->one_iteration_time_ms = p_event->cycle_time_ms;
    p_led_driver->blink_times           =   p_event->blink_times;
    p_led_driver->led_blink_mode        =    p_event->blink_mode;

    return LEDHandler_Blink(p_led_driver);
}

/**
 * @brief Receive queued control events and execute them sequentially.
 *
 * Processing steps:
 * 1. Assert that the constructor supplied a valid context and dependencies.
 * 2. Read at most one event without waiting, log it and process it.
 * 3. Log execution errors; an empty queue is normal, other receive errors are
 *    logged.
 * 4. Delay 1000 ms through the injected interface before checking again.
 *
 * @param[in] p_argument : Handler object with its queue and interfaces ready.
 *
 * @return None; the worker normally runs continuously.
 *
 * @note This private entry is used only by the constructor; invalid internal
 *       dependencies trigger assertions.
 *       Prepare all dependencies before creating the worker; submit requests
 *       only after instantiation succeeds.
 *       Requests for one Handler are serialized; the next event is received
 *       after the current one finishes.
 *       A failed event is logged and does not stop later requests.
 *       Delay errors are logged, then receiving and delaying are retried.
 */
static void LEDHandler_ThreadEntry(void * const p_argument)
{
    bsp_led_handler_t    * const p_self = p_argument;  /* Owning Handler.     */
    /* Receive, execution or delay result. */
    led_handler_status_t         ret    = HANDLER_OK;
    /* Incoming queue event. */
    led_handler_event_t          event  =        {0};

    /*********** 1. Verify worker dependencies ************/
    ELOG_ASSERT(NULL != p_self);
    ELOG_ASSERT(NULL != p_self->p_queue_handle);
    ELOG_ASSERT(NULL != p_self->p_os_queue_interface);
    ELOG_ASSERT(NULL != p_self->p_os_queue_interface->pf_os_queue_get);
    ELOG_ASSERT(NULL != p_self->p_os_time_delay);
    ELOG_ASSERT(NULL != p_self->p_os_time_delay->pf_os_delay_ms);

#if DEBUG
    elog_i(TAG, "LED handler thread started.\r\n");
#endif  /* DEBUG */

    for (;;)
    {
        /******** 2. Receive and process one event ********/
        ret = p_self->p_os_queue_interface->pf_os_queue_get(
            p_self->p_queue_handle, &event, 0U);
        if (HANDLER_OK == ret)
        {
#if DEBUG
            elog_i(TAG, "LED request received: index=%d, cycle=%lu ms, "
                   "times=%lu, mode=%d.\r\n",
                   (int)event.led_index,
                   (unsigned long)event.cycle_time_ms,
                   (unsigned long)event.blink_times,
                   (int)event.blink_mode);
#endif  /* DEBUG */
            ret = LEDHandler_ProcessEvent(p_self, &event);
            if (HANDLER_OK != ret)
            {
#if DEBUG
                elog_e(TAG, "LED request processing failed: index=%d, "
                       "status=%d.\r\n", (int)event.led_index, (int)ret);
#endif  /* DEBUG */
            }
            else
            {
#if DEBUG
                elog_i(TAG, "LED request completed: index=%d.\r\n",
                       (int)event.led_index);
#endif  /* DEBUG */
            }
        }
        else if (HANDLER_ERROR_TIMEOUT != ret)
        {
#if DEBUG
            elog_e(TAG, "LED queue receive failed, status=%d.\r\n", (int)ret);
#endif  /* DEBUG */
        }

        /********* 3. Yield before polling again **********/
        ret = p_self->p_os_time_delay->pf_os_delay_ms(
            LED_HANDLER_POLL_INTERVAL_MS);
        if (HANDLER_OK != ret)
        {
#if DEBUG
            elog_e(TAG, "LED handler delay failed, status=%d.\r\n", (int)ret);
#endif  /* DEBUG */
        }
    }
}
#endif  /* OS_SUPPORT */

/**
 * @brief Bind Handler interfaces and create its queue and worker.
 *
 * Processing steps:
 * 1. Check pointers, repeated instantiation and all required callbacks.
 * 2. Bind interfaces and methods, then clear the registration table.
 * 3. With OS_SUPPORT, create the queue before creating the worker thread.
 * 4. Roll back failures; mark initialized only after all steps succeed.
 *
 * @param[in,out] p_self         : Handler object cleared before first use.
 * @param[in]     p_os_delay     : Millisecond delay interface used with
 *                                 OS_SUPPORT.
 * @param[in]     p_os_queue     : Queue interface used with OS_SUPPORT.
 * @param[in]     p_os_critical  : Critical-section interface used with
 *                                 OS_SUPPORT.
 * @param[in]     p_os_thread    : Thread interface used with OS_SUPPORT.
 * @param[in]     p_time_base_ms : Millisecond time-base interface.
 *
 * @return led_handler_status_t : Instantiation result.
 * @retval HANDLER_OK              The object and required OS resources are
 *                                 ready.
 * @retval HANDLER_ERROR_PARAMETER A required pointer or interface callback is
 *                                 NULL.
 * @retval HANDLER_ERROR_RESOURCE  Already initialized or still owns unreleased
 *                                 resources.
 * @retval HANDLER_ERROR_NOMEMORY  Queue or thread allocation failed.
 *
 * @note The system layer supplies the object and interface tables; injected
 *       callbacks create OS resources.
 *       The system layer coordinates instantiation, registration and cleanup;
 *       submit requests only after success.
 *       Handler, registered Drivers and interface tables must remain valid
 *       while the worker or its queue is in use.
 *       Each Driver belongs to one Handler; its worker maintains the runtime
 *       blink parameters.
 *       OS_SUPPORT builds run in task context and propagate resource creation
 *       errors.
 *       Rollback deletes the thread before the queue and propagates cleanup
 *       errors.
 *       Clear handles only after successful deletion; residual resources
 *       prevent reconstruction.
 */
led_handler_status_t LEDHandler_Instantiate(
                            bsp_led_handler_t            * const         p_self,
#ifdef OS_SUPPORT
                            const handler_os_delay_t     * const     p_os_delay,
                            const handler_os_queue_t     * const     p_os_queue,
                            const handler_os_critical_t  * const  p_os_critical,
                            const handler_os_thread_t    * const    p_os_thread,
#endif  /* OS_SUPPORT */
                            const handler_time_base_ms_t * const p_time_base_ms
                                                                               )
{
    led_handler_status_t ret = HANDLER_OK;  /* Operation result.              */
#ifdef OS_SUPPORT
    led_handler_status_t cleanup_ret = HANDLER_OK;  /* Rollback result.       */
#endif  /* OS_SUPPORT */

#if DEBUG
    elog_i(TAG, "LED handler initialization starting.\r\n");
#endif  /* DEBUG */

    /********** 1. Validate construction inputs ***********/
    if (NULL == p_self ||
#ifdef OS_SUPPORT
        NULL == p_os_delay || NULL == p_os_queue ||
        NULL == p_os_critical || NULL == p_os_thread ||
#endif  /* OS_SUPPORT */
        NULL == p_time_base_ms)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED handler initialization parameter.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    if (HANDLER_INITED == p_self->is_inited)
    {
#if DEBUG
        elog_e(TAG, "LED handler is already initialized.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_RESOURCE;
    }

#ifdef OS_SUPPORT
    /* Reject reconstruction while resource handles remain live. */
    if (NULL != p_self->p_queue_handle || NULL != p_self->p_thread_handle)
    {
#if DEBUG
        elog_e(TAG, "LED handler still owns unreleased OS resources.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_RESOURCE;
    }
#endif  /* OS_SUPPORT */

    if (NULL == p_time_base_ms->pf_get_time_ms
#ifdef OS_SUPPORT
        || NULL == p_os_delay->pf_os_delay_ms
        || NULL == p_os_queue->pf_os_queue_create
        || NULL == p_os_queue->pf_os_queue_put
        || NULL == p_os_queue->pf_os_queue_get
        || NULL == p_os_queue->pf_os_queue_delete
        || NULL == p_os_critical->pf_os_critical_enter
        || NULL == p_os_critical->pf_os_critical_exit
        || NULL == p_os_thread->pf_os_thread_create
        || NULL == p_os_thread->pf_os_thread_delete
#endif  /* OS_SUPPORT */
        )
    {
#if DEBUG
        elog_e(TAG, "Invalid LED handler interface callback.\r\n");
#endif  /* DEBUG */
        return HANDLER_ERROR_PARAMETER;
    }

    /*********** 2. Bind interfaces and methods ***********/
    p_self->p_time_base_ms = p_time_base_ms;
#ifdef OS_SUPPORT
    p_self->p_os_time_delay      =    p_os_delay;
    p_self->p_os_queue_interface =    p_os_queue;
    p_self->p_os_critical        = p_os_critical;
    p_self->p_os_thread          =   p_os_thread;
    p_self->p_queue_handle       =          NULL;
    p_self->p_thread_handle      =          NULL;
#endif  /* OS_SUPPORT */
    p_self->pf_led_control  =  LEDHandler_Control;
    p_self->pf_led_register = LEDHandler_Register;
    p_self->is_inited       =  HANDLER_NOT_INITED;

    ret = LEDHandler_ArrayInit(p_self->instances.led_instance_group,
                              MAX_INSTANCE_NUMBER);
    if (HANDLER_OK != ret)
    {
#if DEBUG
        elog_e(TAG, "Failed to initialize the LED registration table.\r\n");
#endif  /* DEBUG */
        return ret;
    }
    p_self->instances.led_instance_num = 0U;

#ifdef OS_SUPPORT

    /************** 3. Create message queue ***************/
    ret = p_self->p_os_queue_interface->pf_os_queue_create(
        LED_HANDLER_QUEUE_LENGTH,
        sizeof(led_handler_event_t),
        &p_self->p_queue_handle);
    if (HANDLER_OK != ret || NULL == p_self->p_queue_handle)
    {
        if (HANDLER_OK == ret)
        {
            ret = HANDLER_ERROR_RESOURCE;
        }
#if DEBUG
        elog_e(TAG, "LED queue creation failed, status=%d.\r\n", (int)ret);
#endif  /* DEBUG */
        goto rollback_resources;
    }

    /*************** 4. Create worker last ****************/
    ret = p_self->p_os_thread->pf_os_thread_create(
        LEDHandler_ThreadEntry,
        LED_HANDLER_THREAD_NAME,
        LED_HANDLER_THREAD_STACK_DEPTH,
        p_self,
        LED_HANDLER_THREAD_PRIORITY,
        &p_self->p_thread_handle);
    if (HANDLER_OK != ret || NULL == p_self->p_thread_handle)
    {
        if (HANDLER_OK == ret)
        {
            ret = HANDLER_ERROR_RESOURCE;
        }
#if DEBUG
        elog_e(TAG, "LED thread creation failed, status=%d.\r\n", (int)ret);
#endif  /* DEBUG */
        goto rollback_resources;
    }
#endif  /* OS_SUPPORT */

    /******************* 5. Mark ready ********************/
    p_self->is_inited = HANDLER_INITED;

#if DEBUG
    elog_i(TAG, "LED handler initialization complete.\r\n");
#endif  /* DEBUG */
    return HANDLER_OK;

#ifdef OS_SUPPORT
rollback_resources:

    /*************** 6. Roll back resources ***************/

    /******* 6.1 Delete worker ********/
    if (NULL != p_self->p_thread_handle)
    {
        cleanup_ret = p_self->p_os_thread->pf_os_thread_delete(
            p_self->p_thread_handle);
        if (HANDLER_OK != cleanup_ret)
        {
#if DEBUG
            elog_e(TAG, "LED thread rollback failed, status=%d.\r\n",
                   (int)cleanup_ret);
#endif  /* DEBUG */
            return cleanup_ret;
        }
        p_self->p_thread_handle = NULL;
    }

    /******** 6.2 Delete queue ********/
    if (NULL != p_self->p_queue_handle)
    {
        cleanup_ret = p_self->p_os_queue_interface->pf_os_queue_delete(
            p_self->p_queue_handle);
        if (HANDLER_OK != cleanup_ret)
        {
#if DEBUG
            elog_e(TAG, "LED queue rollback failed, status=%d.\r\n",
                   (int)cleanup_ret);
#endif  /* DEBUG */
            return cleanup_ret;
        }
        p_self->p_queue_handle = NULL;
    }
    return ret;
#endif  /* OS_SUPPORT */
}

/**************************** Function Definitions ****************************/
