/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file spi.c
 *
 * @par dependencies
 * - spi.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements SPI1 initialization and polling byte transfers.
 *
 * Processing flow:
 *
 * Call SPI1_Init() before transferring data with the SPI1 byte APIs.
 *
 * @version V1.0 2026-09-29
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "spi.h"
/****************************** Includes ******************************/

/************************ Function Declarations ************************/
u8 SPI1_ReadWriteByte(u8 WriteData);
/************************ Function Declarations ************************/

/************************ Function Definitions ************************/
/**
 * @brief 配置 SPI1 片选引脚。
 *
 * 将 PA4 配置为上拉的推挽输出。
 *
 * @param 无。
 *
 * @return 无。
 */
static void CS_IO_Init(void)
{
    // 片选引脚的 GPIO 配置参数。
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief 初始化 SPI1 和片选引脚。
 *
 * 配置片选及 SPI1 引脚，设置主机全双工模式并启用 SPI1。
 *
 * @param 无。
 *
 * @return 无。
 */
void SPI1_Init(void)
{
    // 1. 配置片选引脚并输出高电平。
    CS_IO_Init();
    GPIO_WriteBit(F_CS_GPIO_Port, F_CS_Pin, Bit_SET);

    // 2. 使能 SPI1 和 GPIOA 时钟。
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // SPI1 复用引脚的 GPIO 配置参数。
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 将 PA5、PA6、PA7 连接到 SPI1 复用功能。
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    // 4. 复位并释放 SPI1。
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

    // SPI1 主机模式的配置参数。
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 5. 启用 SPI1 并发送一个启动字节。
    SPI_Cmd(SPI1, ENABLE);

    SPI1_ReadWriteByte(0xff);
}

/**
 * @brief 设置 SPI1 的波特率预分频值。
 *
 * 清除 CR1 寄存器中的预分频字段，再写入指定的预分频值。
 * SPI 时钟频率由 APB2 时钟频率除以预分频系数得到。
 *
 * @param[in] SPI_BaudRate_Prescaler 标准外设库定义的预分频值。
 *
 * @return 无。
 */
void SPI1_SetSpeed(u8 SPI_BaudRate_Prescaler)
{
    // 1. 检查预分频参数。
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRate_Prescaler));

    // 2. 清除原预分频字段，并写入新的预分频值。
    SPI1->CR1 &= 0xFFC7;
    SPI1->CR1 |= SPI_BaudRate_Prescaler;

    // 3. 启用 SPI1。
    SPI_Cmd(SPI1, ENABLE);
}

/**
 * @brief 通过 SPI1 同步发送并接收一个字节。
 *
 * 等待发送缓冲区可写后发送数据，再等待接收缓冲区有数据。
 *
 * @param[in] WriteData 待发送的字节。
 *
 * @return 接收到的字节。
 */
u8 SPI1_ReadWriteByte(u8 WriteData)
{
    // 1. 等待发送缓冲区可写，然后发送一个字节。
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, WriteData);

    // 2. 等待接收缓冲区有数据，并返回接收到的字节。
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

/**
 * @brief 以轮询方式发送指定数量的字节。
 *
 * 逐字节发送并丢弃同步接收的数据，传输期间检查毫秒超时。
 *
 * @param[in] WriteData 指向待发送数据缓冲区。
 * @param[in] dataSize  待发送的字节数。
 * @param[in] timeout   超时时间，单位为毫秒。
 *
 * @return 传输结果。
 * @retval 1 传输完成。
 * @retval 0 传输超时。
 */
u8 SPI1_WriteByte(u8 *WriteData, u16 dataSize, u32 timeout)
{
    // 超时阈值、起始时刻及收发剩余字节数。
    u32 time = timeout;
    u32 current_time = SysTickUpdateTime;
    u16 txsize = dataSize;
    u16 rxsize = dataSize;

    // 发送缓冲区指针及当前收发状态。
    u8 *pTxBuffPtr = WriteData;
    u8 txflow = 1u;

    // 1. 轮询完成全部收发字节。
    while ((txsize > 0) || (rxsize > 0))
    {
        // 1.1 发送缓冲区可写时，发送下一个字节。
        if ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET) &&
            (txsize > 0) && (txflow == 1))
        {
            SPI_I2S_SendData(SPI1, *pTxBuffPtr);
            pTxBuffPtr += sizeof(u8);
            txsize--;
            txflow = 0;
        }

        // 1.2 接收缓冲区有数据时，读取并丢弃该字节。
        if ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET) &&
            (rxsize > 0) && (txflow == 0))
        {
            SPI_I2S_ReceiveData(SPI1);
            rxsize--;
            txflow = 1;
        }

        // 1.3 达到超时时间时，结束本次传输。
        if ((SysTickUpdateTime - current_time) >= time)
        {
            SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_OVR);
            return 0;
        }
    }

    // 2. 完成传输后清理状态标志。
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_OVR);
    return 1;
}

/**
 * @brief 以轮询方式读取指定数量的字节。
 *
 * 使用同一缓冲区提供发送数据并保存接收数据，传输期间检查毫秒超时。
 *
 * @param[in,out] ReadData 收发共用的数据缓冲区。
 * @param[in] dataSize   待读取的字节数。
 * @param[in] timeout    超时时间，单位为毫秒。
 *
 * @return 传输结果。
 * @retval 1 传输完成。
 * @retval 0 传输超时。
 */
u8 SPI1_ReadByte(u8 *ReadData, u16 dataSize, u32 timeout)
{
    // 超时阈值、起始时刻及收发剩余字节数。
    u32 time = timeout;
    u32 current_time = SysTickUpdateTime;
    u16 txsize = dataSize;
    u16 rxsize = dataSize;

    // 收发共用缓冲区的读写位置及当前收发状态。
    u8 *pTxBuffPtr = ReadData;
    u8 *pRxBuffPtr = ReadData;
    u8 txflow = 1u;

    // 1. 轮询完成全部收发字节。
    while ((txsize > 0) || (rxsize > 0))
    {
        // 1.1 发送缓冲区可写时，发送下一个字节。
        if ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET) &&
            (txsize > 0) && (txflow == 1))
        {
            SPI_I2S_SendData(SPI1, *pTxBuffPtr);
            pTxBuffPtr += sizeof(u8);
            txsize--;
            txflow = 0;
        }

        // 1.2 接收缓冲区有数据时，将其写入缓冲区。
        if ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET) &&
            (rxsize > 0) && (txflow == 0))
        {
            *pRxBuffPtr = SPI_I2S_ReceiveData(SPI1);
            pRxBuffPtr += sizeof(u8);
            rxsize--;
            txflow = 1;
        }

        // 1.3 达到超时时间时，结束本次传输。
        if ((SysTickUpdateTime - current_time) >= time)
        {
            return 0;
        }
    }

    return 1;
}
/************************ Function Definitions ************************/
