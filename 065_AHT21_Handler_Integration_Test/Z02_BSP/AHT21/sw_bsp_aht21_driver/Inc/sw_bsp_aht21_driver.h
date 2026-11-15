/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file sw_bsp_aht21_driver.h
 *
 * @par dependencies
 * - stdint.h
 * - FreeRTOS.h (OS_SUPPORTING)
 * - iic.h (software IIC)
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the AHT21 driver object and platform callback interfaces.
 *
 * Processing flow:
 *
 * The caller supplies the IIC configuration and millisecond time base.
 * The RTOS delay interface accepts FreeRTOS ticks when OS_SUPPORTING is set.
 * AHT21_Instantiate binds the interfaces and initializes the sensor.
 *
 * @version v1.0 2026-11-06
 *
 * @note 1 tab == 4 spaces!
 *       Use matching HARDWARE_IIC and OS_SUPPORTING settings in all
 *       translation units that use this header.
 *
 ******************************************************************************/

#ifndef __SW_BSP_AHT21_DRIVER_H__
#define __SW_BSP_AHT21_DRIVER_H__

#ifndef OS_SUPPORTING
#define OS_SUPPORTING             /* Enable the RTOS delay interface.         */
#endif  /* OS_SUPPORTING */

/********************************** Includes **********************************/

#include <stdint.h>    /* C standard library                                  */

#ifdef OS_SUPPORTING
#include "FreeRTOS.h"  /* RTOS tick type                                      */
#endif  /* OS_SUPPORTING */

#ifndef HARDWARE_IIC
#include "iic.h"       /* GPIO IIC bus definition                             */
#endif  /* HARDWARE_IIC */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

typedef enum aht21_status
{
    AHT21_OK              =    0,  /* Operation completed.                    */
    AHT21_ERROR           =   -1,  /* General error.                          */
    AHT21_ERROR_RESOURCE  =   -2,  /* Resource unavailable.                   */
    AHT21_ERROR_PARAMETER =   -3,  /* Invalid parameter.                      */
    AHT21_ERROR_ISR       =   -4,  /* Invalid interrupt context.              */
    AHT21_ERROR_TIMEOUT   =   -5,  /* Operation timed out.                    */
    AHT21_ERROR_NOMEMORY  =   -6,  /* Insufficient memory.                    */
    AHT21_RESERVED        = 0xFF,  /* Unspecified status.                     */
} aht21_status_t;  /* AHT21 operation status */

typedef struct aht21_iic_interface
{
#ifdef HARDWARE_IIC
    aht21_status_t      (*pf_iic_init) (void *);  /* Initialize the bus.      */
    aht21_status_t    (*pf_iic_deinit) (void *);  /* Unused deinit slot.      */
    aht21_status_t     (*pf_iic_start) (void *);  /* Generate START.          */
    aht21_status_t      (*pf_iic_stop) (void *);  /* Generate STOP.           */
    aht21_status_t  (*pf_iic_wait_ack) (void *);  /* Wait for ACK.            */
    aht21_status_t  (*pf_iic_send_ack) (void *);  /* Send ACK.                */
    aht21_status_t (*pf_iic_send_nack) (void *);  /* Send NACK.               */
    aht21_status_t (*pf_iic_send_byte) (          /* Send one byte.           */
                                                                void          *,
                                                                const uint8_t
                                                                              );
    aht21_status_t (*pf_iic_receive_byte) (  /* Read byte into output.        */
                                                                void    *      ,
                                                                uint8_t * const
                                                                              );

#else
    iic_bus_t * p_bus;                 /* GPIO IIC bus.                       */
    void (*pf_critical_enter) (void);  /* Enter critical.                     */
    void  (*pf_critical_exit) (void);  /* Leave critical.                     */
#endif  /* HARDWARE_IIC */
} aht21_iic_interface_t;  /* IIC configuration */

typedef struct aht21_timebase_interface
{
    uint32_t (*pf_get_tick_count) (void);  /* Read the millisecond count.     */
} aht21_timebase_interface_t;  /* Millisecond time base */

#ifdef OS_SUPPORTING
typedef struct aht21_rtos_delay_interface
{
    void (*pf_rtos_delay) (const TickType_t delay_ticks);  /* Delay in ticks. */
} aht21_rtos_delay_interface_t;  /* RTOS delay callback */
#endif  /* OS_SUPPORTING */

typedef struct bsp_aht21_driver bsp_aht21_driver_t;  /* AHT21 driver type */

struct bsp_aht21_driver
{
    /**************** Platform Interfaces *****************/
    const aht21_iic_interface_t      * p_iic_interface;  /* IIC settings.     */
    const aht21_timebase_interface_t *      p_timebase;  /* Time base.        */
#ifdef OS_SUPPORTING
    const aht21_rtos_delay_interface_t * p_rtos_delay;  /* RTOS delay.        */
#endif  /* OS_SUPPORTING */

    /******************* Driver Methods *******************/
    aht21_status_t (*pf_aht21_inst) (  /* Reserved; NULL.                     */
                     bsp_aht21_driver_t                 * const          p_self,
                     const aht21_iic_interface_t        * const p_iic_interface,
                     const aht21_timebase_interface_t   * const      p_timebase
#ifdef OS_SUPPORTING
                   , const aht21_rtos_delay_interface_t * const    p_rtos_delay
#endif  /* OS_SUPPORTING */
                                                                              );

    aht21_status_t (*pf_aht21_init) (  /* Init sensor.                        */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
    aht21_status_t (*pf_aht21_deinit) (  /* Unsupported deinit.               */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
    aht21_status_t (*pf_aht21_read_status) (  /* Read raw status.             */
                                      const bsp_aht21_driver_t * const   p_self,
                                      uint8_t                  * const p_status
                                                                              );

    aht21_status_t (*pf_aht21_read_temperature_humidity) (  /* Read temp./RH. */
                             bsp_aht21_driver_t * const                  p_self,
                             float              * const         p_temperature_c,
                             float              * const p_relative_humidity_pct
                                                                              );

    aht21_status_t (*pf_aht21_sleep) (  /* Unsupported sleep.                 */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
    aht21_status_t (*pf_aht21_wakeup) (  /* Unsupported wakeup.               */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
};  /* AHT21 driver object */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

/**
 * @brief Bind platform interfaces and initialize an AHT21 driver object.
 *
 * @param[in]  p_iic_interface : Software bus or hardware callbacks.
 * @param[out] p_self          : Receives the initialized driver on success.
 * @param[in]  p_timebase      : Millisecond time-base interface.
 * @param[in]  p_rtos_delay    : RTOS delay interface when enabled.
 *
 * @return aht21_status_t : Instantiation result or an initialization error.
 * @retval AHT21_OK              The sensor passed the power-on status check.
 * @retval AHT21_ERROR_PARAMETER A required pointer or callback is NULL.
 * @retval AHT21_ERROR_RESOURCE  Already initialized or missing power-on bits.
 *
 * @note Keep the interface tables and software bus descriptor valid during use.
 *       A failed call leaves the caller's driver object untouched.
 *       Initialization state is shared by all objects in this driver module.
 */
aht21_status_t AHT21_Instantiate(
                     const aht21_iic_interface_t        * const p_iic_interface,
                     bsp_aht21_driver_t                 * const          p_self,
                     const aht21_timebase_interface_t   * const      p_timebase
#ifdef OS_SUPPORTING
                   , const aht21_rtos_delay_interface_t * const    p_rtos_delay
#endif  /* OS_SUPPORTING */
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __SW_BSP_AHT21_DRIVER_H__ */
