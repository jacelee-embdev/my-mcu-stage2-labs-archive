/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file spi.h
 *
 * @par dependencies
 * - stm32f4xx.h
 * - stm32f4xx_it.h
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares SPI1 initialization and polling transfer APIs.
 *
 * Processing flow:
 *
 * Call SPI1_Init() before using the byte transfer APIs.
 *
 * @version V1.0 2026-09-29
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SPI_H
#define __SPI_H

/****************************** Includes ******************************/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define F_CS_Pin        GPIO_Pin_4
#define F_CS_GPIO_Port  GPIOA
/****************************** Defines ******************************/

/************************ Function Declarations ************************/
/**
 * @brief 初始化 SPI1 和片选引脚。
 *
 * 配置 SPI1 所需引脚与外设，并发送一个启动字节。
 *
 * @param 无。
 *
 * @return 无。
 */
void SPI1_Init(void);

/**
 * @brief 以轮询方式发送指定数量的字节。
 *
 * 每发送一个字节后读取并丢弃对应的接收数据，直到传输完成或超时。
 *
 * @param[in] WriteData 指向待发送数据缓冲区。
 * @param[in] dataSize  待发送的字节数。
 * @param[in] timeout   超时时间，单位为毫秒。
 *
 * @return 传输结果。
 * @retval 1 传输完成。
 * @retval 0 传输超时。
 */
u8 SPI1_WriteByte(u8 *WriteData, u16 dataSize, u32 timeout);

/**
 * @brief 以轮询方式读取指定数量的字节。
 *
 * 使用缓冲区中的数据产生发送时钟，并将接收数据写回同一缓冲区。
 *
 * @param[in,out] ReadData 收发共用的数据缓冲区。
 * @param[in] dataSize   待读取的字节数。
 * @param[in] timeout    超时时间，单位为毫秒。
 *
 * @return 传输结果。
 * @retval 1 传输完成。
 * @retval 0 传输超时。
 */
u8 SPI1_ReadByte(u8 *ReadData, u16 dataSize, u32 timeout);
/************************ Function Declarations ************************/

#endif /* End of __SPI_H */
