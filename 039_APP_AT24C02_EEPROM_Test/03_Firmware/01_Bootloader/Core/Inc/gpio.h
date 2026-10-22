/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file gpio.h
 *
 * @par dependencies
 * - stm32f4xx.h
 * - delay.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the GPIO interfaces for the user key and status LED.
 *
 * Processing flow:
 *
 * Call key_init() and led_init() once before using key_scan(), LED_ON(),
 * or LED_OFF().
 *
 * @version V1.0 2026-09-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__

/****************************** Includes ******************************/
#include "stm32f4xx.h"
#include "delay.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define KEY_GPIO_PORT    GPIOA
#define KEY_GPIO_PIN     GPIO_Pin_0

#define LED_C13_PORT     GPIOC
#define LED_C13_PIN      GPIO_Pin_13

#define KEY_RELEASED     ((uint8_t)0U)
#define KEY_PRESSED      ((uint8_t)1U)

/** @brief Turns off the active-low status LED. */
#define LED_OFF()        GPIO_SetBits(LED_C13_PORT, LED_C13_PIN)

/** @brief Turns on the active-low status LED. */
#define LED_ON()         GPIO_ResetBits(LED_C13_PORT, LED_C13_PIN)
/****************************** Defines ******************************/

/************************ Function Declarations ************************/
/**
 * @brief Initializes the user-key GPIO input.
 *
 * Enables the GPIOA peripheral clock and configures PA0 as an active-low
 * input with an internal pull-up resistor.
 *
 * @param None.
 *
 * @return None.
 */
void key_init(void);

/**
 * @brief Scans and debounces the user key.
 *
 * When the active-low key is detected, waits 50 ms and samples it again.
 * The key is reported as pressed only when both samples are low.
 *
 * @param None.
 *
 * @return uint8_t Debounced key state.
 * @retval KEY_PRESSED  The key remains pressed after debouncing.
 * @retval KEY_RELEASED The key is released.
 */
uint8_t key_scan(void);

/**
 * @brief Initializes the status-LED GPIO output.
 *
 * Enables the GPIOC peripheral clock and configures PC13 as a push-pull
 * output without an internal pull-up or pull-down resistor.
 *
 * @param None.
 *
 * @return None.
 */
void led_init(void);
/************************ Function Declarations ************************/

#endif /* End of __GPIO_H__ */
