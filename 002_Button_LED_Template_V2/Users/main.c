#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    led_init();
    key_init();

    while (1)
    {
        if (key_is_pressed())
        {
            led_toggle();
        }

        delay_ms(1000);
    }
}
