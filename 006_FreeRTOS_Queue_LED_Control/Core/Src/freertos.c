/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usart.h"
#include "bsp_key.h"
#include "bsp_led.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


/* USER CODE END Variables */

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);


  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    key_task_handle = osThreadNew(key_task_entry,
                                  NULL,
                                  &key_task_attributes);
    led_task_handle = osThreadNew(led_task_entry,
                                  NULL,
                                  &led_task_attributes);
                                  
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    key_state_t received_key_state = KEY_STATE_RELEASED;
    led_state_t led_command        = LED_STATE_OFF;
    
  /* Infinite loop */
  for (;;)
    {
        printf("Relay thread test\r\n");
        
        if ((NULL != key_queue) && (NULL != led_queue))
        {
            if (pdPASS == xQueueReceive(key_queue,&received_key_state,(TickType_t)100))
            {
                switch (received_key_state)
                {
                    //1.If the key is pressed
                    case KEY_STATE_PRESSED:
                    {
                        led_command = LED_STATE_TOGGLE;
                        
                        //Print a log if the transmission fails; otherwise, do not print anything.
                        if (pdPASS != xQueueSend(led_queue,&led_command,0))
                        {
                            printf("[E][RELAY] Failed to send LED command.\r\n");
                        }
                        break;
                    }
                    
                    //2.If the key is not pressed
                    case KEY_STATE_RELEASED:
                    {
                        break;
                    }
                    
                    //3.Other situation.
                    default:
                    {
                        printf("[E][RELAY] Invalid key state: %d\r\n",
                               (int)received_key_state);
                        break;
                    }
                }
            }
        }
      
    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/

/* USER CODE BEGIN Application */

/* USER CODE END Application */

