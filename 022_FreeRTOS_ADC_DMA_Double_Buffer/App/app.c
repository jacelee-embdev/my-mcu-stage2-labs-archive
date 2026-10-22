/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file app.c
 *
 * @par dependencies
 * - app.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements ADC DMA double-buffer acquisition and voltage reporting.
 *
 * Processing flow:
 *
 * app_init() allocates and initializes two ADC buffers, creates the DMA
 * completion and buffer-ready queues, and then creates the sampling and
 * processing tasks. The sampling task alternates DMA transfers between the two
 * buffers and notifies the processing task when a buffer is ready. The
 * processing task converts each 12-bit ADC sample to a voltage using VDD_VALUE
 * and reports both values through EasyLogger. The ADC callbacks report DMA
 * completion and ADC errors.
 *
 * @version V1.1 2026-08-31
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "app.h"
/****************************** Includes ******************************/

/****************************** Defines *******************************/
#define TAG               "app"
#define ADC_BUFFER_SIZE   (1U)
#define DMA_ADC_CPLT_INT  (0xA1A1A1A1)
#define BUFFER1_READY     (0x01)
#define BUFFER2_READY     (0x02)
#define ADC_MAX_RAW_VALUE (4095U) /* Maximum value of a 12-bit ADC sample. */
/****************************** Defines *******************************/

/*************************** Static Resources ************************/
static TaskHandle_t adc_sample_task_handle  = NULL;
static TaskHandle_t adc_process_task_handle = NULL;
static QueueHandle_t dma_done_queue         = NULL;
static QueueHandle_t adc_ready_queue        = NULL;

static uint32_t *p_buffer1 = NULL;
static uint32_t *p_buffer2 = NULL;

static dma_buf_state_t dma_buf_slt = DMA_BUF_NO;
/*************************** Static Resources ************************/

/**
 * @brief Acquires ADC samples with DMA double buffering.
 *
 * Starts ADC DMA with buffer 1, waits for DMA completion events, switches the
 * active DMA buffer, and notifies the processing task when a buffer is ready.
 *
 * @param[in] pvParameters Reserved task argument. The function does not use it.
 *
 * @return None. The task returns if a queue receive fails or an unexpected DMA
 *         event is received.
 */
static void adc_sample_task(void *pvParameters)
{
    uint32_t adc_dma_event         = 0xFF;
    uint32_t bufferX_ready_pattern = 0xFF;

    (void)pvParameters;

    /* Step 1: Start ADC DMA with buffer 1. */
    if (HAL_OK != HAL_ADC_Start_DMA(&hadc1, p_buffer1, ADC_BUFFER_SIZE))
    {
        elog_e(TAG, "ADC DMA start buffer1 failed.\r\n");
    }
    dma_buf_slt = DMA_BUF_1;

    for (;;)
    {
        /* Step 2: Wait for an ADC DMA completion event. */
        if (pdPASS != xQueueReceive(dma_done_queue,
                                    &adc_dma_event,
                                    portMAX_DELAY))
        {
            elog_e(TAG, "ADC queue receive failed.\r\n");
            return;
        }

        /* Step 3: Validate and clear the received event. */
        if (DMA_ADC_CPLT_INT != adc_dma_event)
        {
            return;
        }
        adc_dma_event = 0xFF;

        /* Step 4: Switch DMA buffers and publish the completed buffer. */
        switch (dma_buf_slt)
        {
            case DMA_BUF_1:
                /* Restart ADC DMA with buffer 2. */
                dma_buf_slt = DMA_BUF_2;

                if (HAL_OK != HAL_ADC_Start_DMA(&hadc1,
                                                p_buffer2,
                                                ADC_BUFFER_SIZE))
                {
                    elog_e(TAG, "ADC DMA start buffer2 failed.\r\n");
                }

                /* Notify the processing task that buffer 1 is ready. */
                bufferX_ready_pattern = BUFFER1_READY;
                if (pdTRUE == xQueueSend(adc_ready_queue,
                                         &bufferX_ready_pattern,
                                         portMAX_DELAY))
                {
                    bufferX_ready_pattern = 0xFF;
                }
                break;

            case DMA_BUF_2:
                /* Restart ADC DMA with buffer 1. */
                dma_buf_slt = DMA_BUF_1;

                if (HAL_OK != HAL_ADC_Start_DMA(&hadc1,
                                                p_buffer1,
                                                ADC_BUFFER_SIZE))
                {
                    elog_e(TAG, "ADC DMA start buffer1 failed.\r\n");
                }

                /* Notify the processing task that buffer 2 is ready. */
                bufferX_ready_pattern = BUFFER2_READY;
                if (pdTRUE == xQueueSend(adc_ready_queue,
                                         &bufferX_ready_pattern,
                                         portMAX_DELAY))
                {
                    bufferX_ready_pattern = 0xFF;
                }
                break;

            default:
                elog_e(TAG,
                       "Invalid DMA buffer state:%u\r\n",
                       dma_buf_slt);
                break;
        }
    }
}

/**
 * @brief Converts and reports samples from completed ADC DMA buffers.
 *
 * Waits for a buffer-ready message, reads the raw sample from the corresponding
 * ADC buffer, converts the 12-bit sample to millivolts using VDD_VALUE, and
 * reports the raw value and calculated voltage through EasyLogger.
 *
 * @param[in] pvParameters Reserved task argument. The function does not use it.
 *
 * @return None. The task runs indefinitely.
 */
static void adc_process_task(void *pvParameters)
{
    uint32_t bufferX_ready_recv = 0xFF;
    uint32_t adc_raw_value  = 0U;
    uint32_t adc_voltage_mv = 0U;

    (void)pvParameters;

    for (;;)
    {
        /* Step 1: Wait for a completed ADC buffer. */
        if (pdPASS != xQueueReceive(adc_ready_queue,
                                    &bufferX_ready_recv,
                                    portMAX_DELAY))
        {
            elog_e(TAG, "ADC ready queue receive failed.\r\n");
            continue;
        }

        /* Step 2: Convert and report the completed buffer. */
        switch (bufferX_ready_recv)
        {
            case BUFFER1_READY:
                /* Convert the raw sample in buffer 1 to millivolts. */
                adc_raw_value  = p_buffer1[0];
                adc_voltage_mv = (adc_raw_value * VDD_VALUE) /
                                 ADC_MAX_RAW_VALUE;

                /* Report the raw value and calculated voltage. */
                elog_i(TAG,
                       "Buffer1 raw:%lu, voltage:%lu.%03lu V\r\n",
                       adc_raw_value,
                       adc_voltage_mv / 1000U,
                       adc_voltage_mv % 1000U);
                break;

            case BUFFER2_READY:
                /* Convert the raw sample in buffer 2 to millivolts. */
                adc_raw_value  = p_buffer2[0];
                adc_voltage_mv = (adc_raw_value * VDD_VALUE) /
                                 ADC_MAX_RAW_VALUE;

                /* Report the raw value and calculated voltage. */
                elog_i(TAG,
                       "Buffer2 raw:%lu, voltage:%lu.%03lu V\r\n",
                       adc_raw_value,
                       adc_voltage_mv / 1000U,
                       adc_voltage_mv % 1000U);
                break;

            default:
                elog_e(TAG,
                       "Invalid DMA buffer state:%u\r\n",
                       bufferX_ready_recv);
                break;
        }
    }
}

/**
 * @brief Initializes the ADC application resources and tasks.
 *
 * Steps:
 * 1. Allocates two ADC sample buffers.
 * 2. Validates the allocated buffer addresses.
 * 3. Initializes both buffers to 0xFF.
 * 4. Creates and validates the application queues.
 * 5. Creates the ADC sampling task.
 * 6. Creates the ADC processing task for voltage conversion and reporting.
 *
 * @return None.
 *
 * @note Call once after EasyLogger initialization and before the scheduler
 *       starts.
 */
void app_init(void)
{
    /* Step 1: Allocate two ADC sample buffers. */
    p_buffer1 = (uint32_t *)malloc(sizeof(*p_buffer1) * ADC_BUFFER_SIZE);
    p_buffer2 = (uint32_t *)malloc(sizeof(*p_buffer2) * ADC_BUFFER_SIZE);

    /* Step 2: Validate the allocated buffer addresses. */
    if ((NULL == p_buffer1) || (NULL == p_buffer2))
    {
        elog_e(TAG, "ADC buffer allocation failed.\r\n");
        return;
    }

    /* Step 3: Initialize both buffers to 0xFF. */
    memset(p_buffer1, 0xFF, sizeof(*p_buffer1) * ADC_BUFFER_SIZE);
    memset(p_buffer2, 0xFF, sizeof(*p_buffer2) * ADC_BUFFER_SIZE);

    /* Step 4: Create and validate the application queues. */
    dma_done_queue  = xQueueCreate(10, sizeof(uint32_t));
    adc_ready_queue = xQueueCreate(10, sizeof(uint32_t));
    if ((NULL == dma_done_queue) || (NULL == adc_ready_queue))
    {
        elog_e(TAG, "Queue creation failed.\r\n");
        return;
    }

    /* Step 5: Create the ADC sampling task. */
    if (pdPASS != xTaskCreate(adc_sample_task,
                              "adc_sample",
                              128U,
                              NULL,
                              1U,
                              &adc_sample_task_handle))
    {
        elog_e(TAG, "ADC sample task creation failed.\r\n");
        return;
    }

    /* Step 6: Create the ADC voltage processing and reporting task. */
    if (pdPASS != xTaskCreate(adc_process_task,
                              "adc_process",
                              128U,
                              NULL,
                              1U,
                              &adc_process_task_handle))
    {
        elog_e(TAG, "ADC process task creation failed.\r\n");
        return;
    }
}

/**
 * @brief Handles completion of an ADC DMA conversion.
 *
 * Sends a DMA completion event to the sampling task and requests a context
 * switch when a higher-priority task is awakened.
 *
 * @param[in] hadc Pointer to the ADC handle that completed the conversion.
 *
 * @return None.
 *
 * @note This callback runs in interrupt context.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    /* Step 1: Handle completion events from ADC1 only. */
    if (&hadc1 == hadc)
    {
        /* Step 2: Prepare the DMA completion event for the ISR queue. */
        uint32_t dma_pattern_cplt = DMA_ADC_CPLT_INT;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        /* Step 3: Notify the sampling task from interrupt context. */
        if (pdPASS != xQueueSendFromISR(dma_done_queue,
                                        &dma_pattern_cplt,
                                        &xHigherPriorityTaskWoken))
        {
            elog_e(TAG, "ADC queue send failed.\r\n");
        }

        /* Step 4: Yield if the notification awakened a higher-priority task. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
 * @brief Handles errors reported by ADC1.
 *
 * Reads the ADC error code and records overrun or DMA transfer errors.
 *
 * @param[in] hadc Pointer to the ADC handle that reported the error.
 *
 * @return None.
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    /* Step 1: Handle errors reported by ADC1 only. */
    if (&hadc1 == hadc)
    {
        /* Step 2: Read the current ADC error flags. */
        uint32_t error_code = HAL_ADC_GetError(hadc);

        /* Step 3: Report an ADC overrun error. */
        if (0U != (error_code & HAL_ADC_ERROR_OVR))
        {
            elog_e(TAG,
                   "ADC overrun error, ErrorCode: 0x%08lX\r\n",
                   (unsigned long)error_code);
        }

        /* Step 4: Report a DMA transfer error. */
        if (0U != (error_code & HAL_ADC_ERROR_DMA))
        {
            elog_e(TAG,
                   "ADC DMA transfer error, ErrorCode: 0x%08lX\r\n",
                   (unsigned long)error_code);
        }
    }
}
