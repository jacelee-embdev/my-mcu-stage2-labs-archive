/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file system_adaption.h
 *
 * @par dependencies
 * - sw_bsp_aht21_handler.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares explicit AHT21 platform configuration and resource setup.
 *
 * Processing flow:
 *
 * Supply platform configuration from the running AHT21 business task.
 * Initialize resources before creating the handler worker with NULL input.
 *
 * @version v1.0 2026-11-14
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SYSTEM_ADAPTION_H__
#define __SYSTEM_ADAPTION_H__

/********************************** Includes **********************************/

#include "sw_bsp_aht21_handler.h"  /* AHT21 handler layer                     */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

typedef struct system_aht21_config
{
    iic_bus_t                                 iic_bus;  /* IIC bus settings.  */
    aht21_timebase_interface_t     timebase_interface;  /* Time base in ms.   */
    aht21_rtos_delay_interface_t rtos_delay_interface;  /* Delay in ticks.    */

    void (*pf_critical_enter) (void);  /* Enter critical section.             */
    void  (*pf_critical_exit) (void);  /* Leave critical section.             */
    void    (*pf_os_delay_ms) (uint32_t const ms);  /* Delay in ms.           */
} system_aht21_config_t;  /* Caller-supplied platform configuration */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

/**
 * @brief Copy configuration and initialize the handler's queue and driver.
 *
 * @param[in] p_config : Platform configuration copied into persistent storage.
 *
 * @return temp_humi_status_t : Configuration or handler initialization result.
 * @retval TEMP_HUMI_OK              The mounted handler is ready.
 * @retval TEMP_HUMI_ERROR_PARAMETER The configuration pointer is NULL.
 *
 * @note Call once from a running task before creating the handler worker.
 *       GPIO clocks, the IIC delay counter and logging must already be ready.
 *       Does not retain p_config or create a thread.
 */
temp_humi_status_t System_AHT21ResourcesInit(
                                   const system_aht21_config_t * const p_config
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __SYSTEM_ADAPTION_H__ */
