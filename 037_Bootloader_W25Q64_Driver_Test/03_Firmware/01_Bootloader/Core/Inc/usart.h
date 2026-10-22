/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file usart.h
 *
 * @par dependencies
 * - stm32f4xx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the USART1 initialization and polling communication APIs.
 *
 * Processing flow:
 *
 * Call USART1_Init() once before using USART_SendChar() or
 * USART_ReceiveChar().
 *
 * @version V1.0 2026-09-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __USART_H__
#define __USART_H__

/****************************** Includes ******************************/
#include "stm32f4xx.h"
/****************************** Includes ******************************/

/************************ Function Declarations ************************/
/**
 * @brief Initializes USART1 and its GPIO pins.
 *
 * Enables the GPIOA and USART1 peripheral clocks, configures PA9 and PA10
 * for the USART1 alternate function, and enables USART1 at 115200 baud.
 *
 * @param None.
 *
 * @return None.
 */
void USART1_Init(void);

/**
 * @brief Sends one byte through the selected USART peripheral.
 *
 * Waits until the transmit data register is empty and then writes one byte.
 *
 * @param[in] USARTx Pointer to the USART peripheral instance.
 * @param[in] data   Byte to transmit.
 *
 * @return None.
 */
void USART_SendChar(USART_TypeDef *USARTx, uint8_t data);

/**
 * @brief Receives one byte from the selected USART peripheral.
 *
 * Waits until received data is available and returns the low eight bits.
 *
 * @param[in] USARTx Pointer to the USART peripheral instance.
 *
 * @return uint8_t Received byte.
 */
uint8_t USART_ReceiveChar(USART_TypeDef *USARTx);
/************************ Function Declarations ************************/

#endif /* End of __USART_H__ */
