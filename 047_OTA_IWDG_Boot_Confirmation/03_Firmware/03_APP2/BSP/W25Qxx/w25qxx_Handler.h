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
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares buffered W25Q download and valid-length interfaces.
 *
 * Processing flow:
 *
 * Reset download metadata, append file data, and flush any buffered tail.
 * Check the physically written length before committing download metadata.
 * Sequential reads return only bytes within the recorded valid length.
 *
 * @version V1.2 2026-10-06
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
#include <stdint.h>
/******************************* Includes *******************************/

/*************************** Type Definitions ***************************/
/* 页大小为 256 字节；每扇区 16 页，共 4096 字节；每块 16 个扇区。 */
typedef struct
{
    uint8_t databuf[4096];            /* 暂存一个扇区的数据。 */
    uint16_t write_databuf_index;     /* 缓冲区下一写入位置。 */
    uint32_t write_index;             /* 已写入闪存的字节数。 */
    uint8_t write_sector_index;       /* 已写入位置对应的扇区序号。 */

    uint32_t read_index;              /* 已读取的字节数。 */
    uint8_t read_sector_index;        /* 已读取位置对应的扇区序号。 */
} st_W25Q_Handler;
/*************************** Type Definitions ***************************/

/******************************* Defines *******************************/
#define EXTERN_Flash
/* APP 仅下载到外部 A 区，容量与 Bootloader 的 BLOCK_SIZE 一致。 */
#define W25Q64_DOWNLOAD_MAX_SIZE (0x10000U)
/******************************* Defines *******************************/

/************************ Function Declarations ************************/
/**
 * @brief 重置 A 区的读写缓冲区与管理索引。
 *
 * 仅清空内存中的管理状态，不执行物理擦除。
 * 开始新文件前调用，后续写入从分区首地址开始。
 *
 * @return 无。
 */
void W25Q64_ResetDownload(void);

/**
 * @brief 初始化外部闪存并重置读写管理状态。
 *
 * 调用底层器件初始化接口，然后清空 A 区的管理状态。
 * 本接口不返回底层初始化状态。
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
uint8_t W25Q64_EraseChip(void);

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
uint8_t W25Q64_WriteData(uint8_t *data, uint32_t length);

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
uint8_t W25Q64_WriteData_End(void);

/**
 * @brief 获取 A 区已完成物理写入的有效长度。
 *
 * 不包含尚未刷新的缓冲数据。
 *
 * @return 已写入的有效字节数。
 */
uint32_t W25Q64_GetWrittenSize(void);

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
uint8_t W25Q64_ReadData(uint8_t *data, uint16_t *length);
/************************ Function Declarations ************************/

#endif /* 结束头文件保护。 */
