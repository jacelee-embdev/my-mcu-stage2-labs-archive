/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_driver.c
 *
 * @par dependencies
 * - bsp_led_driver.h
 * - stddef.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements LED driver setup and optional synchronous blink control.
 *
 * Processing flow:
 *
 * Bind callbacks, then instantiate the driver with a 600 ms setup delay.
 * Mount synchronous control only when enabled; otherwise use the handler.
 * Propagate callback errors during setup and synchronous blinking.
 *
 * @version v1.0 2026-11-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "bsp_led_driver.h"       /* Module header                            */

#include <stddef.h>               /* C standard library                       */

#include "debug.h"                /* Middleware                               */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG "bsp_led_driver"      /* Log tag.                                 */

#ifndef DEBUG
#define DEBUG (1U)                /* Enable debug output.                     */
#endif  /* DEBUG */

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

#if LED_DRIVER_SYNC_CONTROL_ENABLE
/**
 * @brief Run synchronous blinking with the object's stored parameters.
 *
 * Processing steps:
 * 1. Validate the object and snapshot its blink parameters.
 * 2. Calculate on-time using integer division of the cycle duration.
 * 3. Delay 1 ms before each output update until all cycles finish.
 *
 * @param[in] p_self : Initialized driver object.
 *
 * @return led_status_t : Blink result.
 * @retval LED_OK              All requested cycles completed successfully.
 * @retval LED_ERROR_PARAMETER Invalid object, state, callback or blink mode.
 *
 * @note Each iteration requests a 1 ms delay before changing the LED output.
 *       Scheduling and callback execution affect the actual timing.
 *       A zero cycle duration or blink count performs no output operations.
 *       Callback errors stop the request and are returned unchanged.
 *       The caller coordinates concurrent access to the same object.
 */
static led_status_t LEDDriver_Blink(const bsp_led_driver_t * const p_self)
{
    led_status_t ret = LED_OK;

    /****************** 1. Check target *******************/
    if (NULL == p_self || INITIALIZED != p_self->is_inited)
    {
#if DEBUG
        elog_e(TAG, "Invalid or uninitialized LED blink target.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    if (NULL == p_self->p_led_ops_inst || NULL == p_self->pf_delay_ms ||
        NULL == p_self->p_led_ops_inst->pf_led_on ||
        NULL == p_self->p_led_ops_inst->pf_led_off)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED blink interface.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /*************** 2. Prepare parameters ****************/

    /**** 2.1 Snapshot parameters *****/
    /* Keep the same settings throughout this request. */
    const uint32_t         cycle_time_local  = p_self->one_iteration_time_ms;
    const uint32_t         blink_times_local =           p_self->blink_times;
    const led_blink_mode_t blink_mode_local  =        p_self->led_blink_mode;

    uint32_t led_on_time_ms = 0U;

    /***** 2.2 Calculate on-time ******/
    if (LED_BLINK_1_1 == blink_mode_local)
    {
        led_on_time_ms = cycle_time_local / 2U;
    }
    else if (LED_BLINK_1_2 == blink_mode_local)
    {
        led_on_time_ms = cycle_time_local / 3U;
    }
    else if (LED_BLINK_1_3 == blink_mode_local)
    {
        led_on_time_ms = cycle_time_local / 4U;
    }
    else
    {
#if DEBUG
        elog_e(TAG, "Invalid LED blink mode.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /**************** 3. Execute blinking *****************/
    for (uint32_t i = 0U; i < blink_times_local; i++)
    {
        for (uint32_t j = 0U; j < cycle_time_local; j++)
        {
            /* Wait before updating the LED output. */
            ret = p_self->pf_delay_ms(1U);
            if (LED_OK != ret)
            {
                return ret;
            }

            if (j < led_on_time_ms)
            {
                ret = p_self->p_led_ops_inst->pf_led_on();
            }
            else
            {
                ret = p_self->p_led_ops_inst->pf_led_off();
            }

            if (LED_OK != ret)
            {
                return ret;
            }
        }
    }

    return LED_OK;
}

/**
 * @brief Validate and save control parameters, then run synchronous blinking.
 *
 * @param[in,out] p_self        : Initialized driver object.
 * @param[in]     cycle_time_ms : Cycle duration in ms; must be below 10000.
 * @param[in]     blink_times   : Blink count; must be below 1000.
 * @param[in]     blink_mode    : On/off duration ratio: 1:3, 1:2 or 1:1.
 *
 * @return led_status_t : Control result.
 * @retval LED_OK              Parameters saved and blinking completed.
 * @retval LED_ERROR_PARAMETER Invalid object, interface or input parameter.
 *
 * @note Zero cycle duration or count is allowed and only updates parameters.
 *       Callback errors are returned unchanged.
 *       This function runs synchronously in the caller's context.
 *       The caller coordinates concurrent access to the same object.
 */
static led_status_t LEDDriver_Control(
                                   bsp_led_driver_t       * const        p_self,
                                   const uint32_t                 cycle_time_ms,
                                   const uint32_t                   blink_times,
                                   const led_blink_mode_t            blink_mode
                                                                               )
{
    /****************** 1. Check target *******************/
    if (NULL == p_self || INITIALIZED != p_self->is_inited)
    {
#if DEBUG
        elog_e(TAG, "Invalid or uninitialized LED control target.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /*************** 2. Validate parameters ***************/
    if (!(cycle_time_ms < 10000U &&
          blink_times < 1000U &&
          (LED_BLINK_1_3 == blink_mode ||
           LED_BLINK_1_2 == blink_mode ||
           LED_BLINK_1_1 == blink_mode)))
    {
#if DEBUG
        elog_e(TAG, "Invalid LED control parameter.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /***************** 3. Save and blink ******************/
    p_self->one_iteration_time_ms = cycle_time_ms;
    p_self->blink_times           =   blink_times;
    p_self->led_blink_mode        =    blink_mode;

    return LEDDriver_Blink(p_self);
}
#endif  /* LED_DRIVER_SYNC_CONTROL_ENABLE */

/**
 * @brief Set the initial hardware state through the bound interfaces.
 *
 * Processing steps:
 * 1. Check the object pointer.
 * 2. Turn off the LED, delay 600 ms and check both operation results.
 * 3. Read the millisecond timestamp once without storing it in the object.
 *
 * @param[in] p_self : Object with LED, delay and time-base interfaces bound.
 *
 * @return led_status_t : Initialization result.
 * @retval LED_OK              LED off and delay succeeded; timestamp read.
 * @retval LED_ERROR_PARAMETER The object pointer is NULL.
 *
 * @note Called by the instantiation function after it validates all required
 *       interface pointers and callbacks. LED or delay errors are propagated.
 */
static led_status_t LEDDriver_Init(const bsp_led_driver_t * const p_self)
{
    led_status_t ret        = LED_OK;
    uint32_t     time_stamp =     0U;

    /****************** 1. Check target *******************/
#if DEBUG
    elog_i(TAG, "LED BSP initialization starting.\r\n");
#endif  /* DEBUG */
    if (NULL == p_self)
    {
#if DEBUG
        elog_e(TAG, "LED initialization passed an invalid pointer.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /*************** 2. Initialize hardware ***************/

    /******** 2.1 Turn off LED ********/
    ret = p_self->p_led_ops_inst->pf_led_off();
    if (LED_OK != ret)
    {
        return ret;
    }

    /******* 2.2 Request delay ********/
    ret = p_self->pf_delay_ms(600U);
    if (LED_OK != ret)
    {
        return ret;
    }

    /***************** 3. Read time base ******************/
    time_stamp = p_self->p_time_base->pf_get_timestamp_ms();

    /* Exercise the time base without storing its value in the object. */
    (void)time_stamp;

#if DEBUG
    elog_i(TAG, "LED BSP initialization complete.\r\n");
#endif  /* DEBUG */
    return LED_OK;
}

/**
 * @brief Instantiate an existing LED driver and bind its interfaces and method.
 *
 * Processing steps:
 * 1. Check pointers, repeated instantiation and required callbacks.
 * 2. Bind interfaces, set the control method and reset blink parameters.
 * 3. Turn off the LED, delay 600 ms and read the timestamp once.
 * 4. Clear interfaces and the method on failure; mark initialized on success.
 *
 * @param[in,out] p_self      : Zeroed object with pf_delay_ms already bound.
 * @param[in]     p_led_ops   : LED ops; on/off callbacks must be valid.
 * @param[in]     p_time_base : Time base; timestamp callback must be valid.
 *
 * @return led_status_t : Instantiation result.
 * @retval LED_OK              The object is ready.
 * @retval LED_ERROR_PARAMETER A required pointer or callback is NULL.
 * @retval LED_ERROR_RESOURCE  The object is already initialized.
 *
 * @note LED output or delay errors are returned from the callbacks.
 *       Invalid arguments or repeated instantiation leave the object unchanged.
 *       Initialization failure leaves the object uninitialized; rebind the
 *       delay callback before retrying.
 *       This function allocates no object memory; pf_led_toggle may be NULL.
 *       Initialize hardware and logging first. Interface objects must remain
 *       valid while the driver is in use.
 *       Call from task context when an OS delay adapter is used.
 *       The Handler blinks using this object's parameters, hardware callbacks
 *       and delay callback.
 *       The system layer coordinates sync control and Handler control of
 *       the same driver object.
 */
led_status_t LEDDriver_Instantiate(
                                    bsp_led_driver_t  * const            p_self,
                                    const led_ops_t   * const         p_led_ops,
                                    const time_base_t * const       p_time_base
                                                                               )
{
    led_status_t ret = LED_RESERVED;

    /***************** 1. Validate inputs *****************/

    /******* 1.1 Check pointers *******/
    if (NULL == p_self ||
        NULL == p_led_ops ||
        NULL == p_time_base)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED driver argument.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /******** 1.2 Check state *********/
    if (INITIALIZED == p_self->is_inited)
    {
#if DEBUG
        elog_e(TAG, "The resources have already been initialized.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_RESOURCE;
    }

    /****** 1.3 Check callbacks *******/
    if (NULL == p_self->pf_delay_ms ||
        NULL == p_led_ops->pf_led_on || NULL == p_led_ops->pf_led_off ||
        NULL == p_time_base->pf_get_timestamp_ms)
    {
#if DEBUG
        elog_e(TAG, "Invalid LED driver interface.\r\n");
#endif  /* DEBUG */
        return LED_ERROR_PARAMETER;
    }

    /***************** 2. Bind interfaces *****************/
    /* The caller has already bound the delay callback. */
    p_self->p_led_ops_inst =   p_led_ops;
    p_self->p_time_base    = p_time_base;
#if LED_DRIVER_SYNC_CONTROL_ENABLE
    p_self->pf_led_control = LEDDriver_Control;
#else
    p_self->pf_led_control = NULL;
#endif  /* LED_DRIVER_SYNC_CONTROL_ENABLE */
    p_self->is_inited = UNINITIALIZED;

    /**************** 3. Initialize state *****************/

    /****** 3.1 Reset parameters ******/
    p_self->blink_times           =                    0U;
    p_self->one_iteration_time_ms =                    0U;
    p_self->led_blink_mode        = LED_BLINK_UNSPECIFIED;

    /**** 3.2 Initialize hardware *****/
    ret = LEDDriver_Init(p_self);
    if (LED_OK != ret)
    {
        /******** 3.3 Rollback ********/
#if DEBUG
        elog_e(TAG, "LED initialization failed.\r\n");
#endif  /* DEBUG */
        /* Detach borrowed interfaces; no object memory is owned here. */
        p_self->p_led_ops_inst =          NULL;
        p_self->pf_delay_ms    =          NULL;
        p_self->p_time_base    =          NULL;
        p_self->pf_led_control =          NULL;
        p_self->is_inited      = UNINITIALIZED;
        return ret;
    }

    /******************* 4. Mark ready ********************/
    p_self->is_inited = INITIALIZED;
#if DEBUG
    elog_i(TAG, "LED initialization complete.\r\n");
#endif  /* DEBUG */

    ret = LED_OK;
    return ret;
}

/**************************** Function Definitions ****************************/
