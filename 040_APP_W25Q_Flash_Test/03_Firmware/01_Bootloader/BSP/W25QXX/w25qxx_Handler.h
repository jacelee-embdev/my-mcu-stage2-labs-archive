/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx_Handler.h
 *
 * @par dependencies
 * - stm32f4xx.h
 * - main.h
 * - w25qxx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the W25Q64 buffered read and write interface.
 *
 * Processing flow:
 *
 * Call W25Q64_Init() before writing or reading. Pass data to
 * W25Q64_WriteData(), flush remaining data with W25Q64_WriteData_End(),
 * and retrieve data with W25Q64_ReadData().
 *
 * @version V1.0 2026-09-30
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __W25Q_HANDLER_H__
#define __W25Q_HANDLER_H__

/****************************** Includes ******************************/
#include "stm32f4xx.h"
#include "main.h"
#include "w25qxx.h"
/****************************** Includes ******************************/

/****************************** Type Definitions ******************************/
/* 页大小为 256 字节；每扇区 16 页，共 4096 字节；每块 16 个扇区。 */
typedef struct
{
    u8 databuf[4096];             /* 暂存一个扇区的数据。 */
    u16 write_databuf_index;      /* 缓冲区下一写入位置。 */
    u32 write_index;              /* 已写入闪存的字节数。 */
    u8 write_sector_index;        /* 下一写入扇区序号。 */

    u32 read_index;               /* 已读取的字节数。 */
    u8 read_sector_index;         /* 下一读取扇区序号。 */
} st_W25Q_Handler;
/****************************** Type Definitions ******************************/

/****************************** Defines ******************************/
#define EXTERN_Flash
/****************************** Defines ******************************/

/****************************** Function Declarations ************************/
/**
 * @brief 初始化 W25Q64，并清零读写索引。
 *
 * @return 无。
 */
void W25Q64_Init(void);

/**
 * @brief 擦除整片闪存；成功后清零读写索引。
 *
 * @return 擦除结果。
 * @retval 0 擦除成功。
 * @retval 1 擦除失败。
 */
u8 W25Q64_EraseChip(void);

/**
 * @brief 将数据放入缓冲区，并按扇区写入已填满的数据。
 *
 * @param[in] data 待写入数据的首地址。
 * @param[in] length 待写入的字节数。
 *
 * @return 0。
 */
u8 W25Q64_WriteData(u8 *data, u32 length);

/**
 * @brief 将缓冲区中的剩余数据写入当前扇区。
 *
 * @return 0。
 */
u8 W25Q64_WriteData_End(void);

/**
 * @brief 读取下一段已写入的数据，单次最多读取一个扇区。
 *
 * @param[out] data 接收数据的缓冲区，需容纳一个扇区。
 * @param[out] length 实际读取的字节数。
 *
 * @return 读取结果。
 * @retval 0 读取成功。
 * @retval 1 已无可读数据。
 * @retval 2 底层读取失败。
 */
u8 W25Q64_ReadData(u8 *data, u16 *length);
/****************************** Function Declarations ************************/

#endif /* __W25Q_HANDLER_H */
