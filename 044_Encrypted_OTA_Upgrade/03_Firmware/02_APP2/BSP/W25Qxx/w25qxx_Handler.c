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
 * @brief Manages buffered downloads and sequential reads in W25Q A.
 *
 * Processing flow:
 *
 * Reset metadata before a new download. Append only valid file bytes,
 * erase and program sectors with page readback checks, flush the tail,
 * and expose the physically written length for completion checks.
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
/* 保存读写缓冲区及其索引。 */
static st_W25Q_Handler s_st_W25Q_Handler_1;
/************************** Private Variables **************************/

/************************* Function Definitions *************************/

/**
 * @brief 重置 A 区的读写缓冲区与管理索引。
 *
 * 仅清空内存中的管理状态，不执行物理擦除。
 * 开始新文件前调用，后续写入从分区首地址开始。
 *
 * @return 无。
 */
void W25Q64_ResetDownload(void)
{
    memset(&s_st_W25Q_Handler_1, 0, sizeof(s_st_W25Q_Handler_1));
}

/**
 * @brief 初始化外部闪存并重置读写管理状态。
 *
 * 调用底层器件初始化接口，然后清空 A 区的管理状态。
 * 本接口不返回底层初始化状态。
 *
 * @return 无。
 */
void W25Q64_Init(void)
{
    W25Qx_Init();
    W25Q64_ResetDownload();
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
uint8_t W25Q64_EraseChip(void)
{
    if (W25Qx_OK != W25Qx_Erase_Chip())
    {
        return 1U;
    }
    W25Q64_ResetDownload();
    return 0U;
}

/**
 * @brief 刷新 A 区的当前缓冲数据并逐页回读比较。
 *
 * 处理步骤：
 * 1. 缓冲区为空时直接返回成功。
 * 2. 擦除当前写入位置所在的扇区。
 * 3. 按有效长度分页写入，并回读比较每页内容。
 * 4. 成功后累计已写入长度并清空缓冲区索引。
 * 失败时作废本次分区管理状态。
 *
 * @return 缓冲数据刷新结果。
 * @retval 0 刷新完成，或缓冲区为空。
 * @retval 1 扇区擦除、页写入或回读比较失败。
 */
static uint8_t W25Q64_WriteBuffer(void)
{
    uint32_t addr = s_st_W25Q_Handler_1.write_index;  /* 本次 A 区写入起始地址。 */
    /* 当前缓冲区待刷新的有效字节数。 */
    uint16_t length = s_st_W25Q_Handler_1.write_databuf_index;
    uint8_t read_buffer[256] = {0U};  /* 单页回读比较缓冲区。 */

    if (0U == length)
    {
        return 0U;
    }
    if (W25Qx_OK != W25Qx_Erase_Block(addr))
    {
        W25Q64_ResetDownload();
        return 1U;
    }
    for (uint16_t index = 0U; index < length; )
    {
        uint16_t page_size = length - index;  /* 本轮分页写入的有效字节数。 */
        if (page_size > W25Qx_Para.PAGE_SIZE)
        {
            page_size = (uint16_t)W25Qx_Para.PAGE_SIZE;
        }
        if ((W25Qx_OK != W25Qx_Write(
                 &s_st_W25Q_Handler_1.databuf[index], addr + index,
                 page_size)) ||
            (W25Qx_OK != W25Qx_Read(read_buffer, addr + index, page_size)) ||
            (0 != memcmp(read_buffer,
                         &s_st_W25Q_Handler_1.databuf[index], page_size)))
        {
            W25Qx_Disable();
            W25Q64_ResetDownload();
            return 1U;
        }
        index += page_size;
    }
    s_st_W25Q_Handler_1.write_index += length;
    s_st_W25Q_Handler_1.write_databuf_index = 0U;
    s_st_W25Q_Handler_1.write_sector_index =
        (uint8_t)(s_st_W25Q_Handler_1.write_index / 4096U);
    return 0U;
}

/**
 * @brief 向 A 区追加文件数据，并刷新已填满的扇区。
 *
 * 检查容量和存储组织参数，再将数据追加到扇区缓冲区。
 * 每满 4096 字节刷新一次，并检查写入及回读结果。
 *
 * @param[in] data 待写入的数据缓冲区；非零长度时须有效。
 * @param[in] length 本次追加的有效字节数。
 *
 * @return 数据追加结果。
 * @retval 0 处理成功。
 * @retval 1 参数、容量、擦除、写入或回读失败。
 *
 * @note 尾部刷新后若不足整扇区，开始新文件须先重置管理状态。
 */
uint8_t W25Q64_WriteData(uint8_t *data, uint32_t length)
{
    if (((NULL == data) && (0U != length)) ||
        (4096U != W25Qx_Para.SUBSECTOR_SIZE) ||
        (256U != W25Qx_Para.PAGE_SIZE) ||
        (0U != (s_st_W25Q_Handler_1.write_index % 4096U)) ||
        (s_st_W25Q_Handler_1.write_index > W25Q64_DOWNLOAD_MAX_SIZE) ||
        (s_st_W25Q_Handler_1.write_databuf_index >
         W25Q64_DOWNLOAD_MAX_SIZE - s_st_W25Q_Handler_1.write_index) ||
        (length > W25Q64_DOWNLOAD_MAX_SIZE -
                  s_st_W25Q_Handler_1.write_index -
                  s_st_W25Q_Handler_1.write_databuf_index))
    {
        return 1U;
    }
    for (uint32_t index = 0U; index < length; index++)
    {
        s_st_W25Q_Handler_1.databuf[
            s_st_W25Q_Handler_1.write_databuf_index++] = data[index];
        if (4096U == s_st_W25Q_Handler_1.write_databuf_index)
        {
            if (0U != W25Q64_WriteBuffer())
            {
                return 1U;
            }
        }
    }
    return 0U;
}

/**
 * @brief 刷新 A 区最后不足一个扇区的数据。
 *
 * 检查存储组织参数，再按实际缓冲长度执行刷新。
 * 缓冲区为空时返回成功，重复调用不会重复累计长度。
 *
 * @return 尾部刷新结果。
 * @retval 0 刷新完成，或缓冲区为空。
 * @retval 1 参数、擦除、写入或回读比较失败。
 */
uint8_t W25Q64_WriteData_End(void)
{
    if ((4096U != W25Qx_Para.SUBSECTOR_SIZE) ||
        (256U != W25Qx_Para.PAGE_SIZE))
    {
        return 1U;
    }
    return W25Q64_WriteBuffer();
}

/**
 * @brief 获取 A 区已完成物理写入的有效长度。
 *
 * 不包含尚未刷新的缓冲数据。
 *
 * @return 已写入的有效字节数。
 */
uint32_t W25Q64_GetWrittenSize(void)
{
    return s_st_W25Q_Handler_1.write_index;
}

/**
 * @brief 从 A 区的当前读取位置读取有效数据。
 *
 * 单次最多读取 4096 字节，尾部只返回剩余有效字节。
 * 有效长度输出地址先清零；仅在底层读取成功后推进索引。
 *
 * @param[out] data 接收缓冲区，须至少容纳 4096 字节。
 * @param[out] length 本次实际读取字节数的输出地址。
 *
 * @return 顺序读取结果。
 * @retval 0 读取完成。
 * @retval 1 已无有效数据可读。
 * @retval 2 参数无效或底层读取失败。
 */
uint8_t W25Q64_ReadData(uint8_t *data, uint16_t *length)
{
    uint32_t read_length = 0U;  /* 本次顺序读取的有效字节数。 */

    if (NULL == length)
    {
        return 2U;
    }
    *length = 0U;
    if ((NULL == data) || (4096U != W25Qx_Para.SUBSECTOR_SIZE))
    {
        return 2U;
    }
    if (s_st_W25Q_Handler_1.read_index >= s_st_W25Q_Handler_1.write_index)
    {
        return 1U;
    }
    read_length = s_st_W25Q_Handler_1.write_index -
                  s_st_W25Q_Handler_1.read_index;
    if (read_length > 4096U)
    {
        read_length = 4096U;
    }
    if (W25Qx_OK != W25Qx_Read(data, s_st_W25Q_Handler_1.read_index,
                              read_length))
    {
        return 2U;
    }
    s_st_W25Q_Handler_1.read_index += read_length;
    s_st_W25Q_Handler_1.read_sector_index =
        (uint8_t)(s_st_W25Q_Handler_1.read_index / 4096U);
    *length = (uint16_t)read_length;
    return 0U;
}
/************************* Function Definitions *************************/
