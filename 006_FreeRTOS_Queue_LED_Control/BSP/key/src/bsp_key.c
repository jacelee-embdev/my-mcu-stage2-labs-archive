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
 * @brief Provide the BSP APIs for key input scanning and related RTOS operations.
 *
 * Processing flow:
 *
 *Call key_scan() directly, or run key_task_entry() as an RTOS thread.
 *
 * @version V1.0 2026-07-22
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/
 
#include "bsp_key.h"

//*********************Thread_Func **********************//
osThreadId_t key_task_handle;
const osThreadAttr_t key_task_attributes = {
    .name = "key_task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
//*********************Thread_Func **********************//

//*********************Queue_Handler ********************//
QueueHandle_t key_queue;
//*********************Queue_Handler ********************//

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
key_scan_result_t key_scan(key_state_t *p_key_state)
{
    uint32_t poll_count = 0;
    key_state_t key_state = KEY_STATE_RELEASED;
    
    if (NULL == p_key_state)
    {
        return KEY_SCAN_INVALID_PARAMETER;
    }

    while (poll_count < 10000)
    {
        //1.If the key is pressed.
        if (HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin) == GPIO_PIN_RESET)
        {
            key_state = KEY_STATE_PRESSED;
            *p_key_state = key_state;
            return KEY_SCAN_OK;
        }
        
        //2.If the key isn't pressed.
        poll_count++;
    }

    *p_key_state = key_state;
    return KEY_SCAN_TIMEOUT;
}

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
void key_task_entry(void *argument)
{
    key_scan_result_t scan_result = KEY_SCAN_ERROR;
    key_state_t key_state         = KEY_STATE_RELEASED;

    key_queue = xQueueCreate(10,sizeof(key_state_t));
    
    //Before using the queue, first confirm that it has been successfully created.
    if(NULL == key_queue)
    {
        printf("key_queue created failed.\r\n");
        return;
    }
    
    for (;;)
    {
        printf("KEY thread test.\r\n");
        
        scan_result = key_scan(&key_state);
        
        switch (scan_result)
        {
            //1.If the key scan is successful
            case KEY_SCAN_OK:
            {
                //1.1 If the key is pressed
                if (KEY_STATE_PRESSED == key_state)
                {
                    //If the sending is successful, do not log it. If it fails, then print a log.
                    if (pdPASS != xQueueSend(key_queue,
                                            &key_state,
                                            0))
                    {
                        printf("[E][KEY] Failed to send key event.\r\n");
                    }
                }
                
                //1.2 If the key isn't pressed
                else
                {
                    printf("[E][KEY] Inconsistent scan state: %d\r\n",(int)key_state);
                }
                break;
            }
            
            //2.If the key scan times out.
            case KEY_SCAN_TIMEOUT:
            {
                break;
            }
            
            //3.If the key scanning fails,
            default:
            {
                printf("[E][KEY] Key scan failed: %d\r\n",(int)scan_result);
                break;
            }
        }
        osDelay(500);
    }
}
