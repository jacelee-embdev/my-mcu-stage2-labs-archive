/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file sw_bsp_aht21_handler.h
 *
 * @par dependencies
 * - stdint.h
 * - sw_bsp_aht21_driver.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the AHT21 handler and its queued measurement interface.
 *
 * Processing flow:
 *
 * Supply the IIC, time-base, OS and RTOS delay interfaces.
 * Instantiate the handler to set up its driver and event queue.
 * Submit measurement requests for processing by the handler thread.
 *
 * @version v1.0 2026-11-06
 *
 * @note 1 tab == 4 spaces!
 *       The handler depends on the driver's RTOS delay interface.
 *
 ******************************************************************************/

#ifndef __SW_BSP_AHT21_HANDLER_H__
#define __SW_BSP_AHT21_HANDLER_H__

/********************************** Includes **********************************/

#include <stdint.h>               /* C standard library                       */

#include "sw_bsp_aht21_driver.h"  /* AHT21 driver layer                       */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

typedef enum
{
    TEMP_HUMI_OK              =    0,  /* Operation completed.                */
    TEMP_HUMI_ERROR           =   -1,  /* General error.                      */
    TEMP_HUMI_ERROR_RESOURCE  =   -2,  /* Resource unavailable.               */
    TEMP_HUMI_ERROR_PARAMETER =   -3,  /* Invalid parameter.                  */
    TEMP_HUMI_ERROR_ISR       =   -4,  /* Invalid interrupt context.          */
    TEMP_HUMI_ERROR_TIMEOUT   =   -5,  /* Operation timed out.                */
    TEMP_HUMI_ERROR_NOMEMORY  =   -6,  /* Insufficient memory.                */
    TEMP_HUMI_RESERVED        = 0xFF,  /* Reserved status value.              */
} temp_humi_status_t;  /* Handler operation status */

typedef enum
{
    ONLY_TEMP_EVENT      = 0,     /* Temperature request; unsupported.        */
    ONLY_HUMI_EVENT         ,     /* Humidity request; unsupported.           */
    BOTH_TEMP_HUMI_EVENT          /* Request both readings.                   */
} temp_humi_event_type_t;  /* Requested measurements */

typedef struct
{
    float                  * temperature;  /* Unused temperature output.      */
    float                  *    humidity;  /* Unused humidity output.         */
    uint32_t                    lifetime;  /* Freshness threshold in ms.      */
    uint32_t                   timestamp;  /* Unused request timestamp.       */
    temp_humi_event_type_t          type;  /* Requested measurements.         */

    void (*pfcallback) (float *, float *);  /* Reading callback (temp., RH).  */
} temp_humi_event_t;  /* Measurement request */

typedef struct
{
    void (*os_delay_ms) (uint32_t const ms);  /* Unused delay callback.       */

    temp_humi_status_t (*os_queue_create) (  /* Create queue.                 */
                                           uint32_t const              item_num,
                                           uint32_t const             item_size,
                                           void           ** const queue_handle
                                                                              );

    temp_humi_status_t (*os_queue_send) (  /* Enqueue item.                   */
                                                  void     * const queue_handle,
                                                  void     * const         item,
                                                  uint32_t              timeout
                                                                              );

    temp_humi_status_t (*os_queue_receive) (  /* Dequeue item.                */
                                                        void     * queue_handle,
                                                        void     *          msg,
                                                        uint32_t        timeout
                                                                              );
} temp_humi_handler_os_interface_t;  /* OS callbacks */

typedef struct
{
    aht21_iic_interface_t            * iic_driver_interface;  /* IIC access.  */
    aht21_timebase_interface_t       *   timebase_interface;  /* Time base.   */
    temp_humi_handler_os_interface_t *         os_interface;  /* OS hooks.    */
    aht21_rtos_delay_interface_t     *      yield_interface;  /* RTOS delay.  */
} temp_humi_handler_dependency_injection_t;  /* Instantiation inputs */

typedef struct temp_humi_handler_private_data
    temp_humi_handler_private_data_t;  /* Private handler state */

typedef struct
{
    /**************** Platform Interfaces *****************/
    aht21_timebase_interface_t       *   timebase_interface;  /* Time base.   */
    aht21_iic_interface_t            * iic_driver_interface;  /* IIC access.  */
    aht21_rtos_delay_interface_t     *      yield_interface;  /* RTOS delay.  */
    temp_humi_handler_os_interface_t *         os_interface;  /* OS hooks.    */

    /******************* Handler State ********************/
    bsp_aht21_driver_t               *   p_aht21_instance;  /* Driver object. */
    void                             * event_queue_handle;  /* Request queue. */
    temp_humi_handler_private_data_t *     p_private_data;  /* Private state. */

    uint32_t last_temp_tick;      /* Last read-attempt tick.                  */
    uint32_t last_humi_tick;      /* Humidity age-check tick.                 */
} bsp_temp_humi_handler_t;  /* AHT21 handler object */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

/**
 * @brief Receive queued requests, process cached readings and invoke callbacks.
 *
 * @param[in] argument : Unused task argument. It may be NULL.
 *
 * @note Instantiate and mount the persistent handler before task creation.
 *       Only BOTH_TEMP_HUMI_EVENT can refresh measurements; other types clear
 *       both cached readings. Every iteration invokes the request callback.
 *       Receive and driver read errors do not prevent callback execution.
 *       Callback pointers refer to worker-local readings and must not be
 *       retained.
 */
void AHT21_HandlerThreadEntry(void *argument);

/**
 * @brief Bind the supplied interfaces and initialize the handler.
 *
 * @param[in,out] handler_instance : Persistent handler with a valid driver.
 * @param[in]     input_arg        : Persistent platform interfaces.
 *
 * @return temp_humi_status_t : Handler initialization result.
 * @retval TEMP_HUMI_OK             The queue and driver are ready; handler
 *                                  mounted.
 * @retval TEMP_HUMI_ERROR_RESOURCE An input check, queue creation or setup
 *                                  failed.
 *
 * @note Set p_aht21_instance before calling. The mounted handler is shared by
 *       the worker and request API. This function creates no task.
 *       A driver setup failure does not release the newly created queue.
 */
temp_humi_status_t AHT21_HandlerInstantiate(
                    bsp_temp_humi_handler_t                  * handler_instance,
                    temp_humi_handler_dependency_injection_t *        input_arg
                                                                              );

/**
 * @brief Copy one measurement request into the mounted handler's queue.
 *
 * @param[in] event : Valid request with a non-NULL pfcallback.
 *
 * @return temp_humi_status_t : Queue submission result.
 * @retval TEMP_HUMI_OK             The request was queued.
 * @retval TEMP_HUMI_ERROR_RESOURCE Initialization flag is clear or send failed.
 *
 * @note Call only after successful handler instantiation; the mounted object
 *       and its interfaces must remain valid. Submission may block on a full
 *       queue.
 *       lifetime is an age threshold in ms, tested with a strict greater-than
 *       comparison. Driver read errors are not reported by this API.
 *       The worker uses pfcallback; event output pointers and timestamp are
 *       unused.
 */
temp_humi_status_t AHT21_HandlerRequestReadTemperatureHumidity(
                                                      temp_humi_event_t * event
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __SW_BSP_AHT21_HANDLER_H__ */
