/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file led.h
 *
 * @par dependencies
 * - stm32f4xx_hal.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares active-low status LED initialization and control APIs.
 *
 * Processing flow:
 *
 * Call LED_Init() to configure PC13 and turn the LED off.
 * Then call LED_On(), LED_Off(), or LED_Toggle() to control the LED.
 * LED_ON() and LED_OFF() provide direct GPIO control after initialization.
 *
 * @version v1.0 2026-10-16
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __LED_H__
#define __LED_H__

/********************************** Includes **********************************/

#include "stm32f4xx_hal.h"         /* MCU SDK / HAL                           */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define LED_C13_PORT  GPIOC        /* Status LED GPIO port.                   */
#define LED_C13_PIN   GPIO_PIN_13  /* Status LED GPIO pin.                    */

/**
 * @brief Drive PC13 high to turn off the status LED.
 * @note Call LED_Init() first.
 */
#define LED_OFF() \
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_SET)

/**
 * @brief Drive PC13 low to turn on the status LED.
 * @note Call LED_Init() first.
 */
#define LED_ON()  \
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_RESET)

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

/**
 * @brief Initialize the active-low status LED and turn it off.
 *
 * Processing steps:
 * 1. Enable the GPIOC peripheral clock.
 * 2. Configure PC13 as a low-speed push-pull output with no pull resistor.
 * 3. Call LED_Off() to drive the pin high.
 *
 * @return None.
 */
void LED_Init(void);

/**
 * @brief Drive PC13 low to turn on the status LED.
 *
 * @return None.
 *
 * @note Call LED_Init() first.
 */
void LED_On(void);

/**
 * @brief Drive PC13 high to turn off the status LED.
 *
 * @return None.
 *
 * @note Call LED_Init() first.
 */
void LED_Off(void);

/**
 * @brief Toggle PC13 to change the status LED state.
 *
 * @return None.
 *
 * @note Call LED_Init() first.
 */
void LED_Toggle(void);

/********************************* Declaring **********************************/

#endif  /* __LED_H__ */
