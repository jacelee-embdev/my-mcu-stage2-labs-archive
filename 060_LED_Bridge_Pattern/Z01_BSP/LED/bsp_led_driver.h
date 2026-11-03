/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_driver.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares LED driver instances and optional synchronous control.
 *
 * Processing flow:
 *
 * Zero the driver object and bind its delay callback.
 * Instantiate it with persistent LED operation and time-base interfaces.
 * Use the handler for asynchronous LED control.
 * Enable synchronous control when the driver is used directly.
 *
 * @version v1.0 2026-11-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BSP_LED_DRIVER_H__
#define __BSP_LED_DRIVER_H__

/********************************** Includes **********************************/

#include <stdint.h>               /* C standard library                       */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define OS_SUPPORT                /* Enable Handler OS interfaces.            */

#define INITIALIZED   (1U)        /* The object is initialized.               */
#define UNINITIALIZED (0U)        /* The object is not initialized.           */

/* OS mode uses the Handler by default; override to 1 for direct Driver use. */
#ifndef LED_DRIVER_SYNC_CONTROL_ENABLE
#ifdef OS_SUPPORT
#define LED_DRIVER_SYNC_CONTROL_ENABLE (0U)  /* Use Handler control.          */
#else
#define LED_DRIVER_SYNC_CONTROL_ENABLE (1U)  /* Enable synchronous control.   */
#endif  /* OS_SUPPORT */
#endif  /* LED_DRIVER_SYNC_CONTROL_ENABLE */

#if (0U != LED_DRIVER_SYNC_CONTROL_ENABLE) &&                                  \
    (1U != LED_DRIVER_SYNC_CONTROL_ENABLE)
#error "LED_DRIVER_SYNC_CONTROL_ENABLE must be 0 or 1."
#endif  /* LED_DRIVER_SYNC_CONTROL_ENABLE */

#ifndef LED_STATUS_T_DEFINED
#define LED_STATUS_T_DEFINED
typedef enum
{
    LED_OK              =    0,   /* Operation completed successfully.        */
    LED_ERROR_TIMEOUT   =   -1,   /* Operation timed out.                     */
    LED_ERROR_RESOURCE  =   -2,   /* Resource unavailable.                    */
    LED_ERROR_PARAMETER =   -3,   /* Invalid parameter.                       */
    LED_ERROR_ISR       =   -4,   /* Invalid interrupt context.               */
    LED_ERROR           =   -5,   /* General error.                           */
    LED_ERROR_NOMEMORY  =   -6,   /* Insufficient memory.                     */
    LED_RESERVED        = 0xFF,   /* Unspecified status.                      */
} led_status_t;  /* LED operation status; keep identical to led.h. */
#endif  /* LED_STATUS_T_DEFINED */

typedef enum
{
    LED_BLINK_1_3         =    0,  /* On-time is 1/4 of the cycle.            */
    LED_BLINK_1_2         =    1,  /* On-time is 1/3 of the cycle.            */
    LED_BLINK_1_1         =    2,  /* On-time is 1/2 of the cycle.            */
    LED_BLINK_UNSPECIFIED = 0xFF,  /* The blink ratio is unspecified.         */
} led_blink_mode_t;  /* On/off ratio; on-time uses integer division. */

typedef struct
{
    led_status_t     (*pf_led_on) (void);  /* Turn on the LED.                */
    led_status_t    (*pf_led_off) (void);  /* Turn off the LED.               */
    led_status_t (*pf_led_toggle) (void);  /* Toggle the LED.                 */
} led_ops_t;  /* External LED operation callbacks. */

typedef struct
{
    uint32_t (*pf_get_timestamp_ms) (void);  /* Get the timestamp in ms.      */
} time_base_t;  /* Millisecond counter interface. */

typedef led_status_t (*pf_delay_ms_t) (const uint32_t delay_ms);  /* Delay */

typedef struct bsp_led_driver bsp_led_driver_t;  /* Driver object type. */

typedef led_status_t (*pf_led_control_t) (
                                   bsp_led_driver_t       * const        p_self,
                                   const uint32_t                 cycle_time_ms,
                                   const uint32_t                   blink_times,
                                   const led_blink_mode_t            blink_mode
                                                                              );

struct bsp_led_driver
{
    /******************* Internal State *******************/
    uint8_t is_inited;            /* Initialization flag.                     */

    /****************** Blink Parameters ******************/
    uint32_t         one_iteration_time_ms;  /* Cycle duration in ms.         */
    uint32_t                   blink_times;  /* Number of blinks.             */
    led_blink_mode_t        led_blink_mode;  /* On/off duration ratio.        */

    /********************* Interfaces *********************/
    const led_ops_t   * p_led_ops_inst;  /* Hardware operation callbacks.     */
    const time_base_t *    p_time_base;  /* Millisecond time base.            */
    pf_delay_ms_t          pf_delay_ms;  /* Delay in ms; not tied to OS.      */

    /********************** Methods ***********************/
    pf_led_control_t pf_led_control;  /* NULL if sync is disabled.            */
};  /* Caller-owned LED driver object. */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

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
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __BSP_LED_DRIVER_H__ */
