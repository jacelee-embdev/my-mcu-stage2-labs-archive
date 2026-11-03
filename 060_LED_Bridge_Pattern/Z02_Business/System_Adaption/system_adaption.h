/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file system_adaption.h
 *
 * @par dependencies
 * - stdint.h
 * - bsp_led_driver.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares asynchronous LED resource initialization and control.
 *
 * Processing flow:
 *
 * The scatter loader copies marked BSP initial values to RAM before main().
 * Initialize resources once from the startup task, then submit requests.
 * The handler worker executes queued requests sequentially.
 *
 * @version V1.0 2026-11-03
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SYSTEM_ADAPTION_H__
#define __SYSTEM_ADAPTION_H__

/********************************** Includes **********************************/

#include <stdint.h>               /* C standard library                       */

#include "bsp_led_driver.h"       /* User modules                             */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

typedef enum
{
    SYSTEM_OK              =    0,  /* Operation completed.                   */
    SYSTEM_ERROR           =   -1,  /* General error.                         */
    SYSTEM_ERROR_TIMEOUT   =   -2,  /* Operation timed out.                   */
    SYSTEM_ERROR_RESOURCE  =   -3,  /* Bad state or unavailable resource.     */
    SYSTEM_ERROR_PARAMETER =   -4,  /* Invalid parameter or interface.        */
    SYSTEM_ERROR_NOMEMORY  =   -5,  /* Insufficient memory.                   */
    SYSTEM_ERROR_ISR       =   -6,  /* Invalid interrupt context.             */
    SYSTEM_RESERVED        = 0xFF,  /* Unspecified status.                    */
} system_status_t;  /* System API status; independent of BSP status types. */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

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
system_status_t System_InitResources(void);

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
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __SYSTEM_ADAPTION_H__ */
