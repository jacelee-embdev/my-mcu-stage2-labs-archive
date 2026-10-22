/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file bsp_led.c
 *
 * @par dependencies
 * - bsp_led.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides LED queue initialization, direct control, and timer-driven toggling.
 *
 * Processing flow:
 *
 * Call led_operate() or led_process_command() for direct LED control, or run
 * led_task_entry() with led_timer_callback() for timer-driven toggling.
 *
 * @version V1.3 2026-08-10
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#include "bsp_led.h"

/************************ Thread Definitions ************************/
osThreadId_t led_task_handle;
const osThreadAttr_t led_task_attributes =
{
    .name = "led_Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityHigh,
};

/************************ Queue Handles ************************/
QueueHandle_t g_led_queue = NULL;

/****************************** Shared LED State ***************/
static volatile uint8_t  s_led_toggle_remaining = 0U;

/**
 * @brief Initializes the LED command queue.
 *
 * Creates the queue used to transfer LED commands to the LED task.
 *
 * @return BaseType_t Result of the queue initialization.
 * @retval pdPASS The LED command queue was created successfully.
 * @retval pdFAIL The LED command queue could not be created.
 */
BaseType_t led_init(void)
{
    // Create and check the LED-command queue.
    g_led_queue = xQueueCreate(10U, sizeof(uint8_t));

    if (NULL == g_led_queue)
    {
        return pdFAIL;
    }

    return pdPASS;
}

/**
 * @brief Controls the LED according to the requested state.
 *
 * Drives the active-low LED GPIO to turn the LED on, turn it off,
 * or toggle its current state.
 *
 * @param[in] led_state Requested LED state.
 *
 * @return led_operate_result_t Result of the LED operation.
 * @retval LED_OPERATE_OK                The requested operation was completed.
 * @retval LED_OPERATE_INVALID_PARAMETER The requested LED state is invalid.
 */
led_operate_result_t led_operate(led_state_t led_state)
{
    // 1. Turn on the LED.
    if (LED_STATE_ON == led_state)
    {
        HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);

        return LED_OPERATE_OK;
    }

    // 2. Turn off the LED.
    if (LED_STATE_OFF == led_state)
    {
        HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
        
        return LED_OPERATE_OK;
    }

    // 3. Toggle the LED.
    if (LED_STATE_TOGGLE == led_state)
    {
        HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);
        return LED_OPERATE_OK;
    }

    // 4. Reject an unsupported LED state.
    return LED_OPERATE_INVALID_PARAMETER;
}

/**
 * @brief Processes an LED control command.
 *
 * LED_CMD_NONE performs no operation. LED_CMD_ON and LED_CMD_OFF set the LED
 * state, while LED_CMD_TOGGLE toggles the current state once.
 *
 * @param[in] led_command LED command to process.
 *
 * @return led_process_result_t Result of command processing.
 * @retval LED_PROCESS_OK              The command was processed successfully.
 * @retval LED_PROCESS_ERROR           A lower-level LED operation failed.
 * @retval LED_PROCESS_INVALID_COMMAND The command is not supported.
 *
 * @note This function is non-blocking and may be called from task or timer
 *       interrupt context.
 */
led_process_result_t led_process_command(led_command_t led_command)
{
    led_operate_result_t operate_result = LED_OPERATE_ERROR;

    // 1. Dispatch the requested LED command.
    switch (led_command)
    {
        /* Command 0: no operation. */
        case LED_CMD_NONE:
        {
            return LED_PROCESS_OK;
        }

        /* Command 1: turn on once. */
        case LED_CMD_ON:
        {
            operate_result = led_operate(LED_STATE_ON);
            break;
        }

        /* Command 2: turn off once. */
        case LED_CMD_OFF:
        {
            operate_result = led_operate(LED_STATE_OFF);
            break;
        }

        /* Command 3: toggle once. */
        case LED_CMD_TOGGLE:
        {
            operate_result = led_operate(LED_STATE_TOGGLE);
            break;
        }

        default:
        {
            // Reject an unsupported command without operating the LED.
            return LED_PROCESS_INVALID_COMMAND;
        }
    }

    // 2. Convert a lower-level operation failure to a processing error.
    if (LED_OPERATE_OK != operate_result)
    {
        return LED_PROCESS_ERROR;
    }

    return LED_PROCESS_OK;
}


/**
 * @brief Entry function for the LED toggle-request task.
 *
 * Waits indefinitely for toggle counts on the queue initialized by led_init().
 * Each received count replaces the pending count consumed by the periodic
 * timer callback. The shared count is updated inside a critical section.
 *
 * @param[in] argument Pointer to the task argument. This parameter is not used.
 *
 * @return None. The task runs indefinitely while receiving LED toggle counts.
 *
 * @note led_init() must complete successfully before this task is started.
 */
void led_task_entry(void *argument)
{
    uint8_t  received_toggle_count = 0U;

    for (;;)
    {
        // printf("LED thread test.\r\n");

        // 1. Process requests only after the LED queue has been initialized.
        if (NULL != g_led_queue)
        {
            // 2. Wait indefinitely for the next timer-driven toggle count.
            if (pdPASS == xQueueReceive(g_led_queue,
                                        &received_toggle_count,
                                        portMAX_DELAY))
            {
                // 3. Atomically replace the count shared with the timer callback.
                taskENTER_CRITICAL();
                s_led_toggle_remaining = received_toggle_count;
                taskEXIT_CRITICAL();
            }
        }
    }
}

/**
 * @brief Processes one timer tick for pending LED toggle requests.
 *
 * If timer-driven toggles remain, toggles the LED once and decrements the
 * remaining count. If the LED operation fails, cancels the remaining toggles.
 *
 * @return None.
 *
 * @note This function is called from the TIM2 period-elapsed callback.
 */
void led_timer_callback(void)
{
    led_process_result_t process_result = LED_PROCESS_ERROR;

    // 1. Return immediately when no timer-driven toggles remain.
    if (0U == s_led_toggle_remaining)
    {
        return;
    }

    // 2. Toggle the LED once for the current timer tick.
    process_result = led_process_command(LED_CMD_TOGGLE);

    if (LED_PROCESS_OK == process_result)
    {
        // 3. Consume one pending toggle after a successful operation.
        s_led_toggle_remaining--;
    }
    else
    {
        // 4. Cancel the remaining toggles if the operation fails.
        s_led_toggle_remaining = 0U;
    }
}

