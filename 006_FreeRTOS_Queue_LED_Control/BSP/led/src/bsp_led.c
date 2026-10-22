/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_led.c
 *
 * @par dependencies
 * - bsp_led.h
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
 
#include "bsp_led.h"

//*********************Thread_Func **********************//
osThreadId_t              led_task_handle;
const osThreadAttr_t led_task_attributes = {
  .name = "led_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
//*********************Thread_Func **********************//

//*********************Queue_Handler ********************//
QueueHandle_t led_queue;
//*********************Queue_Handler ********************//


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
led_operate_result_t led_operate(led_state_t led_state)
{
    led_operate_result_t ret = LED_OPERATE_ERROR;
    
    //1.Turn on the LED.
    if(LED_STATE_ON == led_state)
    {
        
        HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);
        ret = LED_OPERATE_OK;
        
        return ret;
    }
    
    //2.Turn off the LED.
    if(LED_STATE_OFF == led_state)
    {
        HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
        ret = LED_OPERATE_OK;
        
        return ret;
    }
    
    //3.Toggle the LED.
    if(LED_STATE_TOGGLE == led_state)
    {
        HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);
        ret = LED_OPERATE_OK;
        printf("LED toggle successfully.\r\n");
        return ret;
    }
    
    return ret;
}


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
void led_task_entry(void *argument)
{
    led_state_t            led_state             =    LED_STATE_OFF;
    led_operate_result_t led_operate_result      =    LED_OPERATE_ERROR;
    led_queue = xQueueCreate( 10, sizeof( led_state_t ) );
    
    //Check if the LED queue was successfully created before use.
    if (NULL == led_queue )
    {
        printf("led_queue created failed \r\n");
        return;
    }
    
    for(;;)
    {
        printf("LED thread test.\r\n");
        
        if( led_queue != 0 )
        {
            if( xQueueReceive( led_queue, &( led_state ), ( TickType_t ) 100 ) )
            {
                led_operate_result = led_operate(led_state);
                
                //If the execution fails, print a log; if it succeeds, do not.
                if(LED_OPERATE_OK != led_operate_result)
                {
                    printf("[E][LED] Operation failed, state=%d, result=%d\r\n",
                            (int)led_state,
                            (int)led_operate_result);
                }
            }
        }
        
    osDelay(500);
    }
}
