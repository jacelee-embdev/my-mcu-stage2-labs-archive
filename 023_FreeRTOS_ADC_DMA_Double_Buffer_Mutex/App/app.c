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
 * @brief Implements ADC DMA double buffering with mutex coordination.
 *
 * Processing flow:
 *
 * app_init() allocates two ADC buffers and creates the DMA completion queue,
 * buffer-ready queue, shared mutex, and two tasks. adc_sample_task() waits for
 * DMA completion and for adc_process_task() to receive and finish the previous
 * buffer before reusing it. It starts DMA on the alternate buffer, releases
 * the mutex, and publishes the completed buffer. adc_process_task() peeks for
 * a ready message, takes the mutex, receives the message, and holds the mutex
 * while converting and reporting the sample. ADC callbacks report DMA
 * completion and ADC errors.
 *
 * @version V1.2 2026-09-06
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
static TaskHandle_t      adc_sample_task_handle  = NULL;
static TaskHandle_t      adc_process_task_handle = NULL;
static QueueHandle_t     dma_done_queue          = NULL;
static QueueHandle_t     adc_ready_queue         = NULL;
static SemaphoreHandle_t adc_buffer_mutex        = NULL;

static uint32_t *p_buffer1 = NULL;
static uint32_t *p_buffer2 = NULL;

static dma_buf_state_t dma_buf_slt = DMA_BUF_NO;
/*************************** Static Resources ************************/

/**
 * @brief Acquires ADC samples with DMA double buffering and mutex coordination.
 *
 * Steps:
 * 1. Starts ADC DMA with buffer 1.
 * 2. Waits for a DMA completion event.
 * 3. Validates the completion event.
 * 4. Waits for adc_process_task() to receive the previous ready message.
 * 5. Takes the mutex to wait for previous processing and reporting to finish.
 * 6. Switches DMA buffers, releases the mutex, and publishes the ready buffer.
 *
 * @param[in] pvParameters Reserved task argument. The function does not use it.
 *
 * @return None. The task returns if the DMA-event receive fails or an
 *         unexpected completion event is received.
 *
 * @note adc_sample_task() is the only producer of adc_ready_queue, and
 *       adc_process_task() is its only consumer. The ready queue is polled
 *       without holding the mutex.
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

        /*
         * Step 4: Wait for adc_process_task() to receive the prior message.
         * Poll without removing the message or holding the mutex.
         */
        while (pdTRUE == xQueuePeek(adc_ready_queue,
                                    &bufferX_ready_pattern,
                                    0))
        {
            vTaskDelay(1U);
        }

        /*
         * Step 5: Take the mutex after the previous message is received.
         * Wait for adc_process_task() to finish processing and reporting.
         */
        while (pdTRUE != xSemaphoreTake(adc_buffer_mutex,
                                        portMAX_DELAY))
        {
            elog_e(TAG, "Mutex take failed.\r\n");
            vTaskDelay(1U);
        }

        /* Step 6: Switch DMA buffers and publish the completed buffer. */
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

                /* Release the mutex before notifying adc_process_task(). */
                if (pdTRUE != xSemaphoreGive(adc_buffer_mutex))
                {
                    elog_e(TAG,
                           "Mutex give failed, line:%d\r\n",
                           __LINE__);
                }

                /* Notify the processing task that buffer 1 is ready. */
                bufferX_ready_pattern = BUFFER1_READY;
                if (pdTRUE == xQueueSend(adc_ready_queue,
                                         &bufferX_ready_pattern,
                                         portMAX_DELAY))
                {
                    bufferX_ready_pattern = 0xFF;
                }
                else
                {
                    elog_e(TAG,
                           "ADC ready queue send failed, buffer:%lu\r\n",
                           (unsigned long)bufferX_ready_pattern);
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

                /* Release the mutex before notifying adc_process_task(). */
                if (pdTRUE != xSemaphoreGive(adc_buffer_mutex))
                {
                    elog_e(TAG,
                           "Mutex give failed, line:%d\r\n",
                           __LINE__);
                }

                /* Notify the processing task that buffer 2 is ready. */
                bufferX_ready_pattern = BUFFER2_READY;
                if (pdTRUE == xQueueSend(adc_ready_queue,
                                         &bufferX_ready_pattern,
                                         portMAX_DELAY))
                {
                    bufferX_ready_pattern = 0xFF;
                }
                else
                {
                    elog_e(TAG,
                           "ADC ready queue send failed, buffer:%lu\r\n",
                           (unsigned long)bufferX_ready_pattern);
                }
                break;

            default:
                /* Release the mutex even when the DMA state is invalid. */
                if (pdTRUE != xSemaphoreGive(adc_buffer_mutex))
                {
                    elog_e(TAG,
                           "Mutex give failed, line:%d\r\n",
                           __LINE__);
                }
                elog_e(TAG,
                       "Invalid DMA buffer state:%u\r\n",
                       dma_buf_slt);
                break;
        }
    }
}

/**
 * @brief Processes completed ADC samples while holding the shared mutex.
 *
 * Steps:
 * 1. Peeks for a ready message without removing it from the queue.
 * 2. Takes the shared mutex before receiving the message.
 * 3. Receives the ready message without blocking while holding the mutex.
 * 4. Reads the sample, converts it to millivolts, and reports it.
 * 5. Releases the mutex after processing and reporting.
 *
 * @param[in] pvParameters Reserved task argument. The function does not use it.
 *
 * @return None. The task runs indefinitely.
 *
 * @note If the ready-message receive fails, the task attempts to release the
 *       mutex before retrying. Successful processing holds the mutex through
 *       the voltage calculation and EasyLogger output.
 */
static void adc_process_task(void *pvParameters)
{
    uint32_t bufferX_ready_recv = 0xFF;
    uint32_t adc_raw_value      = 0U;
    uint32_t adc_voltage_mv     = 0U;

    (void)pvParameters;

    for (;;)
    {
        /* Step 1: Wait for a ready message without removing it. */
        if (pdPASS != xQueuePeek(adc_ready_queue,
                                 &bufferX_ready_recv,
                                 portMAX_DELAY))
        {
            elog_e(TAG, "ADC ready queue peek failed.\r\n");
            continue;
        }

        /* Step 2: Take the mutex before receiving the ready message. */
        if (pdTRUE != xSemaphoreTake(adc_buffer_mutex,
                                     portMAX_DELAY))
        {
            elog_e(TAG, "Mutex take failed.\r\n");
            continue;
        }

        /* Step 3: Receive the ready message without blocking. */
        if (pdPASS != xQueueReceive(adc_ready_queue,
                                    &bufferX_ready_recv,
                                    0))
        {
            elog_e(TAG, "ADC ready queue receive failed.\r\n");

            /* Attempt to release the mutex before retrying the receive. */
            if (pdTRUE != xSemaphoreGive(adc_buffer_mutex))
            {
                elog_e(TAG,
                       "Mutex give failed, line:%d\r\n",
                       __LINE__);
            }

            continue;
        }

        /* Step 4: Read, convert, and report the buffer under the mutex. */
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

        /* Step 5: Release the mutex after processing and reporting. */
        if (pdTRUE != xSemaphoreGive(adc_buffer_mutex))
        {
            elog_e(TAG,
                   "Mutex give failed, line:%d\r\n",
                   __LINE__);
        }
    }
}

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

    /* Step 4: Create and validate the DMA and ready-message queues. */
    dma_done_queue  = xQueueCreate(10, sizeof(uint32_t));
    adc_ready_queue = xQueueCreate(10, sizeof(uint32_t));
    if ((NULL == dma_done_queue) || (NULL == adc_ready_queue))
    {
        elog_e(TAG, "Queue creation failed.\r\n");
        return;
    }

    /* Step 5: Create and validate the shared buffer mutex. */
    adc_buffer_mutex = xSemaphoreCreateMutex();
    if (NULL == adc_buffer_mutex)
    {
        elog_e(TAG, "ADC buffer mutex creation failed.\r\n");
        return;
    }

    /* Step 6: Create the ADC sampling task. */
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

    /* Step 7: Create the ADC voltage processing and reporting task. */
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
 * @brief Reports an ADC1 DMA completion event to the sampling task.
 *
 * Steps:
 * 1. Handles completion events from ADC1 only.
 * 2. Prepares the completion event and task-wakeup flag.
 * 3. Sends the event to dma_done_queue using the ISR-safe queue API.
 * 4. Requests a context switch if a higher-priority task is awakened.
 *
 * @param[in] hadc Pointer to the ADC handle that completed the conversion.
 *
 * @return None.
 *
 * @note Runs in interrupt context. This callback does not take or release
 *       adc_buffer_mutex.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    /* Step 1: Handle completion events from ADC1 only. */
    if (&hadc1 == hadc)
    {
        /* Step 2: Prepare the DMA completion event for the ISR queue. */
        uint32_t   dma_pattern_cplt         = DMA_ADC_CPLT_INT;
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
 * @brief Reports ADC1 overrun and DMA transfer errors.
 *
 * Steps:
 * 1. Handles errors from ADC1 only.
 * 2. Reads the ADC error flags.
 * 3. Logs an ADC overrun error when its flag is set.
 * 4. Logs a DMA transfer error when its flag is set.
 *
 * @param[in] hadc Pointer to the ADC handle that reported the error.
 *
 * @return None.
 *
 * @note Called by the HAL ADC/DMA interrupt handlers. The callback only logs
 *       errors; it does not restart acquisition or change buffer ownership.
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
