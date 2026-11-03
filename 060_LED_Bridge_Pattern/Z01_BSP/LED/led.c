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
 * Then call LED_ON(), LED_OFF(), or LED_Toggle() to control the LED.
 *
 * @version v1.0 2026-10-28
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "led.h"                  /* Module header                            */

#include "stm32f4xx_hal_rcc.h"    /* MCU SDK / HAL                            */

/********************************** Includes **********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Initialize the active-low status LED and leave it off.
 *
 * Processing steps:
 * 1. Enable the GPIOC peripheral clock.
 * 2. Configure PC13 as a push-pull output without pull-up or pull-down.
 * 3. Drive the pin high after configuration to turn the LED off.
 *
 * @return None.
 */
void LED_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure = {0};  /* LED pin configuration.    */

    /**************** 1. Enable GPIO clock ****************/
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /************** 2. Configure output pin ***************/
    gpio_init_structure.Pin   =         LED_C13_PIN;
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  =         GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_C13_PORT, &gpio_init_structure);

    /**************** 3. Turn off the LED *****************/
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_SET);
}

/**
 * @brief Drive the output low to turn on the status LED.
 *
 * @return led_status_t : Operation status.
 * @retval LED_OK The GPIO write call ran; the actual pin level is not checked.
 *
 * @note Call LED_Init() before using this function.
 */
led_status_t LED_ON(void)
{
    /* Drive the active-low LED pin low. */
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_RESET);

    return LED_OK;
}

/**
 * @brief Drive the output high to turn off the status LED.
 *
 * @return led_status_t : Operation status.
 * @retval LED_OK The GPIO write call ran; the actual pin level is not checked.
 *
 * @note Call after LED_Init() has configured the GPIO.
 */
led_status_t LED_OFF(void)
{
    /* Drive the active-low LED pin high. */
    HAL_GPIO_WritePin(LED_C13_PORT, LED_C13_PIN, GPIO_PIN_SET);

    return LED_OK;
}

/**
 * @brief Toggle the output level to switch the status LED state.
 *
 * @return led_status_t : Operation status.
 * @retval LED_OK The GPIO toggle call ran; the actual pin level is not checked.
 *
 * @note Call LED_Init() before using this function.
 */
led_status_t LED_Toggle(void)
{
    /* Toggle the LED output level. */
    HAL_GPIO_TogglePin(LED_C13_PORT, LED_C13_PIN);

    return LED_OK;
}

/**************************** Function Definitions ****************************/
