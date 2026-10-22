#include "delay.h"

#define DELAY_HCLK_MHZ  16U

/**
  * @brief  微秒延时 (基于16MHz HCLK)
  */
void delay_us(uint32_t nus)
{
    uint32_t temp;

    if (nus == 0U)
    {
        return;
    }

    /* 16MHz HCLK 下，1us 需要计数16次 */
    SysTick->LOAD = DELAY_HCLK_MHZ * nus - 1U;
    SysTick->VAL = 0x00;           /* 清空当前值 */
    SysTick->CTRL |= 1 << 2;       /* 选择内核时钟源 (HCLK) */
    SysTick->CTRL |= 1 << 0;       /* 开启定时器 */

    /* 等待计数完成 (位16为0时表示未溢出) */
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~(1 << 0);    /* 关闭定时器 */
}

/**
  * @brief  毫秒延时
  */
void delay_ms(uint32_t nms)
{
    while(nms--)
    {
        delay_us(1000);
    }
}

/**
  * @brief  秒延时
  */
void delay_s(uint32_t ns)
{
    while(ns--)
    {
        delay_ms(1000);
    }
}

/**
  * @brief  重写HAL库的毫秒延时
  * @note   HAL库默认使用Systick中断，这里改为查询方式，兼容性更好
  */
void HAL_Delay(uint32_t nms)
{
    delay_ms(nms);
}
