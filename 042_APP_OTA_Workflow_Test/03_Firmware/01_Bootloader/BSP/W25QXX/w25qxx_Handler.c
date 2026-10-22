/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx_Handler.c
 *
 * @par dependencies
 * - w25qxx_Handler.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements buffered W25Q64 sector writes and sequential reads.
 *
 * Processing flow:
 *
 * Initialize the device, append data to the write buffer, flush any
 * remaining data, and read back the written data as needed.
 *
 * @version V1.0 2026-09-30
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "w25qxx_Handler.h"
/****************************** Includes ******************************/

/***************************** Private Variables *****************************/
/* 保存读写缓冲区及其索引。 */
static st_W25Q_Handler s_st_W25Q_Handler_1;
/***************************** Private Variables *****************************/

/****************************** Function Definitions ************************/

/**
 * @brief 初始化闪存，并将读写索引归零。
 *
 * @return 无。
 */
void W25Q64_Init(void)
{
    W25Qx_Init();
    s_st_W25Q_Handler_1.read_index = 0;
    s_st_W25Q_Handler_1.read_sector_index = 0;
    s_st_W25Q_Handler_1.write_databuf_index = 0;
    s_st_W25Q_Handler_1.write_index = 0;
    s_st_W25Q_Handler_1.write_sector_index = 0;
}

/**
 * @brief 擦除整片闪存，并在成功后清零读写索引。
 *
 * @return 擦除结果。
 * @retval 0 擦除成功。
 * @retval 1 擦除失败。
 */
u8 W25Q64_EraseChip(void)
{
    if (0 == W25Qx_Erase_Chip())
    {
        s_st_W25Q_Handler_1.read_index = 0;
        s_st_W25Q_Handler_1.read_sector_index = 0;
        s_st_W25Q_Handler_1.write_databuf_index = 0;
        s_st_W25Q_Handler_1.write_index = 0;
        s_st_W25Q_Handler_1.write_sector_index = 0;
        return 0;
    }
    return 1;
}

/**
 * @brief 将数据暂存到缓冲区，每填满一个扇区即写入闪存。
 *
 * @param[in] data 待写入数据的首地址。
 * @param[in] length 待写入的字节数。
 *
 * @return 0。
 */
u8 W25Q64_WriteData(u8 *data, u32 length)
{
    u32 addr = 0;
    u16 index = 0;

    for (u16 i = 0; i < length; i++)
    {
        // 1. 将当前字节放入扇区缓冲区。
        index = s_st_W25Q_Handler_1.write_databuf_index;
        s_st_W25Q_Handler_1.databuf[index] = *(data + i);
        s_st_W25Q_Handler_1.write_databuf_index++;

        // 2. 缓冲区填满后，擦除目标区域并逐页写入。
        if (s_st_W25Q_Handler_1.write_databuf_index ==
            W25Qx_Para.SUBSECTOR_SIZE)
        {
            s_st_W25Q_Handler_1.write_databuf_index = 0;
            addr = W25Qx_Para.SUBSECTOR_SIZE *
                   s_st_W25Q_Handler_1.write_sector_index;
            W25Qx_Erase_Block(addr);
            W25Qx_WriteEnable();

            for (u8 j = 0; j < 16; j++)
            {
                // 2.1 计算当前页地址并写入一页数据。
                addr = (W25Qx_Para.SUBSECTOR_SIZE *
                        s_st_W25Q_Handler_1.write_sector_index) + \
                       (W25Qx_Para.PAGE_SIZE * j);
                index = W25Qx_Para.PAGE_SIZE * j;
                W25Qx_Write(&s_st_W25Q_Handler_1.databuf[index],
                            addr, W25Qx_Para.PAGE_SIZE);
            }

            s_st_W25Q_Handler_1.write_sector_index++;
            s_st_W25Q_Handler_1.write_index += W25Qx_Para.SUBSECTOR_SIZE;
        }
    }
    return 0;
}

/**
 * @brief 将缓冲区内尚未凑满扇区的数据写入闪存。
 *
 * @return 0。
 */
u8 W25Q64_WriteData_End(void)
{
    u32 addr = 0;
    u16 index = 0;
    u8 page_size = 0;

    // 1. 仅在缓冲区中仍有数据时执行写入。
    if (0 != s_st_W25Q_Handler_1.write_databuf_index)
    {
        page_size = s_st_W25Q_Handler_1.write_databuf_index /
                    W25Qx_Para.PAGE_SIZE;
        addr = W25Qx_Para.SUBSECTOR_SIZE *
               s_st_W25Q_Handler_1.write_sector_index;
        W25Qx_Erase_Block(addr);
        W25Qx_WriteEnable();

        // 2. 先写入所有完整页。
        for (u8 j = 0; j < page_size; j++)
        {
            addr = (W25Qx_Para.SUBSECTOR_SIZE *
                    s_st_W25Q_Handler_1.write_sector_index) + \
                   (W25Qx_Para.PAGE_SIZE * j);
            index = W25Qx_Para.PAGE_SIZE * j;
            W25Qx_Write(&s_st_W25Q_Handler_1.databuf[index],
                        addr, W25Qx_Para.PAGE_SIZE);
        }

        // 3. 剩余字节不足一页时，按实际长度写入。
        if (0 != (s_st_W25Q_Handler_1.write_databuf_index %
                  W25Qx_Para.PAGE_SIZE))
        {
            addr = (W25Qx_Para.SUBSECTOR_SIZE *
                    s_st_W25Q_Handler_1.write_sector_index) + \
                   (W25Qx_Para.PAGE_SIZE * page_size);
            index = W25Qx_Para.PAGE_SIZE * page_size;
            W25Qx_Write(&s_st_W25Q_Handler_1.databuf[index], addr, \
                        s_st_W25Q_Handler_1.write_databuf_index %
                        W25Qx_Para.PAGE_SIZE);
        }
        s_st_W25Q_Handler_1.write_index +=
            s_st_W25Q_Handler_1.write_databuf_index;
    }
    return 0;
}

/**
 * @brief 从下一待读位置读取数据，每次最多读取一个扇区。
 *
 * @param[out] data 接收数据的缓冲区，需容纳一个扇区。
 * @param[out] length 实际读取的字节数。
 *
 * @return 读取结果。
 * @retval 0 读取成功。
 * @retval 1 已无可读数据。
 * @retval 2 底层读取失败。
 */
u8 W25Q64_ReadData(u8 *data, u16 *length)
{
    u32 addr = 0;

    // 1. 比较已写入长度与已读取长度。
    if (s_st_W25Q_Handler_1.write_index >
        s_st_W25Q_Handler_1.read_index)
    {
        // 2. 已有完整扇区时读取整个扇区。
        if (s_st_W25Q_Handler_1.write_sector_index >
            s_st_W25Q_Handler_1.read_sector_index)
        {
            *length = W25Qx_Para.SUBSECTOR_SIZE;
            addr = s_st_W25Q_Handler_1.read_sector_index *
                   W25Qx_Para.SUBSECTOR_SIZE;
            if (0 != W25Qx_Read(data, addr, *length))
                return 2;
            s_st_W25Q_Handler_1.read_sector_index++;
        }
        else
        {
            // 3. 不足一个扇区时按剩余长度读取。
            *length = s_st_W25Q_Handler_1.write_index -
                      s_st_W25Q_Handler_1.read_index;
            addr = s_st_W25Q_Handler_1.read_sector_index *
                   W25Qx_Para.SUBSECTOR_SIZE;
            if (0 != W25Qx_Read(data, addr, *length))
                return 2;
        }
        s_st_W25Q_Handler_1.read_index += *length;
        return 0;
    }
    else
    {
        return 1;
    }
}
/****************************** Function Definitions ************************/
