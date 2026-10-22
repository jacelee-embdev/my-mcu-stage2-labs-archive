/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file usart.c
 *
 * @par dependencies
 * - usart.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements USART1 initialization and polling communication.
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

/****************************** Includes ******************************/
#include "usart.h"
/****************************** Includes ******************************/

/********************** Private Function Declarations **********************/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void USART_Configuration(void);
/********************** Private Function Declarations **********************/

/**
 * @brief Enables the clocks required by USART1.
 *
 * Enables the GPIOA clock for the USART pins and the APB2 clock for USART1.
 *
 * @param None.
 *
 * @return None.
 */
static void RCC_Configuration(void)
{
    // 1. Enable the GPIOA peripheral clock.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 2. Enable the USART1 peripheral clock.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/**
 * @brief Configures the USART1 transmit and receive GPIO pins.
 *
 * Configures PA9 as USART1_TX and PA10 as USART1_RX using alternate-function
 * mode, push-pull output type, and internal pull-up resistors.
 *
 * @param None.
 *
 * @return None.
 */
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 1. Configure PA9 as the USART1 transmit pin.
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. Apply the same configuration to the USART1 receive pin on PA10.
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. Connect PA9 and PA10 to the USART1 alternate function.
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

/**
 * @brief Configures and enables the USART1 peripheral.
 *
 * Selects 115200 baud, eight data bits, one stop bit, no parity, no hardware
 * flow control, and enables both reception and transmission.
 *
 * @param None.
 *
 * @return None.
 */
static void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure = {0};

    // 1. Configure the USART1 communication parameters.
    USART_InitStructure.USART_BaudRate = 115200U;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    // 2. Apply the configuration and enable USART1.
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

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
void USART_SendChar(USART_TypeDef *USARTx, uint8_t data)
{
    // 1. Wait until the transmit data register is empty.
    while (RESET == USART_GetFlagStatus(USARTx, USART_FLAG_TXE))
    {
    }

    // 2. Write the byte to the USART data register.
    USART_SendData(USARTx, data);
}

/**
 * @brief Receives one byte from the selected USART peripheral.
 *
 * Waits until received data is available and returns the low eight bits.
 *
 * @param[in] USARTx Pointer to the USART peripheral instance.
 *
 * @return uint8_t Received byte.
 */
uint8_t USART_ReceiveChar(USART_TypeDef *USARTx)
{
    // 1. Wait until the receive data register contains unread data.
    while (RESET == USART_GetFlagStatus(USARTx, USART_FLAG_RXNE))
    {
    }

    // 2. Read and return the received byte.
    return (uint8_t)USART_ReceiveData(USARTx);
}

/**
 * @brief Initializes USART1 and its GPIO pins.
 *
 * Enables the required clocks, configures the GPIO alternate functions,
 * configures USART1, and enables the peripheral.
 *
 * @param None.
 *
 * @return None.
 */
void USART1_Init(void)
{
    // 1. Enable the clocks used by USART1 and its GPIO pins.
    RCC_Configuration();

    // 2. Configure the USART1 transmit and receive pins.
    GPIO_Configuration();

    // 3. Configure and enable the USART1 peripheral.
    USART_Configuration();
}
