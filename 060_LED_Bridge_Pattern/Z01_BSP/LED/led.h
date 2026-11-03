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
 * Then call LED_ON(), LED_OFF(), or LED_Toggle() to control the LED.
 *
 * @version v1.0 2026-10-28
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __LED_H__
#define __LED_H__

/********************************** Includes **********************************/

#include "stm32f4xx_hal.h"        /* MCU SDK / HAL                            */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define LED_C13_PORT (GPIOC)        /* LED GPIO port.                         */
#define LED_C13_PIN  (GPIO_PIN_13)  /* PC13 LED pin.                          */

#ifndef LED_STATUS_T_DEFINED
#define LED_STATUS_T_DEFINED
typedef enum
{
    LED_OK              =    0,   /* Operation completed successfully.        */
    LED_ERROR_TIMEOUT   =   -1,   /* Operation timed out.                     */
    LED_ERROR_RESOURCE  =   -2,   /* Resource unavailable.                    */
    LED_ERROR_PARAMETER =   -3,   /* Invalid parameter.                       */
    LED_ERROR_ISR       =   -4,   /* Invalid interrupt context.               */
    LED_ERROR           =   -5,   /* General error; reserved.                 */
    LED_ERROR_NOMEMORY  =   -6,   /* Insufficient memory; reserved.           */
    LED_RESERVED        = 0xFF,   /* Reserved status.                         */
} led_status_t;  /* Shared LED status; permits either header include order */
#endif  /* LED_STATUS_T_DEFINED */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

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
void LED_Init(void);

/**
 * @brief Drive the output low to turn on the status LED.
 *
 * @return led_status_t : Operation status.
 * @retval LED_OK The GPIO write call ran; the actual pin level is not checked.
 *
 * @note Call LED_Init() before using this function.
 */
led_status_t LED_ON(void);

/**
 * @brief Drive the output high to turn off the status LED.
 *
 * @return led_status_t : Operation status.
 * @retval LED_OK The GPIO write call ran; the actual pin level is not checked.
 *
 * @note Call after LED_Init() has configured the GPIO.
 */
led_status_t LED_OFF(void);

/**
 * @brief Toggle the output level to switch the status LED state.
 *
 * @return led_status_t : Operation status.
 * @retval LED_OK The GPIO toggle call ran; the actual pin level is not checked.
 *
 * @note Call LED_Init() before using this function.
 */
led_status_t LED_Toggle(void);

/********************************* Declaring **********************************/

#endif  /* __LED_H__ */
