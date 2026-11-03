/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_handler.h
 *
 * @par dependencies
 * - stdint.h
 * - bsp_led_driver.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares LED registration, asynchronous control, and OS interfaces.
 *
 * Processing flow:
 *
 * Zero the handler object and instantiate it with persistent interfaces.
 * Create its message queue and worker thread when OS support is enabled.
 * Register LED drivers and submit indexed control requests through the queue.
 * Execute queued blink requests sequentially from the worker thread.
 * Keep objects and interface tables valid while the worker is running.
 *
 * @version v1.0 2026-11-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BSP_LED_HANDLER_H__
#define __BSP_LED_HANDLER_H__

/********************************** Includes **********************************/

#include <stdint.h>               /* C standard library                       */

#include "bsp_led_driver.h"       /* User modules                             */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

typedef enum
{
    HANDLER_NOT_INITED = 0,       /* Not initialized.                         */
    HANDLER_INITED     = 1,       /* Initialized.                             */
} led_handler_init_t;  /* Handler initialization state */

typedef enum
{
    LED_1 = 0,                    /* Registration slot 1.                     */
    LED_2                   ,     /* Registration slot 2.                     */
    LED_3                   ,     /* Registration slot 3.                     */
    LED_4                   ,     /* Registration slot 4.                     */
    LED_5                   ,     /* Registration slot 5.                     */
    LED_6                   ,     /* Registration slot 6.                     */
    LED_7                   ,     /* Registration slot 7.                     */
    LED_8                   ,     /* Registration slot 8.                     */
    LED_9                   ,     /* Registration slot 9.                     */
    LED_10                  ,     /* Registration slot 10.                    */
    MAX_INSTANCE_NUMBER     ,     /* Registration capacity: 10.               */
    LED_NOT_INITIALIZED = -1      /* No valid index assigned.                 */
} led_index_t;  /* Slots start at zero; MAX_INSTANCE_NUMBER is capacity */

typedef enum
{
    HANDLER_OK              =    0,  /* Operation completed.                  */
    HANDLER_ERROR           =   -1,  /* General error.                        */
    HANDLER_ERROR_TIMEOUT   =   -2,  /* Operation timed out.                  */
    HANDLER_ERROR_RESOURCE  =   -3,  /* Bad state or unavailable resource.    */
    HANDLER_ERROR_PARAMETER =   -4,  /* Invalid parameter or interface.       */
    HANDLER_ERROR_NOMEMORY  =   -5,  /* Insufficient memory.                  */
    HANDLER_ERROR_ISR       =   -6,  /* Invalid interrupt context.            */
    HANDLER_RESERVED        = 0xFF,  /* Unspecified status.                   */
} led_handler_status_t;  /* Handler status; compare individual enum values */

typedef struct
{
    led_handler_status_t (*pf_get_time_ms) (  /* Read ms count.               */
                                    uint32_t * const                  p_time_ms
                                                                              );
} handler_time_base_ms_t;  /* Read ms count through output; return status */

#ifdef OS_SUPPORT

typedef struct
{
    led_handler_status_t (*pf_os_delay_ms) (  /* Delay in ms.                 */
                                    const uint32_t                     delay_ms
                                                                              );
} handler_os_delay_t;  /* OS delay in ms; returns status */

typedef struct
{
    /**
     * @brief Create a queue and output its handle.
     *
     * @param[in]  item_num       : Queue capacity.
     * @param[in]  item_size      : Size of one copied item.
     * @param[out] p_queue_handle : Receives a valid handle on success.
     *
     * @return led_handler_status_t : Queue creation result.
     *
     * @note Failure outputs an unreleased resource handle or NULL.
     */
    led_handler_status_t (*pf_os_queue_create) (  /* Create queue.            */
                                    const uint32_t                     item_num,
                                    const uint32_t                    item_size,
                                    void           ** const      p_queue_handle
                                                                              );

    /**
     * @brief Copy an item into the queue.
     *
     * @param[in] p_queue_handle : Queue handle.
     * @param[in] p_item         : Item copied before the send call returns.
     * @param[in] timeout        : Adapter-defined units; Handler passes zero.
     *
     * @return led_handler_status_t : Queue submission result.
     *
     * @note The queue does not retain the caller's message buffer after
     *       sending.
     */
    led_handler_status_t    (*pf_os_queue_put) (  /* Send copied item.        */
                                    void           * const       p_queue_handle,
                                    const void     * const               p_item,
                                    const uint32_t                      timeout
                                                                              );

    /**
     * @brief Receive a queue item into the output buffer.
     *
     * @param[in]  p_queue_handle : Queue handle.
     * @param[out] p_msg          : Receiving buffer for one queue item.
     * @param[in]  timeout        : Adapter-defined units; Handler passes zero.
     *
     * @return led_handler_status_t : Queue receive result.
     */
    led_handler_status_t    (*pf_os_queue_get) (  /* Receive an item.         */
                                    void           * const       p_queue_handle,
                                    void           * const                p_msg,
                                    const uint32_t                      timeout
                                                                              );

    /**
     * @brief Delete the specified queue.
     *
     * @param[in] p_queue_handle : Queue handle to delete.
     *
     * @return led_handler_status_t : Queue deletion result.
     *
     * @note Successful deletion invalidates the handle; retain it if deletion
     *       fails.
     */
    led_handler_status_t (*pf_os_queue_delete) (  /* Delete queue.            */
                                    void * const                 p_queue_handle
                                                                              );
} handler_os_queue_t;  /* OS queue callbacks */

typedef struct
{
    led_handler_status_t (*pf_os_critical_enter) (void);  /* Enter critical.  */
    led_handler_status_t  (*pf_os_critical_exit) (void);  /* Exit critical.   */
} handler_os_critical_t;  /* Task-context registration lock */

/**
 * @brief Entry type for the Handler worker thread.
 *
 * @param[in] p_argument : Thread context pointing to the Handler object.
 *
 * @return None; the entry processes requests continuously and does not return.
 *
 * @note The context object must remain valid while the thread uses it.
 */
typedef void (*pf_handler_thread_entry_t) (void * const p_argument);

typedef struct
{
    /**
     * @brief Create the worker and return its handle.
     *
     * @param[in]  pf_thread_entry : Thread entry function.
     * @param[in]  p_thread_name   : Thread name.
     * @param[in]  stack_depth     : Stack depth in units defined by the OS
     *                               adapter.
     * @param[in]  p_argument      : Context passed to the thread entry.
     * @param[in]  priority        : Priority interpreted by the OS adapter.
     * @param[out] p_thread_handle : Non-NULL output for the thread handle.
     *
     * @return led_handler_status_t : Thread creation result from the adapter.
     * @retval HANDLER_OK The thread was created and a valid handle returned.
     *
     * @note Success outputs a valid handle; failure outputs a residual handle
     *       or NULL.
     *       The new thread may run immediately; its queue and all dependencies
     *       must already be ready.
     *       The context object must remain valid while the thread uses it.
     */
    led_handler_status_t (*pf_os_thread_create) (  /* Create worker.          */
                       const pf_handler_thread_entry_t          pf_thread_entry,
                       const char                       * const   p_thread_name,
                       const uint32_t                               stack_depth,
                       void                             * const      p_argument,
                       const uint32_t                                  priority,
                       void                            ** const p_thread_handle
                                                                              );

    /**
     * @brief Delete the specified worker thread.
     *
     * @param[in] p_thread_handle : Valid handle of the thread to delete.
     *
     * @return led_handler_status_t : Thread deletion result from the adapter.
     * @retval HANDLER_OK The deletion call completed.
     *
     * @note The caller coordinates thread shutdown; a successfully deleted
     *       handle is invalid.
     *       On failure, retain the handle and keep all thread dependencies
     *       valid.
     */
    led_handler_status_t (*pf_os_thread_delete) (  /* Delete worker.          */
                                    void * const                p_thread_handle
                                                                              );
} handler_os_thread_t;  /* OS thread callbacks */
#endif  /* OS_SUPPORT */

typedef struct bsp_led_handler bsp_led_handler_t;  /* Handler object type */

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
typedef led_handler_status_t (*pf_handler_led_control_t) (
                                   bsp_led_handler_t      * const        p_self,
                                   const uint32_t                 cycle_time_ms,
                                   const uint32_t                   blink_times,
                                   const led_blink_mode_t            blink_mode,
                                   const led_index_t                  led_index
                                                                              );

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
typedef led_handler_status_t (*pf_handler_led_register_t) (
                                    bsp_led_handler_t * const            p_self,
                                    bsp_led_driver_t  * const      p_led_driver,
                                    led_index_t       * const           p_index
                                                                              );

typedef struct
{
    uint32_t                                  led_instance_num;  /* Count.    */
    bsp_led_driver_t * led_instance_group[MAX_INSTANCE_NUMBER];  /* Drivers.  */
} instance_registered_t;  /* Driver addresses only; no copy or ownership */

struct bsp_led_handler
{
    /******************* Internal State *******************/
    uint8_t is_inited;            /* Initialization flag.                     */

    /**************** Registered Instances ****************/
    instance_registered_t instances;  /* Registered drivers.                  */

#ifdef OS_SUPPORT

    /******************** OS Resources ********************/
    void *  p_queue_handle;       /* Queue handle.                            */
    void * p_thread_handle;       /* Worker thread.                           */
#endif  /* OS_SUPPORT */

    /********************* Interfaces *********************/
    const handler_time_base_ms_t * p_time_base_ms;  /* Time-base callbacks.   */
#ifdef OS_SUPPORT
    const handler_os_delay_t    *      p_os_time_delay;  /* Delay callbacks.  */
    const handler_os_queue_t    * p_os_queue_interface;  /* Queue callbacks.  */
    const handler_os_critical_t *        p_os_critical;  /* Critical lock.    */
    const handler_os_thread_t   *          p_os_thread;  /* Thread callbacks. */
#endif  /* OS_SUPPORT */

    /********************** Methods ***********************/
    pf_handler_led_control_t   pf_led_control;  /* Submit a request.          */
    pf_handler_led_register_t pf_led_register;  /* Register a Driver.         */
};  /* Caller-owned Handler object */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

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
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __BSP_LED_HANDLER_H__ */
