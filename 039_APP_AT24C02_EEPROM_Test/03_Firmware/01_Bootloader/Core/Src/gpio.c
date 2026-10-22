/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file gpio.c
 *
 * @par dependencies
 * - gpio.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements the GPIO interfaces for the user key and status LED.
 *
 * Processing flow:
 *
 * Call key_init() and led_init() once before using key_scan(), LED_ON(),
 * or LED_OFF().
 *
 * @version V1.1 2026-09-21
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "gpio.h"
/****************************** Includes ******************************/

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
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_structure = {0};

    // 1. Enable the GPIOA peripheral clock.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 2. Configure PA0 as a pulled-up input for the active-low key.
    gpio_init_structure.GPIO_Pin   = KEY_GPIO_PIN;
    gpio_init_structure.GPIO_Mode  = GPIO_Mode_IN;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;
    gpio_init_structure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(KEY_GPIO_PORT, &gpio_init_structure);
}

/**
 * @brief Scans and debounces the user key.
 *
 * When the active-low key is detected, waits 10 ms and samples it again.
 * The key is reported as pressed only when both samples are low.
 *
 * @param None.
 *
 * @return uint8_t Debounced key state.
 * @retval KEY_PRESSED  The key remains pressed after debouncing.
 * @retval KEY_RELEASED The key is released.
 */
uint8_t key_scan(void)
{
    uint8_t key_state = KEY_RELEASED;

    // 1. Check whether the active-low key is pressed.
    if (Bit_RESET == GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN))
    {
        // 2. Debounce the key before taking the second sample.
        delay_ms(10U);

        // 3. Confirm that the key remains pressed after debouncing.
        if (Bit_RESET == GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN))
        {
            key_state = KEY_PRESSED;
        }
    }

    return key_state;
}

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
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_structure = {0};

    // 1. Enable the GPIOC peripheral clock.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 2. Configure PC13 as the push-pull LED output.
    gpio_init_structure.GPIO_Pin   = LED_C13_PIN;
    gpio_init_structure.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;
    gpio_init_structure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_C13_PORT, &gpio_init_structure);
}
