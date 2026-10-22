/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx_Handler.c
 *
 * @par dependencies
 * - w25qxx_Handler.h
 * - string.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Manages buffered writes and sequential reads in W25Q A/B regions.
 *
 * Processing flow:
 *
 * Initialize the device and reset region metadata. Restore a saved length
 * when required, append file bytes, flush sector data with readback checks,
 * and read only the recorded valid bytes from the selected region.
 *
 * @version V1.2 2026-10-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "w25qxx_Handler.h"
#include <string.h>
/******************************* Includes *******************************/

/************************** Private Variables **************************/
/* A/B 区分别保存读写缓冲区和索引。 */
static st_W25Q_Handler s_ast_W25Q_Handler[2];
/************************** Private Variables **************************/

/************************* Function Definitions *************************/

/**
 * @brief 重置指定分区的读写缓冲区与管理索引。
 *
 * 仅清空内存中的管理状态，不执行物理擦除。
 * 开始新文件前调用，后续写入从分区首地址开始。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 *
 * @return 无。
 */
void Erase_Flash_Block(u8 block_index)
{
    if (block_index > BLOCK_2)
    {
        return;
    }
    memset(&s_ast_W25Q_Handler[block_index], 0,
           sizeof(s_ast_W25Q_Handler[block_index]));
}

/**
 * @brief 根据已有文件长度恢复指定分区的写入管理参数。
 *
 * 设置已写入长度、缓冲区位置和扇区序号，不访问物理闪存。
 * 调用者须提供有效分区编号及区内长度；复位后用来恢复 A 区包长度。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 * @param[in] app_size 该分区已有文件的有效字节数。
 *
 * @return 无。
 */
void SetBlockParmeter(u8 block_index, uint32_t app_size)
{
    s_ast_W25Q_Handler[block_index].write_index = app_size;
    s_ast_W25Q_Handler[block_index].write_databuf_index = app_size % 4096;
    s_ast_W25Q_Handler[block_index].write_sector_index = app_size / 4096;
}

/**
 * @brief 初始化外部闪存并重置读写管理状态。
 *
 * 调用底层器件初始化接口，然后清空A/B 两个分区的管理状态。
 * 本接口不返回底层初始化状态。
 *
 * @return 无。
 */
void W25Q64_Init(void)
{
    W25Qx_Init();
    Erase_Flash_Block(BLOCK_1);
    Erase_Flash_Block(BLOCK_2);
}

/**
 * @brief 擦除整片外部闪存并重置管理状态。
 *
 * 底层擦除成功后清空管理状态；失败时直接返回。
 *
 * @return 整片擦除结果。
 * @retval 0 擦除完成。
 * @retval 1 底层擦除失败。
 */
u8 W25Q64_EraseChip(void)
{
    if (W25Qx_OK != W25Qx_Erase_Chip())
    {
        return 1U;
    }
    Erase_Flash_Block(BLOCK_1);
    Erase_Flash_Block(BLOCK_2);
    return 0U;
}

/**
 * @brief 刷新指定分区的当前缓冲数据并逐页回读比较。
 *
 * 处理步骤：
 * 1. 缓冲区为空时直接返回成功。
 * 2. 擦除当前写入位置所在的扇区。
 * 3. 按有效长度分页写入，并回读比较每页内容。
 * 4. 成功后累计已写入长度并清空缓冲区索引。
 * 失败时作废本次分区管理状态。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 *
 * @return 缓冲数据刷新结果。
 * @retval 0 刷新完成，或缓冲区为空。
 * @retval 1 扇区擦除、页写入或回读比较失败。
 */
static u8 W25Q64_WriteBuffer(u8 block_index)
{
    /* 当前分区的管理状态。 */
    st_W25Q_Handler *p_handler = &s_ast_W25Q_Handler[block_index];
    /* 本次外部闪存访问起始地址。 */
    u32 addr = block_index * BLOCK_SIZE + p_handler->write_index;
    u16 length = p_handler->write_databuf_index;  /* 当前缓冲区待刷新的有效字节数。 */
    u8 read_buffer[256];  /* 单页回读比较缓冲区。 */

    if (0U == length)
    {
        return 0U;
    }
    if (W25Qx_OK != W25Qx_Erase_Block(addr))
    {
        Erase_Flash_Block(block_index);
        return 1U;
    }

    for (u16 index = 0U; index < length; )
    {
        u16 page_size = length - index;  /* 本轮分页写入的有效字节数。 */
        if (page_size > W25Qx_Para.PAGE_SIZE)
        {
            page_size = (u16)W25Qx_Para.PAGE_SIZE;
        }
        if ((W25Qx_OK != W25Qx_Write(&p_handler->databuf[index],
                                    addr + index, page_size)) ||
            (W25Qx_OK != W25Qx_Read(read_buffer, addr + index, page_size)) ||
            (0 != memcmp(read_buffer, &p_handler->databuf[index], page_size)))
        {
            /* 底层 SPI 失败可能提前返回，显式释放片选以便后续重试。 */
            W25Qx_Disable();
            Erase_Flash_Block(block_index);
            return 1U;
        }
        index += page_size;
    }

    p_handler->write_index += length;
    p_handler->write_sector_index =
        (u8)(p_handler->write_index / W25Qx_Para.SUBSECTOR_SIZE);
    p_handler->write_databuf_index = 0U;
    return 0U;
}

/**
 * @brief 向指定分区追加文件数据，并刷新已填满的扇区。
 *
 * 检查容量和存储组织参数，再将数据追加到扇区缓冲区。
 * 每满 4096 字节刷新一次，并检查写入及回读结果。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 * @param[in] data 待写入的数据缓冲区；非零长度时须有效。
 * @param[in] length 本次追加的有效字节数。
 *
 * @return 数据追加结果。
 * @retval 0 处理成功。
 * @retval 1 参数、容量、擦除、写入或回读失败。
 *
 * @note 尾部刷新后若不足整扇区，开始新文件须先重置管理状态。
 */
u8 W25Q64_WriteData(u8 block_index, u8 *data, u32 length)
{
    st_W25Q_Handler *p_handler;  /* 当前分区的管理状态。 */

    if ((block_index > BLOCK_2) || ((NULL == data) && (0U != length)) ||
        (4096U != W25Qx_Para.SUBSECTOR_SIZE) ||
        (256U != W25Qx_Para.PAGE_SIZE))
    {
        return 1U;
    }
    p_handler = &s_ast_W25Q_Handler[block_index];
    if (((p_handler->write_index % W25Qx_Para.SUBSECTOR_SIZE) != 0U) ||
        (length > BLOCK_SIZE - p_handler->write_index -
                  p_handler->write_databuf_index))
    {
        return 1U;
    }

    for (u32 i = 0U; i < length; i++)
    {
        p_handler->databuf[p_handler->write_databuf_index++] = data[i];
        if (p_handler->write_databuf_index == W25Qx_Para.SUBSECTOR_SIZE)
        {
            if (0U != W25Q64_WriteBuffer(block_index))
            {
                return 1U;
            }
        }
    }
    return 0U;
}

/**
 * @brief 刷新指定分区最后不足一个扇区的数据。
 *
 * 检查存储组织参数，再按实际缓冲长度执行刷新。
 * 缓冲区为空时返回成功，重复调用不会重复累计长度。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 *
 * @return 尾部刷新结果。
 * @retval 0 刷新完成，或缓冲区为空。
 * @retval 1 参数、擦除、写入或回读比较失败。
 */
u8 W25Q64_WriteData_End(u8 block_index)
{
    if ((block_index > BLOCK_2) || (4096U != W25Qx_Para.SUBSECTOR_SIZE) ||
        (256U != W25Qx_Para.PAGE_SIZE))
    {
        return 1U;
    }
    return W25Q64_WriteBuffer(block_index);
}

/**
 * @brief 获取指定分区已完成物理写入的有效长度。
 *
 * 不包含尚未刷新的缓冲数据。
 * 分区编号无效时返回零。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 *
 * @return 已写入的有效字节数。
 */
uint32_t Read_BlockSize(uint8_t block_index)
{
    if (block_index > BLOCK_2)
    {
        return 0U;
    }
    return s_ast_W25Q_Handler[block_index].write_index;
}

/**
 * @brief 将指定分区的读取位置复位到区首。
 *
 * 只清零读取索引，不改变已写入长度或缓冲区状态。
 * 分区编号无效时直接返回。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 *
 * @return 无。
 */
void W25Q64_ReadData_Reset(u8 block_index)
{
    if (block_index <= BLOCK_2)
    {
        s_ast_W25Q_Handler[block_index].read_index = 0U;
        s_ast_W25Q_Handler[block_index].read_sector_index = 0U;
    }
}

/**
 * @brief 从指定分区的当前读取位置读取有效数据。
 *
 * 单次最多读取 4096 字节，尾部只返回剩余有效字节。
 * 有效长度输出地址先清零；仅在底层读取成功后推进索引。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 * @param[out] data 接收缓冲区，须至少容纳 4096 字节。
 * @param[out] length 本次实际读取字节数的输出地址。
 *
 * @return 顺序读取结果。
 * @retval 0 读取完成。
 * @retval 1 已无有效数据可读。
 * @retval 2 参数无效或底层读取失败。
 */
u8 W25Q64_ReadData(u8 block_index, u8 *data, u16 *length)
{
    st_W25Q_Handler *p_handler;  /* 当前分区的管理状态。 */
    u32 read_length;  /* 本次顺序读取的有效字节数。 */
    u32 addr;  /* 本次外部闪存访问起始地址。 */

    if (NULL == length)
    {
        return 2U;
    }
    *length = 0U;
    if ((block_index > BLOCK_2) || (NULL == data) ||
        (4096U != W25Qx_Para.SUBSECTOR_SIZE))
    {
        return 2U;
    }
    p_handler = &s_ast_W25Q_Handler[block_index];
    if (p_handler->read_index >= p_handler->write_index)
    {
        return 1U;
    }
    read_length = p_handler->write_index - p_handler->read_index;
    if (read_length > W25Qx_Para.SUBSECTOR_SIZE)
    {
        read_length = W25Qx_Para.SUBSECTOR_SIZE;
    }
    addr = block_index * BLOCK_SIZE + p_handler->read_index;
    if (W25Qx_OK != W25Qx_Read(data, addr, read_length))
    {
        W25Qx_Disable();
        return 2U;
    }
    p_handler->read_index += read_length;
    p_handler->read_sector_index =
        (u8)(p_handler->read_index / W25Qx_Para.SUBSECTOR_SIZE);
    *length = (u16)read_length;
    return 0U;
}
/************************* Function Definitions *************************/
