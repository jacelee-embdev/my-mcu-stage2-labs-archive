#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

#define LED_GPIO_PORT              GPIOC
#define LED_GPIO_PIN               GPIO_PIN_13
#define LED_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

void led_init(void);
void led_toggle(void);

#endif
