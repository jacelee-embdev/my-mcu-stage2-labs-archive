/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file led.c
 *
 * @par dependencies
 * - led.h
 * - stm32f4xx_hal_rcc.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements active-low status LED initialization and control.
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

/********************************** Includes **********************************/

#include "led.h"                   /* Module header                           */

#include "stm32f4xx_hal_rcc.h"     /* MCU SDK / HAL                           */

/********************************** Includes **********************************/

/**************************** Function Definitions ****************************/

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
void LED_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure = {0};

    /************* 1. Enable the GPIOC clock **************/
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /***************** 2. Configure PC13 ******************/
    gpio_init_structure.Pin   =          LED_C13_PIN;
    gpio_init_structure.Mode  =  GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  =          GPIO_NOPULL;
    gpio_init_structure.Speed =  GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_C13_PORT, &gpio_init_structure);

    /**************** 3. Turn off the LED *****************/
    LED_Off();
}

/**
 * @brief Drive PC13 low to turn on the status LED.
 *
 * @return None.
 *
 * @note Call LED_Init() first.
 */
void LED_On(void)
{
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Drive PC13 high to turn off the status LED.
 *
 * @return None.
 *
 * @note Call LED_Init() first.
 */
void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_SET);
}

/**
 * @brief Toggle PC13 to change the status LED state.
 *
 * @return None.
 *
 * @note Call LED_Init() first.
 */
void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_C13_PORT, LED_C13_PIN);
}

/**************************** Function Definitions ****************************/
