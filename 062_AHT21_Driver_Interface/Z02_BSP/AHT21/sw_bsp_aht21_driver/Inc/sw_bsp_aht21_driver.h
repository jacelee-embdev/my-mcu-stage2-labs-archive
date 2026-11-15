/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file sw_bsp_aht21_driver.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author JesMicro
 *
 * @brief Declares the AHT21 driver object and platform callback interfaces.
 *
 * Interface overview:
 *
 * The caller supplies IIC and millisecond time-base interfaces.
 * OS_SUPPORTING adds an RTOS delay interface.
 * AHT21_Instantiate is declared as the entry point for binding the interfaces.
 *
 * @version v1.0 2026-11-06
 *
 * @note The driver source is a skeleton. Use matching HARDWARE_IIC and
 *       OS_SUPPORTING settings in all translation units using this header.
 *
 ******************************************************************************/

#ifndef __SW_BSP_AHT21_DRIVER_H__
#define __SW_BSP_AHT21_DRIVER_H__

/********************************** Includes **********************************/

#include <stdint.h>             /* C standard library                         */

/********************************** Includes **********************************/

/*********************************** Defines **********************************/

typedef enum aht21_status
{
    AHT21_OK              =    0, /* Operation completed.                     */
    AHT21_ERROR           =   -1, /* General error.                           */
    AHT21_ERROR_RESOURCE  =   -2, /* Resource unavailable.                    */
    AHT21_ERROR_PARAMETER =   -3, /* Invalid parameter.                       */
    AHT21_ERROR_ISR       =   -4, /* Invalid interrupt context.               */
    AHT21_ERROR_TIMEOUT   =   -5, /* Operation timed out.                     */
    AHT21_ERROR_NOMEMORY  =   -6, /* Insufficient memory.                     */
    AHT21_RESERVED        = 0xFF, /* Unspecified status.                      */
} aht21_status_t;  /* AHT21 operation status */

#ifdef HARDWARE_IIC
typedef struct aht21_iic_interface
{
    aht21_status_t (*pf_iic_init        ) (void *); /* Init bus.              */
    aht21_status_t (*pf_iic_deinit      ) (void *); /* Deinit bus.            */
    aht21_status_t (*pf_iic_send_ack    ) (void *); /* Send ACK.              */
    aht21_status_t (*pf_iic_send_nack   ) (void *); /* Send NACK.             */
    aht21_status_t (*pf_iic_send_byte   ) (         /* Send a byte.           */
                                           void *                              ,
                                           uint8_t
                                                                              );
    aht21_status_t (*pf_iic_receive_byte) (void *); /* Byte receive status.   */
} aht21_iic_interface_t;  /* Hardware IIC callbacks */
#else
typedef struct aht21_iic_interface
{
    aht21_status_t (*pf_iic_init        ) (void *); /* Initialize the bus.    */
    aht21_status_t (*pf_iic_deinit      ) (void *); /* Deinitialize the bus.  */
    aht21_status_t (*pf_iic_start       ) (void *); /* Generate START.        */
    aht21_status_t (*pf_iic_stop        ) (void *); /* Generate STOP.         */
    aht21_status_t (*pf_iic_wait_ack    ) (void *); /* Wait for ACK.          */
    aht21_status_t (*pf_iic_send_ack    ) (void *); /* Send ACK.              */
    aht21_status_t (*pf_iic_send_nack   ) (void *); /* Send NACK.             */
    aht21_status_t (*pf_iic_send_byte   ) (         /* Send one byte.         */
                                           void *        ,
                                           const uint8_t
                                                        );
    aht21_status_t (*pf_iic_receive_byte) (         /* Read byte into output. */
                                           void *        ,
                                           uint8_t * const
                                                        );
    aht21_status_t (*pf_critical_enter  ) (void  ); /* Enter critical.        */
    aht21_status_t (*pf_critical_exit   ) (void  ); /* Leave critical.        */
} aht21_iic_interface_t;  /* Software IIC callbacks */
#endif  /* HARDWARE_IIC */

typedef struct aht21_timebase_interface
{
    uint32_t (*pf_get_tick_count) (void);   /* Read the millisecond count.    */
} aht21_timebase_interface_t;  /* Millisecond time base */

#ifdef OS_SUPPORTING
typedef struct aht21_rtos_delay_interface
{
    void (*pf_rtos_delay) (const uint32_t delay_ms);  /* Delay in ms.         */
} aht21_rtos_delay_interface_t;  /* RTOS delay callback */
#endif  /* OS_SUPPORTING */

typedef struct bsp_aht21_driver bsp_aht21_driver_t;  /* AHT21 driver type */

struct bsp_aht21_driver
{
    /***************** Platform Interfaces ****************/
    const aht21_iic_interface_t       * p_iic_interface;  /* IIC callbacks.   */
    const aht21_timebase_interface_t  *      p_timebase;  /* Time base.       */
#ifdef OS_SUPPORTING
    const aht21_rtos_delay_interface_t *    p_rtos_delay; /* RTOS delay.      */
#endif  /* OS_SUPPORTING */

    /******************* Driver Methods *******************/
    aht21_status_t (*pf_aht21_inst            ) (   /* Future vtable.         */
                      bsp_aht21_driver_t                * const          p_self,
                      const aht21_iic_interface_t       * const p_iic_interface,
                      const aht21_timebase_interface_t  * const      p_timebase
#ifdef OS_SUPPORTING
                    , const aht21_rtos_delay_interface_t * const    p_rtos_delay
#endif  /* OS_SUPPORTING */
                                                                              );

    aht21_status_t (*pf_aht21_init            ) (   /* Init sensor.           */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
    aht21_status_t (*pf_aht21_deinit          ) (   /* Deinit sensor.         */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
    aht21_status_t (*pf_aht21_read_id         ) (   /* ID operation.          */
                                              bsp_aht21_driver_t * const p_self
                                                                              );

    aht21_status_t (
           *pf_aht21_read_temperature_humidity) (   /* Read temp./RH.         */
                             bsp_aht21_driver_t * const                  p_self,
                             float              * const         p_temperature_c,
                             float              * const p_relative_humidity_pct
                                                                              );

    aht21_status_t (*pf_aht21_read_temperature) (   /* Read temp. (C).        */
                                     bsp_aht21_driver_t * const          p_self,
                                     float              * const p_temperature_c
                                                                              );

    aht21_status_t (*pf_aht21_read_humidity   ) (   /* Read RH (%).           */
                             bsp_aht21_driver_t * const                  p_self,
                             float              * const p_relative_humidity_pct
                                                                              );

    aht21_status_t (*pf_aht21_sleep           ) (   /* Enter sleep.           */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
    aht21_status_t (*pf_aht21_wakeup          ) (   /* Wake up.               */
                                              bsp_aht21_driver_t * const p_self
                                                                              );
};  /* AHT21 driver object */

/*********************************** Defines **********************************/

/********************************** Declaring *********************************/

/**
 * @brief Bind the platform interfaces to an AHT21 driver object.
 *
 * @param[in]     p_iic_interface : IIC callback table.
 * @param[in,out] p_self          : AHT21 driver object.
 * @param[in]     p_timebase      : Millisecond time-base interface.
 * @param[in]     p_rtos_delay    : RTOS delay interface when enabled.
 *
 * @return aht21_status_t : Instantiation result.
 *
 * @note Keep the interface tables valid while the driver uses them.
 */
aht21_status_t AHT21_Instantiate(
                      const aht21_iic_interface_t       * const p_iic_interface,
                      bsp_aht21_driver_t                * const          p_self,
                      const aht21_timebase_interface_t  * const      p_timebase
#ifdef OS_SUPPORTING
                   , const aht21_rtos_delay_interface_t * const    p_rtos_delay
#endif  /* OS_SUPPORTING */
                                                                              );

/********************************** Declaring *********************************/

#endif  /* __SW_BSP_AHT21_DRIVER_H__ */
