/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_key.h
 *
 * @par dependencies
 * - stdint.h
 * - stdio.h
 * - main.h
 * - cmsis_os.h
 * - FreeRTOS.h
 * - queue.h
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
 * @version V1.1 2026-08-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

/****************************** Includes ******************************/
#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "cmsis_os.h"

#include "FreeRTOS.h"
#include "queue.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define KEY_DEBOUNCE_TIME_MS    (10U)   /* Minimum valid press time in ms. */
#define KEY_LONG_PRESS_TIME_MS  (500U)  /* Long-press threshold in ms. */

typedef enum
{
    FALLING = 0,  /* Key-press falling edge. */
    RISING  = 1,  /* Key-release rising edge. */
} key_trigger_edge_t; /* Edge type captured by the key EXTI callback. */

typedef struct
{
    key_trigger_edge_t edge_type;  /* Captured key-edge type. */
    uint32_t trigger_tick;         /* HAL tick captured at the edge. */
} key_press_event_t; /* Timestamped raw key-edge event. */

typedef enum
{
    KEY_EVENT_NONE        = 0,  /* No key press event was detected. */
    KEY_EVENT_SHORT_PRESS = 1,  /* A short key press was detected. */
    KEY_EVENT_LONG_PRESS  = 2,  /* A long key press was detected. */
} key_event_t; /* Detected key press event. */

typedef enum
{
    KEY_FSM_WAIT_PRESS   = 0,  /* Wait for the key falling edge. */
    KEY_FSM_WAIT_RELEASE = 1,  /* Wait for the key rising edge. */
} key_fsm_state_t; /* Key-task state-machine state. */

/****************************** Defines ******************************/

/****************************** Declaring ******************************/
/************************ Thread Resources ************************/
extern osThreadId_t key_task_handle;
extern const osThreadAttr_t key_task_attributes;

/************************ Queue Resources ************************/
extern QueueHandle_t g_key_queue;

/************************ API Declaration ************************/

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
void key_task_entry(void *argument);

/****************************** Declaring ******************************/

#endif /* End of __BSP_KEY_H__ */
