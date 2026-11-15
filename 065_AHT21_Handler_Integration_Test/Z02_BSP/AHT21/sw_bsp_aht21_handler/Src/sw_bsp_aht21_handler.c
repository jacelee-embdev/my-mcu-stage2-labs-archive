/*******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file sw_bsp_aht21_handler.c
 *
 * @par dependencies
 * - sw_bsp_aht21_handler.h
 * - stdbool.h
 * - stddef.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements queued temperature and humidity measurement requests.
 *
 * Processing flow:
 *
 * Bind the supplied interfaces and initialize the event queue and driver.
 * Mount the persistent handler after successful instantiation.
 * Receive joint requests, apply freshness checks and invoke the callback.
 *
 * @version v1.0 2026-11-14
 *
 * @note 1 tab == 4 spaces!
 *       Ported from ec_bsp_temp_humi_xxx_handler.c by Jack, EternalChip.
 *       Only BOTH_TEMP_HUMI_EVENT requests perform measurements.
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "sw_bsp_aht21_handler.h"  /* Module header                           */

#include <stdbool.h>               /* C standard library                      */
#include <stddef.h>

#include "debug.h"                 /* Middleware                              */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG   "aht21.handler"     /* Log tag.                                 */
#define DEBUG                     /* Enable debug output.                     */

#define AHT21_HANDLER_NOT_INITIALIZED false           /* Uninitialized state. */
#define AHT21_HANDLER_INITIALIZED     true            /* Initialized state.   */
#define AHT21_HANDLER_WAIT_FOREVER    (0xFFFFFFFFUL)  /* Queue wait in ticks. */

struct temp_humi_handler_private_data
{
    bool is_initialized;          /* Handler initialization flag.             */
};  /* Private handler state */

static bsp_temp_humi_handler_t          * sg_p_temp_humi_instance = NULL;
static temp_humi_handler_private_data_t   sg_private_data         =  {0};

/********************************** Defines ***********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Create the event queue and instantiate the AHT21 driver.
 *
 * @param[in,out] p_handler_instance : Handler with bound interfaces.
 *
 * @return temp_humi_status_t : Initialization result.
 * @retval TEMP_HUMI_OK             The queue and driver were initialized.
 * @retval TEMP_HUMI_ERROR_RESOURCE The queue callback or setup failed.
 */
static temp_humi_status_t AHT21_HandlerInit(
                                   bsp_temp_humi_handler_t * p_handler_instance
                                                                               )
{
    elog_d(TAG, "AHT21_HandlerInit start");
    int8_t result = 0;

    /************* 1. Create the event queue **************/
    if (NULL == p_handler_instance->os_interface->os_queue_create)
    {
        elog_e(TAG, "os_queue_create is NULL");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    result = p_handler_instance->os_interface->os_queue_create(
        10U, sizeof(temp_humi_event_t),
        &p_handler_instance->event_queue_handle);
    elog_d(TAG, "os_queue_create result=%d", (int)result);
    if (result)
    {
#ifdef DEBUG
        elog_e(TAG, "error: create queue failed!");
#endif  /* DEBUG */
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    /************* 2. Instantiate the driver **************/
    result = AHT21_Instantiate(
        p_handler_instance->iic_driver_interface,
        p_handler_instance->p_aht21_instance,
        p_handler_instance->timebase_interface
#ifdef OS_SUPPORTING
      , p_handler_instance->yield_interface
#endif  /* OS_SUPPORTING */
    );
#ifdef DEBUG
    elog_d(TAG, "AHT21_Instantiate result=%d", (int)result);
#endif  /* DEBUG */
    if (result)
    {
        elog_e(TAG, "AHT21_Instantiate failed");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    elog_d(TAG, "AHT21_HandlerInit end");
    return TEMP_HUMI_OK;
}

/**
 * @brief Clear the mounted handler's initialization flag.
 *
 * @return temp_humi_status_t : TEMP_HUMI_OK.
 *
 * @note Clears the flag only; the queue and driver are not released.
 */
static temp_humi_status_t __attribute__((unused)) AHT21_HandlerDeinit(void)
{
    sg_p_temp_humi_instance->p_private_data->is_initialized =
        AHT21_HANDLER_NOT_INITIALIZED;

    return TEMP_HUMI_OK;
}

/**
 * @brief Mount the handler for the request submission interface.
 *
 * @param[in] p_handler_instance : Persistent instance to publish.
 */
static void AHT21_HandlerMount(bsp_temp_humi_handler_t *p_handler_instance)
{
    sg_p_temp_humi_instance = p_handler_instance;
}

/**
 * @brief Bind the supplied interfaces and initialize the handler.
 *
 * @param[in,out] p_handler_instance : Persistent handler with a valid driver.
 * @param[in]     p_input_arg        : Persistent platform interfaces.
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
                  bsp_temp_humi_handler_t                  * p_handler_instance,
                  temp_humi_handler_dependency_injection_t *        p_input_arg
                                                                               )
{
    temp_humi_status_t result = TEMP_HUMI_OK;

    elog_d(TAG, "AHT21_HandlerInstantiate start");

    /************ 1. Check the supplied inputs ************/
    if (NULL == p_handler_instance)
    {
        elog_e(TAG, "p_handler_instance is NULL");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    if (NULL == p_input_arg)
    {
        elog_e(TAG, "p_input_arg is NULL");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    if (NULL == p_input_arg->timebase_interface)
    {
        elog_e(TAG, "timebase_interface is NULL");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    if (NULL == p_input_arg->iic_driver_interface)
    {
        elog_e(TAG, "iic_driver_interface is NULL");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    if (NULL == p_input_arg->os_interface)
    {
        elog_e(TAG, "os_interface is NULL");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    /*************** 2. Bind the interfaces ***************/
    p_handler_instance->timebase_interface =
        p_input_arg->timebase_interface;
    p_handler_instance->iic_driver_interface =
        p_input_arg->iic_driver_interface;
    p_handler_instance->yield_interface = p_input_arg->yield_interface;
    p_handler_instance->os_interface    =    p_input_arg->os_interface;
    p_handler_instance->p_private_data  =             &sg_private_data;

    /**************** 3. Initialize state *****************/
    result = AHT21_HandlerInit(p_handler_instance);
    if (TEMP_HUMI_OK != result)
    {
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    p_handler_instance->p_private_data->is_initialized =
        AHT21_HANDLER_INITIALIZED;
    AHT21_HandlerMount(p_handler_instance);

    elog_d(TAG, "AHT21_HandlerInstantiate end");
    return TEMP_HUMI_OK;
}

/**
 * @brief Refresh cached readings for a joint temperature and humidity request.
 *
 * @param[in,out] p_handler_instance : Handler with time-base and driver.
 * @param[in]     event              : Request type and freshness threshold.
 * @param[in,out] p_temperature      : Worker's cached temperature in Celsius.
 * @param[in,out] p_humidity         : Worker's cached relative humidity in
 *                                     percent.
 *
 * @return temp_humi_status_t : Request type result.
 * @retval TEMP_HUMI_OK    The request type is BOTH_TEMP_HUMI_EVENT.
 * @retval TEMP_HUMI_ERROR Unsupported type; both readings were set to zero.
 *
 * @note Read both values only when temperature age exceeds event.lifetime.
 *       Equal age reuses the cached values. The humidity check only updates
 *       its timestamp and does not perform a separate measurement.
 *       Driver read errors are ignored; the temperature timestamp is still
 *       updated after each attempt.
 */
static temp_humi_status_t AHT21_HandlerReadTemperatureHumidity(
                                   bsp_temp_humi_handler_t * p_handler_instance,
                                   temp_humi_event_t                      event,
                                   float                   *      p_temperature,
                                   float                   *         p_humidity
                                                                               )
{
    temp_humi_status_t result = TEMP_HUMI_OK;
    uint32_t tick =
        p_handler_instance->timebase_interface->pf_get_tick_count();

    switch (event.type)
    {
        case BOTH_TEMP_HUMI_EVENT:
            if ((tick - p_handler_instance->last_temp_tick) > event.lifetime)
            {
                p_handler_instance->p_aht21_instance
                    ->pf_aht21_read_temperature_humidity(
                        p_handler_instance->p_aht21_instance,
                        p_temperature, p_humidity);
                p_handler_instance->last_temp_tick = tick;
                elog_i(TAG, "New Data");
            }
            else
            {
                elog_i(TAG, "Old Data");
            }

            if ((tick - p_handler_instance->last_humi_tick) > event.lifetime)
            {
                /* Update the humidity age marker without a separate read. */
                p_handler_instance->last_humi_tick = tick;
            }
            break;

        default:
            *p_temperature =            0.0f;
            *p_humidity    =            0.0f;
            result         = TEMP_HUMI_ERROR;
            break;
    }

    return result;
}

/**
 * @brief Copy one measurement request into the mounted handler's queue.
 *
 * @param[in] p_event : Valid request with a non-NULL pfcallback.
 *
 * @return temp_humi_status_t : Queue submission result.
 * @retval TEMP_HUMI_OK             The request was queued.
 * @retval TEMP_HUMI_ERROR_RESOURCE Initialization flag is clear or send failed.
 *
 * @note Call only after successful handler instantiation; the mounted object
 *       and its interfaces must remain valid.
 *       Queue submission uses AHT21_HANDLER_WAIT_FOREVER in OS tick units.
 *       The worker delivers readings through pfcallback; request output
 *       pointers and the request timestamp are not used.
 */
temp_humi_status_t AHT21_HandlerRequestReadTemperatureHumidity(
                                                    temp_humi_event_t * p_event
                                                                               )
{
    elog_d(TAG, "AHT21_HandlerRequestReadTemperatureHumidity");
    temp_humi_status_t result = TEMP_HUMI_OK;

    /*********** 1. Check initialization state ************/
    if (AHT21_HANDLER_NOT_INITIALIZED ==
        sg_p_temp_humi_instance->p_private_data->is_initialized)
    {
        elog_e(TAG, "handler instance is not initialized");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    /**************** 2. Submit the event *****************/
    result = sg_p_temp_humi_instance->os_interface->os_queue_send(
        sg_p_temp_humi_instance->event_queue_handle,
        p_event, AHT21_HANDLER_WAIT_FOREVER);
    elog_d(TAG, "send result=%d", (int)result);
    if (result)
    {
        elog_e(TAG, "error: os_queue_send event failed!");
        return TEMP_HUMI_ERROR_RESOURCE;
    }

    return TEMP_HUMI_OK;
}

/**
 * @brief Receive queued requests, process cached readings and invoke callbacks.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @note Instantiate the persistent handler before creating this worker.
 *       The receive result is logged but does not gate request processing.
 *       Every iteration invokes pfcallback, including unsupported request
 *       types.
 *       Callback pointers refer to worker-local readings and must not be
 *       retained.
 */
void AHT21_HandlerThreadEntry(void *p_argument)
{
    (void)p_argument;

    elog_d(TAG, "AHT21_HandlerThreadEntry start");
    float                     temperature        =                    0.0f;
    float                     humidity           =                    0.0f;
    bsp_temp_humi_handler_t * p_handler_instance = sg_p_temp_humi_instance;
    temp_humi_event_t event;
    temp_humi_status_t result = TEMP_HUMI_OK;

    for (;;)
    {

        /************** 1. Receive an event ***************/
        result = p_handler_instance->os_interface->os_queue_receive(
            p_handler_instance->event_queue_handle,
            &event, AHT21_HANDLER_WAIT_FOREVER);
        elog_d(TAG, "receive result=%d", (int)result);

        /******** 2. Read temperature and humidity ********/
        if (TEMP_HUMI_OK ==
            AHT21_HandlerReadTemperatureHumidity(
                p_handler_instance, event, &temperature, &humidity))
        {
            /* Joint request accepted; driver errors are not propagated. */
        }
        else
        {
            /* Unsupported request type; both readings were cleared. */
        }

        /************ 3. Execute the callback *************/
        event.pfcallback(&temperature, &humidity);
    }
}

/**************************** Function Definitions ****************************/
