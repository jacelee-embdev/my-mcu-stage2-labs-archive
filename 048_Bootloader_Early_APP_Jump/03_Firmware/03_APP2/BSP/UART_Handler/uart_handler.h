/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file uart_handler.h
 *
 * @par dependencies
 * - os_init.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the HAL UART receive-event callback.
 *
 * Processing flow:
 *
 * The HAL invokes this callback from UART or DMA interrupt context.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __UART_HANDLER_H__
#define __UART_HANDLER_H__

/********************************* Includes *********************************/
#include "os_init.h"
/********************************* Includes *********************************/

/************************** Function Declarations ***************************/
/**
 * @brief 向接收线程转发 USART1 的空闲或传输完成长度。
 *
 * 由 HAL 在中断上下文调用，通过接收长度队列通知任务。
 *
 * @param[in] huart 产生事件的串口句柄。
 * @param[in] Size 本次事件对应的接收字节数。
 *
 * @return 无。
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
/************************** Function Declarations ***************************/

#endif /* 结束头文件保护。 */
