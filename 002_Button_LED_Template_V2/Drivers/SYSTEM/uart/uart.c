#include "uart.h"
#include "string.h"

/* 串口句柄 */
UART_HandleTypeDef uart1_handle;

/* 接收缓冲区 */
uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
uint16_t uart1_rx_length = 0;

/**
 * @brief       重定义fputc (供printf使用)
 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);       /* 等待发送完成 */
    USART1->DR = (uint8_t)ch;               /* 发送数据 */
    return ch;
}

/**
 * @brief       串口初始化
 */
void uart1_init(uint32_t baudrate)
{
    uart1_handle.Instance = USART1;                        /* 直接指定 USART1 */
    uart1_handle.Init.BaudRate = baudrate;
    uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;
    uart1_handle.Init.StopBits = UART_STOPBITS_1;
    uart1_handle.Init.Parity = UART_PARITY_NONE;
    uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1_handle.Init.Mode = UART_MODE_TX_RX;
    
    HAL_UART_Init(&uart1_handle);
}

/**
 * @brief       HAL库底层初始化回调
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    if (huart->Instance == USART1)
    {
        /* 1. 使能时钟 */
        USART1_TX_CLK_ENABLE();                 /* 使能TX GPIO时钟 (宏定义) */
        USART1_RX_CLK_ENABLE();                 /* 使能RX GPIO时钟 (宏定义) */
        __HAL_RCC_USART1_CLK_ENABLE();          /* 使能USART1时钟 */

        /* 2. 配置 USART1 TX/RX 引脚 */
        gpio_init_struct.Pin = USART1_TX_PIN | USART1_RX_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(USART1_TX_PORT, &gpio_init_struct);
        
        /* 3. 配置中断 */
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);

        /* 4. 开启接收和空闲中断 */
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    }
}

/**
 * @brief       清空缓冲区
 */
void uart1_rx_clear(void)
{
    memset(uart1_rx_buffer, 0, sizeof(uart1_rx_buffer));
    uart1_rx_length = 0;
}

/**
 * @brief       串口中断服务函数
 */
void USART1_IRQHandler(void)
{
    uint8_t receive_data = 0;   
    
    /* 接收数据中断 */
    if(__HAL_UART_GET_FLAG(&uart1_handle, UART_FLAG_RXNE) != RESET){
        if(uart1_rx_length >= sizeof(uart1_rx_buffer))
            uart1_rx_length = 0;
            
        HAL_UART_Receive(&uart1_handle, &receive_data, 1, 1000);
        uart1_rx_buffer[uart1_rx_length++] = receive_data;
    }

    /* 空闲中断 (一帧数据接收完毕) */
    if (__HAL_UART_GET_FLAG(&uart1_handle, UART_FLAG_IDLE) != RESET)
    {
        printf("recv: %s\r\n", uart1_rx_buffer);
        uart1_rx_clear();
        __HAL_UART_CLEAR_IDLEFLAG(&uart1_handle);
    }
}
