/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file uart_handler.c
 *
 * @par dependencies
 * - uart_handler.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Forwards UART receive events to the YMODEM task.
 *
 * Processing flow:
 *
 * The HAL calls the receive-event callback from interrupt context.
 * Forward USART1 IDLE and transfer-complete lengths through the queue.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************* Includes *********************************/
#include "uart_handler.h"
/********************************* Includes *********************************/

/******************************** Functions *********************************/
/**
 * @brief 处理串口接收事件，并通知接收线程。
 *
 * 1. 仅处理 USART1 的空闲和传输完成事件。
 * 2. 将接收长度从中断上下文写入队列，接收数据仍在原缓冲区中。
 * 3. 根据队列操作的唤醒结果请求任务切换。
 * 当前普通 DMA 模式下，HAL 已在事件回调前结束本次接收。
 *
 * @param[in] huart 产生接收事件的串口句柄。
 * @param[in] Size 本次事件对应的接收字节数。
 *
 * @return 无。
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    UNUSED(huart);
    UNUSED(Size);

    uint16_t t_u16_rec_len = 0;  /* 待发送的接收长度。 */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  /* 任务唤醒标记。 */

    // 1. 仅向队列转发 USART1 的接收事件。
    if (huart->Instance == USART1)
    {
        HAL_UART_RxEventTypeTypeDef event;  /* HAL 记录的接收事件类型。 */
        event = HAL_UARTEx_GetRxEventType(huart);

        // 2. 忽略半传输事件，转发空闲或传输完成事件。
        if ((event == HAL_UART_RXEVENT_IDLE) ||
            (event == HAL_UART_RXEVENT_TC))
        {
            t_u16_rec_len = Size;
            xQueueSendFromISR(g_YmodemReclength_queue,
                              &t_u16_rec_len,
                              &xHigherPriorityTaskWoken);
        }
    }

    // 3. 仅在队列操作唤醒更高优先级任务时请求切换。
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/******************************** Functions *********************************/
