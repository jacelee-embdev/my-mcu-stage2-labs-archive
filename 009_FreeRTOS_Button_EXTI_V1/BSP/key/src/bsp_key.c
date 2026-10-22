/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_key.c
 *
 * @par dependencies
 * - bsp_key.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides interrupt-driven key detection and RTOS event delivery.
 *
 * Processing flow:
 *
 * The GPIO EXTI callback captures timestamped key edges and sends them to the
 * key task. The task filters invalid presses, classifies valid press durations,
 * and publishes short-press or long-press events through the result queue.
 *
 * @version V1.2 2026-08-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#include "bsp_key.h"

/************************ Thread Definitions ************************/
osThreadId_t key_task_handle;
const osThreadAttr_t key_task_attributes = {
    .name = "key_task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/************************ Queue Handles ************************/
QueueHandle_t g_key_queue = NULL;
static QueueHandle_t s_key_edge_queue = NULL;

/**
 * @brief Entry function for the interrupt-driven key task.
 *
 * Creates the internal edge queue and external result queue. It blocks for
 * timestamped edge events, filters intervals below KEY_DEBOUNCE_TIME_MS,
 * classifies valid presses with KEY_LONG_PRESS_TIME_MS, and sends completed
 * short-press or long-press results without waiting.
 *
 * @param[in] argument Pointer to the task argument. This parameter is unused.
 *
 * @return None. This task never returns. If queue creation fails, it logs the
 *         error and remains blocked indefinitely.
 */
void key_task_entry(void *argument)
{
    key_fsm_state_t key_state    = KEY_FSM_WAIT_PRESS;
    uint32_t press_first_tick    = 0U;
    uint32_t press_interval_ms   = 0U;
    key_press_event_t edge_event = {FALLING, 0U};
    key_event_t key_result       = KEY_EVENT_NONE;

    /* Step 1: Create the external result queue and the internal edge queue. */
    g_key_queue      = xQueueCreate(10U, sizeof(key_event_t));
    s_key_edge_queue = xQueueCreate(10U, sizeof(key_press_event_t));

    /* Step 2: Verify that both key queues were created successfully. */
    if ((NULL == g_key_queue) || (NULL == s_key_edge_queue))
    {
        printf("[E][KEY] Key queue creation failed.\r\n");

        for (;;)
        {
            osDelay(1000U);
        }
    }

    for (;;)
    {
        /* Step 3: Block until the ISR sends a key-edge event. */
        if (pdPASS != xQueueReceive(s_key_edge_queue,
                                    &edge_event,
                                    portMAX_DELAY))
        {
            printf("[E][KEY] Failed to receive key-edge event.\r\n");
            continue;
        }

        key_result = KEY_EVENT_NONE;

        /* Step 4: Update the key state machine with the received edge. */
        switch (key_state)
        {
            case KEY_FSM_WAIT_PRESS:
            {
                if (FALLING == edge_event.edge_type)
                {
                    press_first_tick = edge_event.trigger_tick;
                    key_state        = KEY_FSM_WAIT_RELEASE;
                }
                break;
            }

            case KEY_FSM_WAIT_RELEASE:
            {
                if (RISING == edge_event.edge_type)
                {
                    press_interval_ms =
                        edge_event.trigger_tick - press_first_tick;
                    key_state         = KEY_FSM_WAIT_PRESS;

                    if (press_interval_ms < KEY_DEBOUNCE_TIME_MS)
                    {
                        key_result = KEY_EVENT_NONE;
                    }
                    else if (press_interval_ms < KEY_LONG_PRESS_TIME_MS)
                    {
                        key_result = KEY_EVENT_SHORT_PRESS;
                    }
                    else
                    {
                        key_result = KEY_EVENT_LONG_PRESS;
                    }
                }
                else if (FALLING == edge_event.edge_type)
                {
                    press_first_tick = edge_event.trigger_tick;
                }
                break;
            }

            default:
            {
                key_state        = KEY_FSM_WAIT_PRESS;
                press_first_tick = 0U;
                printf("[E][KEY] Invalid key state.\r\n");
                break;
            }
        }

        /* Step 5: Forward a completed key result to the external queue. */
        switch (key_result)
        {
            case KEY_EVENT_SHORT_PRESS:
            case KEY_EVENT_LONG_PRESS:
            {
                if (pdPASS != xQueueSend(g_key_queue,
                                         &key_result,
                                         0U))
                {
                    printf("[E][KEY] Failed to send key result.\r\n");
                }
                break;
            }

            case KEY_EVENT_NONE:
            {
                break;
            }

            default:
            {
                printf("[E][KEY] Invalid key result.\r\n");
                break;
            }
        }
    }
}

/**
 * @brief Handles the GPIO EXTI callback for the key.
 *
 * Captures the expected key edge and its timestamp, sends the event to the
 * internal queue, and configures the opposite edge for the next interrupt.
 *
 * @param[in] GPIO_Pin GPIO pin mask reported by the HAL EXTI handler.
 *
 * @return None.
 *
 * @note Runs in ISR context. Retained printf calls remain disabled because the
 *       UART retarget uses task-context scheduler protection.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static key_trigger_edge_t next_edge = FALLING;

    if (Key_Pin != GPIO_Pin)
    {
        return;
    }

    if (FALLING == next_edge)
    {
        key_press_event_t edge_event = {FALLING, 0U};
        GPIO_InitTypeDef gpio_config = {0};

        /* Step 1: Capture the falling-edge event and its timestamp. */
        edge_event.edge_type    = FALLING;
        edge_event.trigger_tick = HAL_GetTick();

        /* Step 2: Verify that the internal key-edge queue is ready. */
        if (NULL == s_key_edge_queue)
        {
            // printf("[E][KEY] s_key_edge_queue is NULL.\r\n");
            /* Kept disabled because printf is not ISR-safe. Its UART retarget
             * uses vTaskSuspendAll() and xTaskResumeAll() for task-context
             * protection. */
        }
        else
        {
            /* Step 3: Send the falling-edge event to the internal queue. */
            if (pdPASS != xQueueSendFromISR(s_key_edge_queue,
                                            &edge_event,
                                            NULL))
            {
                // printf("[E][KEY] Failed to send falling-edge event.\r\n");
                /* Kept disabled because printf is not ISR-safe. Its UART
                 * retarget uses vTaskSuspendAll() and xTaskResumeAll() for
                 * task-context protection. */
            }
        }

        /* Step 4: Configure the next interrupt for the rising edge. */
        next_edge       = RISING;
        gpio_config.Pin  = Key_Pin;
        gpio_config.Mode = GPIO_MODE_IT_RISING;
        gpio_config.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(Key_GPIO_Port, &gpio_config);
    }
    else
    {
        key_press_event_t edge_event = {RISING, 0U};
        GPIO_InitTypeDef gpio_config = {0};

        /* Step 1: Capture the rising-edge event and its timestamp. */
        edge_event.edge_type    = RISING;
        edge_event.trigger_tick = HAL_GetTick();

        /* Step 2: Verify that the internal key-edge queue is ready. */
        if (NULL == s_key_edge_queue)
        {
            // printf("[E][KEY] s_key_edge_queue is NULL.\r\n");
            /* Kept disabled because printf is not ISR-safe. Its UART retarget
             * uses vTaskSuspendAll() and xTaskResumeAll() for task-context
             * protection. */
        }
        else
        {
            /* Step 3: Send the rising-edge event to the internal queue. */
            if (pdPASS != xQueueSendFromISR(s_key_edge_queue,
                                            &edge_event,
                                            NULL))
            {
                // printf("[E][KEY] Failed to send rising-edge event.\r\n");
                /* Kept disabled because printf is not ISR-safe. Its UART
                 * retarget uses vTaskSuspendAll() and xTaskResumeAll() for
                 * task-context protection. */
            }
        }

        /* Step 4: Configure the next interrupt for the falling edge. */
        next_edge       = FALLING;
        gpio_config.Pin  = Key_Pin;
        gpio_config.Mode = GPIO_MODE_IT_FALLING;
        gpio_config.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(Key_GPIO_Port, &gpio_config);
    }
}
