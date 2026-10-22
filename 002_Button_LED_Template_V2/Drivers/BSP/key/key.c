#include "key.h"

void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    KEY_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = KEY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_PORT, &gpio_init_struct);
}

uint8_t key_is_pressed(void)
{
    return (HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN) == GPIO_PIN_RESET);
}
