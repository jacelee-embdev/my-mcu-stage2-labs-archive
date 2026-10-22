/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_led.h
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
 * @brief Provide the BSP APIs for LED control and related RTOS operations.
 *
 * Processing flow:
 *
 * Call led_operate() directly, or run led_task_entry() as an RTOS thread.
 *
 * @version V1.0 2026-07-25
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/
 
#ifndef __BSP_LED_H__
#define __BSP_LED_H__

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
//*********************Thread_Func **********************//
extern osThreadId_t led_task_handle;
extern const osThreadAttr_t led_task_attributes;
//*********************Thread_Func **********************//

//*********************Queue_Handler ********************//
extern QueueHandle_t led_queue;
//*********************Queue_Handler ********************//

/* 函数返回状态枚举             */
typedef enum
{
    LED_OPERATE_OK                   = 0,          /* Success. */
    LED_OPERATE_ERROR                = 1,          /* General error. */
    LED_OPERATE_TIMEOUT              = 2,          /* Polling timed out. */
    LED_OPERATE_RESOURCE_UNAVAILABLE = 3,          /* Resource unavailable. */
    LED_OPERATE_INVALID_PARAMETER    = 4,          /* Invalid parameter. */
    LED_OPERATE_OUT_OF_MEMORY        = 5,          /* Out of memory. */
    LED_OPERATE_ISR_NOT_ALLOWED      = 6,          /* Invalid in ISR context. */
    LED_OPERATE_RESERVED             = 0x7FFFFFFF  /* Reserved. */
} led_operate_result_t; /* Return value of led_operate(). */

typedef enum
{
    LED_STATE_OFF    = 0,  /* LED is turned off. */
    LED_STATE_ON     = 1,  /* LED is turned on. */
    LED_STATE_TOGGLE = 2,  /* Toggle the current LED state. */
} led_state_t; /* Requested operating state of the LED. */

/****************************** Defines ******************************/

//****************************** Declaring ******************************//

/**
 * @brief Controls the LED according to the requested state.
 *
 * Drives the active-low LED GPIO to turn the LED on, turn it off,
 * or toggle its current state.
 *
 * @param[in] led_state Requested LED state.
 *
 * @return led_operate_result_t Result of the LED operation.
 * @retval LED_OPERATE_OK    The requested LED operation was completed.
 * @retval LED_OPERATE_ERROR The requested LED state is invalid.
 */
led_operate_result_t    led_operate         (led_state_t led_state);

/**
 * @brief Entry function for the LED-control task.
 *
 * Creates the LED-state queue and continuously waits for requested LED
 * states. Whenever a state is received, led_operate() is called to update
 * the LED.
 *
 * @param[in] argument Pointer to the task argument. This parameter is not used.
 *
 * @return None. This function returns if queue creation fails; otherwise,
 *         the task runs indefinitely.
 */
void                    led_task_entry      (void *argument);
//****************************** Declaring ******************************//

#endif /* End of __BSP_LED_H__ */
