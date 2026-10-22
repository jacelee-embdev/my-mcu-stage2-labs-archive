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
 * @brief Provides BSP APIs for LED control and related RTOS operations.
 *
 * Processing flow:
 *
 * Call led_operate() and led_process_command() from task context,
 * or run led_task_entry() as an RTOS thread.
 *
 * @version V1.1 2026-07-26
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
/************************ Thread Definitions ************************/
extern osThreadId_t led_task_handle;
extern const osThreadAttr_t led_task_attributes;

/************************ Queue Handles ************************/
extern QueueHandle_t g_led_queue;


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

typedef enum
{
    LED_PROCESS_OK              = 0,  /* Command processed successfully. */
    LED_PROCESS_ERROR           = 1,  /* A lower-level LED operation failed. */
    LED_PROCESS_INVALID_COMMAND = 2,  /* Unsupported LED command. */
} led_process_result_t; /* The results returned after executing the LED command.*/

typedef enum
{
    LED_CMD_NONE          = 0,  /* No LED command; take no action. */
    LED_CMD_TOGGLE        = 1,  /* Toggle the LED state. */
    LED_CMD_BLINK_3_TIMES = 2,  /* Blink the LED three times. */
} led_command_t; /* LED command enumeration. */
/****************************** Defines ******************************/


/************************ Function Declarations ************************/

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
led_operate_result_t led_operate(led_state_t led_state);


/**
 * @brief Processes an LED control command.
 *
 * LED_CMD_NONE performs no operation. LED_CMD_TOGGLE toggles the LED once.
 * LED_CMD_BLINK_3_TIMES toggles the LED six times with a 300 ms interval,
 * producing three blink cycles and restoring the original LED state.
 *
 * @param[in] led_command LED command to process.
 *
 * @return led_process_result_t Result of command processing.
 * @retval LED_PROCESS_OK              The command was processed successfully.
 * @retval LED_PROCESS_ERROR           A lower-level LED operation failed.
 * @retval LED_PROCESS_INVALID_COMMAND The command is not supported.
 *
 * @note This function must be called from task context because the blink
 *       command uses vTaskDelay().
 */
led_process_result_t led_process_command(led_command_t led_command);


/**
 * @brief Entry function for the LED-control task.
 *
 * Creates the LED-command queue and continuously waits for LED commands.
 * Whenever a command is received, led_process_command() is called to process
 * the command and update the LED. The task delays for 10 RTOS ticks between
 * receive attempts.
 *
 * @param[in] argument Pointer to the task argument. This parameter is not used.
 *
 * @return None. This task never returns. If queue creation fails,
 *         the task logs the error and remains blocked indefinitely.
 */
void led_task_entry(void *argument);
/************************ Function Declarations ************************/

#endif /* End of __BSP_LED_H__ */
