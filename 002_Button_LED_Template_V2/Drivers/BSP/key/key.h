#ifndef __KEY_H__
#define __KEY_H__

#include "sys.h"

#define KEY_GPIO_PORT              GPIOA
#define KEY_GPIO_PIN               GPIO_PIN_0
#define KEY_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

void key_init(void);
uint8_t key_is_pressed(void);

#endif
