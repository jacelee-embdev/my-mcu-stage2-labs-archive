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

#include <stdint.h>                     /* C standard library                 */

#include "sw_bsp_aht21_driver.h"        /* AHT21 driver layer                 */

/********************************** Includes **********************************/

/*********************************** Defines **********************************/

typedef enum
{
    TEMP_HUMI_OK              =    0,   /* Operation completed.               */
    TEMP_HUMI_ERROR           =   -1,   /* General error.                     */
    TEMP_HUMI_ERROR_RESOURCE  =   -2,   /* Resource unavailable.              */
    TEMP_HUMI_ERROR_PARAMETER =   -3,   /* Invalid parameter.                 */
    TEMP_HUMI_ERROR_ISR       =   -4,   /* Invalid interrupt context.         */
    TEMP_HUMI_ERROR_TIMEOUT   =   -5,   /* Operation timed out.               */
    TEMP_HUMI_ERROR_NOMEMORY  =   -6,   /* Insufficient memory.               */
    TEMP_HUMI_RESERVED        = 0xFF,   /* Reserved status value.             */
} temp_humi_status_t;  /* Handler operation status */

typedef enum
{
    ONLY_TEMP_EVENT = 0,                /* Request temperature only.          */
    ONLY_HUMI_EVENT,                    /* Request relative humidity only.    */
    BOTH_TEMP_HUMI_EVENT                /* Request both readings.             */
} temp_humi_event_type_t;  /* Requested measurements */

typedef struct
{
    float                   *temperature;    /* Temperature output pointer.   */
    float                      *humidity;    /* Humidity output pointer.      */
    uint32_t                    lifetime;    /* Maximum reading age in ms.    */
    uint32_t                   timestamp;    /* Event timestamp.              */
    temp_humi_event_type_t          type;    /* Requested measurements.       */

    void (*pfcallback) (float *, float *);   /* Reading callback (temp., RH). */
} temp_humi_event_t;  /* Measurement request */

typedef struct
{
    void                    (*os_delay_ms) (uint32_t const ms); /* Delay (ms).*/

    temp_humi_status_t  (*os_queue_create) (                /* Create queue.  */
                                        uint32_t const                 item_num,
                                        uint32_t const                item_size,
                                        void ** const              queue_handle
                                                                              );

    temp_humi_status_t    (*os_queue_send) (                /* Enqueue item.  */
                                        void * const               queue_handle,
                                        void * const                       item,
                                        uint32_t                        timeout
                                                                              );

    temp_humi_status_t (*os_queue_receive) (                /* Dequeue item.  */
                                        void *                     queue_handle,
                                        void *                              msg,
                                        uint32_t                        timeout
                                                                              );
} temp_humi_handler_os_interface_t;  /* OS callbacks */

typedef struct
{
    aht21_iic_interface_t            *iic_driver_interface;  /* IIC access.   */
    aht21_timebase_interface_t         *timebase_interface;  /* Time base.    */
    temp_humi_handler_os_interface_t         *os_interface;  /* OS callbacks. */
    aht21_rtos_delay_interface_t          *yield_interface;  /* RTOS delay.   */
} temp_humi_handler_dependency_injection_t;  /* Instantiation inputs */

typedef struct temp_humi_handler_private_data
    temp_humi_handler_private_data_t;  /* Private handler state */

typedef struct
{
    /***************** Platform Interfaces ****************/
    aht21_timebase_interface_t         *timebase_interface;  /* Time base.    */
    aht21_iic_interface_t            *iic_driver_interface;  /* IIC access.   */
    aht21_rtos_delay_interface_t          *yield_interface;  /* RTOS delay.   */
    temp_humi_handler_os_interface_t         *os_interface;  /* OS callbacks. */

    /******************** Handler State *******************/
    bsp_aht21_driver_t                   *p_aht21_instance; /* Driver object. */
    void                               *event_queue_handle; /* Request queue. */
    temp_humi_handler_private_data_t       *p_private_data; /* Private state. */
    uint32_t                                last_temp_tick; /* Temp. read ms. */
    uint32_t                                last_humi_tick; /* RH read ms.    */
} bsp_temp_humi_handler_t;  /* AHT21 handler object */

/*********************************** Defines **********************************/

/********************************** Declaring *********************************/

/**
 * @brief Process queued temperature and humidity requests.
 *
 * The request specifies which readings are needed and how old they may be.
 *
 * @param[in] argument : Pointer to the handler's instantiation inputs.
 */
void temp_humi_handler_thread(void *argument);

/**
 * @brief Bind the supplied interfaces and initialize the handler.
 *
 * Instantiation sets up the event queue and the associated AHT21 driver.
 *
 * @param[in,out] handler_instance : Handler object to initialize.
 * @param[in]     input_arg        : Platform interfaces for the handler.
 *
 * @return temp_humi_status_t : Handler initialization status.
 */
temp_humi_status_t bsp_temp_humi_handler_inst(
                bsp_temp_humi_handler_t                       *handler_instance,
                temp_humi_handler_dependency_injection_t             *input_arg
                                                                              );

/**
 * @brief Queue a measurement request for the handler thread.
 *
 * @param[in] event : Request containing the data type, freshness limit and
 *                    result delivery fields.
 *
 * @return temp_humi_status_t : Queue submission status.
 */
temp_humi_status_t bsp_temp_humi_read(temp_humi_event_t *event);

/********************************** Declaring *********************************/

#endif  /* __SW_BSP_AHT21_HANDLER_H__ */
