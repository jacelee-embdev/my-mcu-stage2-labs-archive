#ifndef __UART_H__
#define __UART_H__

#include "sys.h"
#include "stdio.h"

/* ================================================================ */
/* GPIO 硬件配置宏定义                  */
/* ================================================================ */

/* 发送引脚 (TX) 配置 */
#define USART1_TX_PORT              GPIOA                           /* TX 端口 */
#define USART1_TX_PIN               GPIO_PIN_9                      /* TX 引脚 */
#define USART1_TX_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()    /* TX 端口时钟 */

/* 接收引脚 (RX) 配置 */
#define USART1_RX_PORT              GPIOA                           /* RX 端口 */
#define USART1_RX_PIN               GPIO_PIN_10                     /* RX 引脚 */
#define USART1_RX_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()    /* RX 端口时钟 */

/* ================================================================ */

/* 接收缓冲区大小 */
#define UART1_RX_BUFFER_SIZE        128U


/* 函数声明 */
void uart1_init(uint32_t baudrate);
void uart1_rx_clear(void);

#endif
