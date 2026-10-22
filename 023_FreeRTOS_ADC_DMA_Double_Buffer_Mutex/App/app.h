/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file app.h
 *
 * @par dependencies
 * - stdlib.h
 * - string.h
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - semphr.h
 * - elog.h
 * - main.h
 * - adc.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the ADC double-buffer application with mutex coordination.
 *
 * Processing flow:
 *
 * Call app_init() once after EasyLogger initialization and before the scheduler
 * starts. It allocates two ADC buffers and creates the DMA completion queue,
 * buffer-ready queue, mutex, and two tasks. adc_sample_task() coordinates
 * buffer reuse with adc_process_task() through the ready queue and mutex.
 * adc_process_task() takes the mutex before receiving a ready message and
 * holds it while reading, converting, and reporting the sample.
 *
 * @version V1.2 2026-09-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __APP_H__
#define __APP_H__

/****************************** Includes ******************************/

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "elog.h"
#include "main.h"
#include "adc.h"

/****************************** Includes ******************************/

/****************************** Defines *******************************/
typedef enum
{
    DMA_BUF_NO = 0, /* No DMA buffer is selected. */
    DMA_BUF_1,      /* DMA buffer 1 is active.     */
    DMA_BUF_2       /* DMA buffer 2 is active.     */
} dma_buf_state_t;
/****************************** Defines *******************************/

/****************************** Declaring *****************************/

/**
 * @brief Initializes the ADC buffers, synchronization resources, and tasks.
 *
 * Steps:
 * 1. Allocates two ADC sample buffers.
 * 2. Validates the allocated buffer addresses.
 * 3. Initializes both buffers to 0xFF.
 * 4. Creates and validates the DMA completion and buffer-ready queues.
 * 5. Creates and validates the shared buffer mutex.
 * 6. Creates the ADC sampling task.
 * 7. Creates the ADC processing task for voltage conversion and reporting.
 *
 * @return None. Resource or task creation failures are logged and stop the
 *         remaining initialization.
 *
 * @note Call once after EasyLogger initialization and before the scheduler
 *       starts.
 */
void app_init(void);
/****************************** Declaring *****************************/

#endif /* __APP_H__ */
