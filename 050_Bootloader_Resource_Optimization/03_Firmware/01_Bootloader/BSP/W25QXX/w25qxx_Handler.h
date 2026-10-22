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
 * @brief Declares buffered W25Q A/B region access and metadata interfaces.
 *
 * Processing flow:
 *
 * Initialize region state or restore a recorded file length. Append data
 * and flush the tail, then reset the read position and read the valid data.
 * Region state reset does not physically erase the flash.
 *
 * @par provenance
 * Adapted with reference to the course driver portability example.
 * SFUD library code retains its original upstream license.
 *
 * @version V1.1 2026-10-09
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __W25Q_HANDLER_H__
#define __W25Q_HANDLER_H__

/******************************* Includes *******************************/
#include "stm32f4xx.h"
#include "main.h"
#include "w25qxx.h"
/******************************* Includes *******************************/

/*************************** Type Definitions ***************************/
/* 使用 4096 字节扇区缓冲区；每个 64 KiB 分区包含 16 个扇区。 */
typedef struct
{
    u8 databuf[4096];                 /* 暂存一个扇区的数据。 */
    u16 write_databuf_index;          /* 缓冲区下一写入位置。 */
    u32 write_index;                  /* 已写入闪存的字节数。 */
    u8 write_sector_index;            /* 已写入位置对应的扇区序号。 */

    u32 read_index;                   /* 已读取的字节数。 */
    u8 read_sector_index;             /* 已读取位置对应的扇区序号。 */
} st_W25Q_Handler;
/*************************** Type Definitions ***************************/

/******************************* Defines *******************************/
#define EXTERN_Flash
#define BLOCK_1 (0)
#define BLOCK_2 (1)

#define BLOCK_SIZE (0x10000)
/******************************* Defines *******************************/

/************************ Function Declarations ************************/
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
void Erase_Flash_Block(u8 block_index);

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
void SetBlockParmeter(u8 block_index, uint32_t app_size);

/**
 * @brief 初始化外部闪存并重置读写管理状态。
 *
 * 调用 SFUD 适配层初始化，然后清空 A/B 两个分区的管理状态。
 * 仅接受容量至少 128 KiB、擦除粒度为 4096 字节的设备。
 * 本接口不返回初始化状态；设备不可用时后续读写返回错误。
 *
 * @return 无。
 */
void W25Q64_Init(void);

/**
 * @brief 擦除整片外部闪存并重置管理状态。
 *
 * 底层擦除成功后清空管理状态；失败时直接返回。
 *
 * @return 整片擦除结果。
 * @retval 0 擦除完成。
 * @retval 1 底层擦除失败。
 */
u8 W25Q64_EraseChip(void);

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
u8 W25Q64_WriteData(u8 block_index, u8 *data, u32 length);

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
u8 W25Q64_WriteData_End(u8 block_index);

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
uint32_t Read_BlockSize(uint8_t block_index);

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
void W25Q64_ReadData_Reset(u8 block_index);

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
u8 W25Q64_ReadData(u8 block_index, u8 *data, u16 *length);
/************************ Function Declarations ************************/

#endif /* __W25Q_HANDLER_H__ */
