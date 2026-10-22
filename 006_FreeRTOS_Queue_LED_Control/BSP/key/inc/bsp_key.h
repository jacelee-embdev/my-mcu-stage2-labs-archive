/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_key.h
 *
 * @par dependencies
 * - stdio.h
 * - stdint.h
 * - main.h
 * - cmsis_os.h
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - gpio.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provide the BSP APIs for key input scanning and related RTOS operations.
 *
 * Processing flow:
 *
 * Call directly.
 *
 * @version V1.0 2026-07-24
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/
 
#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

/****************************** Includes ******************************/
#include <stdint.h>     // The Compiler Library
#include <stdio.h>

#include "main.h"       // Core / OS layer
#include "cmsis_os.h"

#include "FreeRTOS.h"   // Specific file for operation
#include "task.h"
#include "queue.h"
#include "gpio.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/

/********************** Thread_Func **********************/
extern osThreadId_t key_task_handle;
extern const osThreadAttr_t key_task_attributes;
/********************** Thread_Func **********************/

//*********************Queue_Handler ********************//
extern QueueHandle_t key_queue;
//*********************Queue_Handler ********************//

/* 函数返回状态枚举             */
typedef enum
{
    KEY_SCAN_OK                   = 0,          /* Success. */
    KEY_SCAN_ERROR                = 1,          /* General error. */
    KEY_SCAN_TIMEOUT              = 2,          /* Polling timed out. */   
    KEY_SCAN_RESOURCE_UNAVAILABLE = 3,          /* Resource unavailable. */
    KEY_SCAN_INVALID_PARAMETER    = 4,          /* Invalid parameter. */
    KEY_SCAN_OUT_OF_MEMORY        = 5,          /* Out of memory. */
    KEY_SCAN_ISR_NOT_ALLOWED      = 6,          /* Invalid in ISR context. */
    KEY_SCAN_RESERVED             = 0x7FFFFFFF  /* Reserved. */
} key_scan_result_t; /* Return value of key_scan(). */

typedef enum
{
    KEY_STATE_PRESSED  = 0,  /* Key is pressed. */
    KEY_STATE_RELEASED = 1,  /* Key is released. */
} key_state_t; /* Physical state of the key. */

/****************************** Defines ******************************/


//****************************** Declaring ******************************//

/**
 * @brief Scans the key input and reports its state.
 *
 * Polls the key GPIO for an active-low key press. If a key press is
 * detected, the output state is set to KEY_STATE_PRESSED. Otherwise,
 * it is set to KEY_STATE_RELEASED when the polling limit is reached.
 *
 * @param[out] p_key_state Pointer to the variable that receives the key state.
 *                         This pointer must not be NULL.
 *
 * @return key_scan_result_t Result of the key scan.
 * @retval KEY_SCAN_OK      A key press was detected.
 * @retval KEY_SCAN_TIMEOUT No key press was detected within the polling limit.
 */
key_scan_result_t       key_scan            (key_state_t *p_key_state);

/**
 * @brief Entry function for the key-scanning task.
 *
 * Creates the key-state queue and continuously scans the key at 500 ms
 * intervals. Whenever a pressed state is detected, the key state is sent
 * to the queue without blocking.
 *
 * @param[in] argument Pointer to the task argument. This parameter is not used.
 *
 * @return None. This task runs indefinitely.
 */
void                    key_task_entry      (void *argument);

//****************************** Declaring ******************************//


#endif /* End of __BSP_KEY_H__ */
