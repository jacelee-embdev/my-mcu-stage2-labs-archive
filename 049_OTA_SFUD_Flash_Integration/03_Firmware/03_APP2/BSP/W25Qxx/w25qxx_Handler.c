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
 * @brief Manages buffered downloads and sequential reads in W25Q A.
 *
 * Processing flow:
 *
 * Reset metadata before a new download. Append only valid file bytes,
 * erase and program sectors through SFUD, verify data by readback,
 * flush the tail, and expose the written length for completion checks.
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
#define W25Q_BUFFER_SIZE (4096U)  /* 当前管理层使用的扇区缓冲大小。 */
/******************************* Defines *******************************/

/************************** Private Variables **************************/
/* 保存读写缓冲区及其索引。 */
static st_W25Q_Handler s_st_W25Q_Handler_1;
/* 仅保存初始化成功且满足 A 区布局要求的设备指针。 */
static const sfud_flash *s_p_flash = NULL;
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
 * @brief 初始化外部闪存并重置 A 区读写管理状态。
 *
 * 调用 SFUD 适配层初始化，取得设备并检查初始化结果、容量和擦除粒度。
 * 仅接受容量足以容纳 64 KiB A 区、擦除粒度为 4096 字节的设备。
 * 无论识别结果如何均清空 A 区管理状态；设备不可用时读写返回错误。
 *
 * @return 无。
 */
void W25Q64_Init(void)
{
    const sfud_flash *p_flash = NULL;  /* SFUD 识别得到的设备。 */

    s_p_flash = NULL;
    SFUD_W25Q64_Init();
    p_flash = sfud_get_device(SFUD_W25Q64_DEVICE_INDEX);
    if ((NULL != p_flash) && p_flash->init_ok &&
        (p_flash->chip.capacity >= W25Q64_DOWNLOAD_MAX_SIZE) &&
        (W25Q_BUFFER_SIZE == p_flash->chip.erase_gran))
    {
        s_p_flash = p_flash;
    }
    W25Q64_ResetDownload();
}

/**
 * @brief 擦除整片外部闪存并重置管理状态。
 *
 * 底层擦除成功后清空管理状态；失败时直接返回。
 *
 * @return 整片擦除结果。
 * @retval 0 擦除完成。
 * @retval 1 设备不可用或底层擦除失败。
 */
uint8_t W25Q64_EraseChip(void)
{
    if ((NULL == s_p_flash) ||
        (SFUD_SUCCESS != sfud_chip_erase(s_p_flash)))
    {
        return 1U;
    }
    W25Q64_ResetDownload();
    return 0U;
}

/**
 * @brief 通过 SFUD 刷新 A 区缓冲数据并分段回读比较。
 *
 * 1. 缓冲区为空时直接返回成功。
 * 2. 检查长度、扇区对齐与 A 区边界。
 * 3. 擦除覆盖扇区并写入有效数据，再用 256 字节缓冲区回读比较。
 * 4. 成功后累计已写入长度并清空缓冲索引。
 * 底层访问或比较失败时作废本次下载管理状态。
 *
 * @return 缓冲数据刷新结果。
 * @retval 0 刷新完成，或缓冲区为空。
 * @retval 1 参数、擦除、写入或回读比较失败。
 */
static uint8_t W25Q64_WriteBuffer(void)
{
    uint32_t addr = s_st_W25Q_Handler_1.write_index;  /* A 区写入地址。 */
    /* 当前缓冲区待刷新的有效字节数。 */
    uint16_t length = s_st_W25Q_Handler_1.write_databuf_index;
    uint8_t read_buffer[256] = {0U};  /* 分段回读比较缓冲区。 */

    if (0U == length)
    {
        return 0U;
    }
    if ((addr > W25Q64_DOWNLOAD_MAX_SIZE) ||
        (length > W25Q_BUFFER_SIZE) ||
        (0U != (addr % W25Q_BUFFER_SIZE)) ||
        (length > W25Q64_DOWNLOAD_MAX_SIZE - addr))
    {
        return 1U;
    }
    if (SFUD_SUCCESS != sfud_erase_write(s_p_flash, addr, length,
                                        s_st_W25Q_Handler_1.databuf))
    {
        W25Q64_ResetDownload();
        return 1U;
    }
    for (uint16_t index = 0U; index < length; )
    {
        uint16_t read_size = length - index;  /* 本轮回读字节数。 */

        if (read_size > sizeof(read_buffer))
        {
            read_size = (uint16_t)sizeof(read_buffer);
        }
        if ((SFUD_SUCCESS != sfud_read(s_p_flash, addr + index,
                                      read_size, read_buffer)) ||
            (0 != memcmp(read_buffer,
                         &s_st_W25Q_Handler_1.databuf[index], read_size)))
        {
            W25Q64_ResetDownload();
            return 1U;
        }
        index += read_size;
    }
    s_st_W25Q_Handler_1.write_index += length;
    s_st_W25Q_Handler_1.write_databuf_index = 0U;
    s_st_W25Q_Handler_1.write_sector_index =
        (uint8_t)(s_st_W25Q_Handler_1.write_index / W25Q_BUFFER_SIZE);
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
        (NULL == s_p_flash) ||
        (s_st_W25Q_Handler_1.write_databuf_index >= W25Q_BUFFER_SIZE) ||
        (0U != (s_st_W25Q_Handler_1.write_index % W25Q_BUFFER_SIZE)) ||
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
        if (W25Q_BUFFER_SIZE == s_st_W25Q_Handler_1.write_databuf_index)
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
    if (NULL == s_p_flash)
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
    if ((NULL == data) || (NULL == s_p_flash))
    {
        return 2U;
    }
    if (s_st_W25Q_Handler_1.read_index >= s_st_W25Q_Handler_1.write_index)
    {
        return 1U;
    }
    read_length = s_st_W25Q_Handler_1.write_index -
                  s_st_W25Q_Handler_1.read_index;
    if (read_length > W25Q_BUFFER_SIZE)
    {
        read_length = W25Q_BUFFER_SIZE;
    }
    if (SFUD_SUCCESS != sfud_read(s_p_flash,
                                  s_st_W25Q_Handler_1.read_index,
                                  read_length, data))
    {
        return 2U;
    }
    s_st_W25Q_Handler_1.read_index += read_length;
    s_st_W25Q_Handler_1.read_sector_index =
        (uint8_t)(s_st_W25Q_Handler_1.read_index / W25Q_BUFFER_SIZE);
    *length = (uint16_t)read_length;
    return 0U;
}
/************************* Function Definitions *************************/
