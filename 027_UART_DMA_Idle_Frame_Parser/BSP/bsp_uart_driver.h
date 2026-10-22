/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bsp_uart_driver.h
 *
 * @par dependencies
 * - FreeRTOS.h
 * - task.h
 * - usart.h
 * - circular_buffer.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the USART1 Receive-to-Idle circular-DMA interface.
 *
 * Processing flow:
 *
 * Call bsp_uart_rx_start() from the packet-parser task. The driver starts
 * circular DMA over the application-owned buffer, publishes new bytes from
 * HT, TC, and IDLE events, and wakes the calling task through a direct task
 * notification.
 *
 * @version V1.1 2026-09-12
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BSP_UART_DRIVER_H__
#define __BSP_UART_DRIVER_H__

/****************************** Includes ******************************/
#include "FreeRTOS.h"           /* OS layer. */
#include "task.h"

#include "usart.h"              /* Core layer. */

#include "circular_buffer.h"    /* Middleware layer. */
/****************************** Includes ******************************/

/****************************** Declaring ******************************/
/************************ API Declaration ************************/
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
 * @retval HAL_OK Reception started successfully.
 * @retval HAL_ERROR An input is invalid or DMA is not circular.
 * @retval HAL_BUSY The UART already has an active receive operation.
 *
 * @note Call this function from the task that must receive DMA data-ready
 *       notifications.
 */
HAL_StatusTypeDef bsp_uart_rx_start(UART_HandleTypeDef *p_huart,
                                    circular_buf_t *p_buffer);
/****************************** Declaring ******************************/

#endif /* End of __BSP_UART_DRIVER_H__ */
