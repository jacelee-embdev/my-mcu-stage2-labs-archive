#ifndef __DELAY_H__
#define __DELAY_H__

#include "sys.h"

/* 函数声明 */
void delay_us(uint32_t nus);      /* 微秒级延时 */
void delay_ms(uint32_t nms);      /* 毫秒级延时 */
void delay_s(uint32_t ns);        /* 秒级延时 */
void HAL_Delay(uint32_t nms);     /* 重写HAL库延时函数 */

#endif
