/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_uart_driver.c
 *
 * @par dependencies
 * - bsp_uart_driver.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements USART1 Receive-to-Idle reception with circular DMA.
 *
 * Processing flow:
 *
 * bsp_uart_rx_start() binds one application-owned buffer to USART1 and starts
 * DMA once. HAL_UARTEx_RxEventCallback() separates HT, TC, and IDLE events.
 * Each handler publishes only the bytes added since the previous event and
 * wakes the packet-parser task directly from interrupt context.
 *
 * @version V1.1 2026-09-12
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "bsp_uart_driver.h"
/****************************** Includes ******************************/

/*************************** Static Resources ************************/
static UART_HandleTypeDef *g_p_rx_huart = NULL;
static circular_buf_t *g_p_rx_buffer = NULL;
static TaskHandle_t g_parser_task = NULL;
static volatile uint16_t g_last_dma_size = 0U;
/*************************** Static Resources ************************/

/**
 * @brief Publishes bytes added since the previous DMA receive event.
 *
 * Size is the current position in the DMA array, not the byte count added by
 * this event. The previous Size is retained so the increment can be calculated
 * before and after DMA wraps at BUFFER_SIZE.
 *
 * Steps:
 * 1. Validates the bound receive context and DMA position.
 * 2. Calculates the new byte count from Size and g_last_dma_size.
 * 3. Advances head_total when the event publishes new data.
 * 4. Wakes the packet-parser task from interrupt context.
 *
 * @param[in] size Current DMA write position reported by HAL.
 *
 * @return None.
 */
static void uart_rx_publish(uint16_t size)
{
    uint32_t new_data_length = 0U;
    BaseType_t higher_priority_task_woken = pdFALSE;

    /* Step 1: Validate the receive context and reported position. */
    if ((NULL == g_p_rx_buffer) ||
        (NULL == g_parser_task) ||
        (0U == size)            ||
        (BUFFER_SIZE < size))
    {
        return;
    }

    /* Step 2: Calculate bytes added before or after DMA wraparound. */
    if (size >= g_last_dma_size)
    {
        new_data_length = (uint32_t)size - g_last_dma_size;
    }
    else
    {
        new_data_length = (BUFFER_SIZE - g_last_dma_size) + size;
    }
    g_last_dma_size = size;

    /* Step 3: Ignore a repeated event at the same DMA position. */
    if (0U == new_data_length)
    {
        return;
    }

    g_p_rx_buffer->head_total += new_data_length;

    /* Step 4: Wake the parser after publishing the new head position. */
    vTaskNotifyGiveFromISR(g_parser_task, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

/**
 * @brief Handles a DMA half-transfer receive event.
 *
 * @param[in] size Current DMA write position.
 *
 * @return None.
 */
static void uart_rx_ht_callback(uint16_t size)
{
    uart_rx_publish(size);
}

/**
 * @brief Handles a DMA transfer-complete receive event.
 *
 * @param[in] size Current DMA write position.
 *
 * @return None.
 */
static void uart_rx_tc_callback(uint16_t size)
{
    uart_rx_publish(size);
}

/**
 * @brief Handles a UART idle receive event.
 *
 * @param[in] size Current DMA write position.
 *
 * @return None.
 */
static void uart_rx_idle_callback(uint16_t size)
{
    uart_rx_publish(size);
}

/**
 * @brief Starts USART1 Receive-to-Idle reception in circular DMA mode.
 *
 * Steps:
 * 1. Validates the UART handle, DMA mode, and receive buffer.
 * 2. Records the calling task as the receive-notification target.
 * 3. Resets the cumulative buffer positions and previous DMA position.
 * 4. Starts DMA reception over the complete receive array.
 *
 * @param[in] p_huart Pointer to the USART1 HAL handle.
 * @param[in,out] p_buffer Pointer to the application-owned receive buffer.
 *
 * @return HAL_StatusTypeDef Result of the receive-start operation.
 */
HAL_StatusTypeDef bsp_uart_rx_start(UART_HandleTypeDef *p_huart,
                                    circular_buf_t *p_buffer)
{
    TaskHandle_t parser_task = NULL;
    HAL_StatusTypeDef hal_status = HAL_ERROR;

    /* Step 1: Validate the UART, its DMA configuration, and the buffer. */
    if ((NULL == p_huart)                ||
        (USART1 != p_huart->Instance)     ||
        (NULL == p_huart->hdmarx)         ||
        (DMA_CIRCULAR != p_huart->hdmarx->Init.Mode) ||
        (NULL == p_buffer))
    {
        return HAL_ERROR;
    }

    /* Step 2: Record the calling task as the notification target. */
    parser_task = xTaskGetCurrentTaskHandle();
    if (NULL == parser_task)
    {
        return HAL_ERROR;
    }

    g_p_rx_huart = p_huart;
    g_p_rx_buffer = p_buffer;
    g_parser_task = parser_task;

    /* Step 3: Reset the cumulative and DMA-relative positions. */
    p_buffer->head_total = 0U;
    p_buffer->tail_total = 0U;
    g_last_dma_size = 0U;

    /* Step 4: Start circular DMA reception over the complete array. */
    hal_status = HAL_UARTEx_ReceiveToIdle_DMA(p_huart,
                                              p_buffer->data,
                                              BUFFER_SIZE);
    if (HAL_OK != hal_status)
    {
        return hal_status;
    }

    return HAL_OK;
}

/**
 * @brief Handles USART1 Receive-to-Idle DMA events.
 *
 * Dispatches HT, TC, and IDLE events to separate handlers. All handlers use
 * the same position-based publisher, so repeated events at the same Size do
 * not publish the same bytes twice.
 *
 * @param[in] p_huart Pointer to the UART handle that generated the event.
 * @param[in] size Current DMA write position in the circular receive array.
 *
 * @return None.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *p_huart, uint16_t size)
{
    HAL_UART_RxEventTypeTypeDef rx_event_type = HAL_UART_RXEVENT_TC;

    /* Step 1: Accept receive events from the configured USART1 only. */
    if ((NULL == p_huart)            ||
        (p_huart != g_p_rx_huart)     ||
        (USART1 != p_huart->Instance))
    {
        return;
    }

    /* Step 2: Obtain and dispatch the receive-event type. */
    rx_event_type = HAL_UARTEx_GetRxEventType(p_huart);
    switch (rx_event_type)
    {
        case HAL_UART_RXEVENT_HT:
        {
            uart_rx_ht_callback(size);
            break;
        }

        case HAL_UART_RXEVENT_TC:
        {
            uart_rx_tc_callback(size);
            break;
        }

        case HAL_UART_RXEVENT_IDLE:
        {
            uart_rx_idle_callback(size);
            break;
        }

        default:
        {
            break;
        }
    }
}
