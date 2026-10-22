/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx_Handler.c
 *
 * @par dependencies
 * - w25qxx_Handler.h
 * - sfud_adapter.h
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
 * @par provenance
 * Adapted with reference to the course driver portability example.
 * SFUD library code retains its original upstream license.
 *
 * @version V1.3 2026-10-09
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "w25qxx_Handler.h"
#include "sfud_adapter.h"
#include <string.h>
/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define W25Q_BUFFER_SIZE (4096U)
/******************************* Defines *******************************/

/************************** Private Variables **************************/
/* A/B 区分别保存读写缓冲区和索引。 */
static st_W25Q_Handler s_ast_W25Q_Handler[2];
/* 仅保存初始化成功且符合当前 A/B 分区布局的 SFUD 设备。 */
static const sfud_flash *s_p_flash;
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
 * 恢复已写入长度和扇区序号，清空待写缓冲并复位读取位置。
 * 不访问物理闪存；复位后用来恢复 A 区包长度，不恢复追加写入。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 * @param[in] app_size 该分区已有文件的有效字节数。
 *
 * @return 无。
 */
void SetBlockParmeter(u8 block_index, uint32_t app_size)
{
    if ((block_index > BLOCK_2) || (app_size > BLOCK_SIZE))
    {
        return;
    }
    Erase_Flash_Block(block_index);
    s_ast_W25Q_Handler[block_index].write_index = app_size;
    s_ast_W25Q_Handler[block_index].write_sector_index =
        (u8)(app_size / W25Q_BUFFER_SIZE);
}

/**
 * @brief 初始化外部闪存并重置读写管理状态。
 *
 * 调用 SFUD 适配层初始化，然后清空 A/B 两个分区的管理状态。
 * 仅接受容量至少 128 KiB、擦除粒度为 4096 字节的设备。
 * 本接口不返回初始化状态；设备不可用时后续读写返回错误。
 *
 * @return 无。
 */
void W25Q64_Init(void)
{
    const sfud_flash *p_flash;  /* SFUD 识别得到的设备。 */

    s_p_flash = NULL;
    SFUD_W25Q64_Init();
    p_flash = sfud_get_device(SFUD_W25Q64_DEVICE_INDEX);
    if ((NULL != p_flash) && p_flash->init_ok &&
        (p_flash->chip.capacity >= (2U * BLOCK_SIZE)) &&
        (W25Q_BUFFER_SIZE == p_flash->chip.erase_gran))
    {
        s_p_flash = p_flash;
    }
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
    if ((NULL == s_p_flash) ||
        (SFUD_SUCCESS != sfud_chip_erase(s_p_flash)))
    {
        return 1U;
    }
    Erase_Flash_Block(BLOCK_1);
    Erase_Flash_Block(BLOCK_2);
    return 0U;
}

/**
 * @brief 使用 SFUD 刷新指定分区的缓冲数据并分段回读比较。
 *
 * 处理步骤：
 * 1. 缓冲区为空时直接返回成功。
 * 2. 由 SFUD 擦除当前扇区并按有效长度写入。
 * 3. 使用小缓冲区分段回读比较。
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
    /* 当前缓冲区待刷新的有效字节数。 */
    u16 length = p_handler->write_databuf_index;
    u8 read_buffer[256];  /* 分段回读比较缓冲区，限制栈占用。 */

    if (0U == length)
    {
        return 0U;
    }
    if ((p_handler->write_index > BLOCK_SIZE) ||
        (length > W25Q_BUFFER_SIZE) ||
        ((p_handler->write_index % W25Q_BUFFER_SIZE) != 0U) ||
        (length > BLOCK_SIZE - p_handler->write_index))
    {
        return 1U;
    }
    if (SFUD_SUCCESS != sfud_erase_write(s_p_flash, addr, length,
                                       p_handler->databuf))
    {
        Erase_Flash_Block(block_index);
        return 1U;
    }

    for (u16 index = 0U; index < length; )
    {
        /* 本轮回读比较的有效字节数。 */
        u16 read_size = length - index;
        if (read_size > sizeof(read_buffer))
        {
            read_size = (u16)sizeof(read_buffer);
        }
        if ((SFUD_SUCCESS != sfud_read(s_p_flash, addr + index,
                                      read_size, read_buffer)) ||
            (0 != memcmp(read_buffer, &p_handler->databuf[index], read_size)))
        {
            Erase_Flash_Block(block_index);
            return 1U;
        }
        index += read_size;
    }

    p_handler->write_index += length;
    p_handler->write_sector_index =
        (u8)(p_handler->write_index / W25Q_BUFFER_SIZE);
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
        (NULL == s_p_flash))
    {
        return 1U;
    }
    p_handler = &s_ast_W25Q_Handler[block_index];
    if ((p_handler->write_index > BLOCK_SIZE) ||
        (p_handler->write_databuf_index >= W25Q_BUFFER_SIZE) ||
        ((p_handler->write_index % W25Q_BUFFER_SIZE) != 0U) ||
        (p_handler->write_databuf_index >
         BLOCK_SIZE - p_handler->write_index) ||
        (length > BLOCK_SIZE - p_handler->write_index -
                  p_handler->write_databuf_index))
    {
        return 1U;
    }

    for (u32 i = 0U; i < length; i++)
    {
        p_handler->databuf[p_handler->write_databuf_index++] = data[i];
        if (p_handler->write_databuf_index == W25Q_BUFFER_SIZE)
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
    if ((block_index > BLOCK_2) || (NULL == s_p_flash))
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
    if ((block_index > BLOCK_2) || (NULL == data) || (NULL == s_p_flash))
    {
        return 2U;
    }
    p_handler = &s_ast_W25Q_Handler[block_index];
    if (p_handler->read_index >= p_handler->write_index)
    {
        return 1U;
    }
    read_length = p_handler->write_index - p_handler->read_index;
    if (read_length > W25Q_BUFFER_SIZE)
    {
        read_length = W25Q_BUFFER_SIZE;
    }
    addr = block_index * BLOCK_SIZE + p_handler->read_index;
    if (SFUD_SUCCESS != sfud_read(s_p_flash, addr, read_length, data))
    {
        return 2U;
    }
    p_handler->read_index += read_length;
    p_handler->read_sector_index =
        (u8)(p_handler->read_index / W25Q_BUFFER_SIZE);
    *length = (u16)read_length;
    return 0U;
}
/************************* Function Definitions *************************/
