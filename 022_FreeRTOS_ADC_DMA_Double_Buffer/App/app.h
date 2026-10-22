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
 * - elog.h
 * - main.h
 * - adc.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the ADC double-buffer application interface and resources.
 *
 * Processing flow:
 *
 * app_init() allocates and initializes two ADC buffers, creates the DMA
 * completion and buffer-ready queues, and then creates the sampling and
 * processing tasks. The application alternates DMA transfers between the two
 * buffers. The processing task converts each completed 12-bit ADC sample to a
 * voltage and reports the raw and calculated values through EasyLogger.
 *
 * @version V1.1 2026-08-31
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
 * @brief Initializes ADC double-buffer resources and application tasks.
 *
 * Allocates and initializes two ADC buffers, creates the DMA completion and
 * buffer-ready queues, and then creates the ADC sampling and voltage-processing
 * tasks. If resource allocation or task creation fails, the function records
 * an error and stops the remaining initialization.
 *
 * @return None.
 *
 * @note Call once after EasyLogger initialization and before the scheduler
 *       starts.
 */
void app_init(void);
/****************************** Declaring *****************************/

#endif /* __APP_H__ */
