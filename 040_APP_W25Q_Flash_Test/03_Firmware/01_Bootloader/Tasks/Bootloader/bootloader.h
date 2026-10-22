/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bootloader.h
 *
 * @par dependencies
 * - main.h
 * - aes.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares APP installation from W25Q and application handover.
 *
 * Processing flow:
 *
 * Call copy_backup_to_app() to install the image from W25Q into the
 * internal run region, then call bootloader_jump_to_app() to start it.
 *
 * @version v1.3 2026-09-30
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

/****************************** Includes ******************************/
#include "main.h"
#include "aes.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/* APP 运行区及原内部 Flash 备份区的起始地址。 */
#define APP_RUN_BASE_ADDR       ((uint32_t)0x08008000U)
#define APP_BACKUP_BASE_ADDR    ((uint32_t)0x08020000U)

/* APP 运行区容量及沿用原备份区容量的升级包上限。 */
#define APP_RUN_SIZE            ((uint32_t)0x00018000U)
#define APP_BACKUP_SIZE         ((uint32_t)0x00020000U)

/* 用于校验 APP 初始栈顶地址是否位于内部 SRAM 的掩码。 */
#define SRAM_ADDR_MASK          ((uint32_t)0x2FFE0000U)
/****************************** Defines ******************************/

/****************************** Declaring ******************************/

/**
 * @brief 校验 APP 初始栈顶地址，并将控制权交给 APP。
 *
 * 校验 APP 初始栈顶地址，读取复位入口，关闭中断并切换向量表基址，
 * 随后复位时钟配置、设置主堆栈指针并跳转到 APP。
 *
 * @return 无返回值；跳转成功后通常不会返回。
 */
void bootloader_jump_to_app(void);

/**
 * @brief 从外部 W25Q 读取加密固件，解密后安装到 APP 运行区。
 *
 * 处理步骤：
 * 1. 校验长度，从 W25Q 读取并解密首块，获取 APP 明文大小。
 * 2. 校验明文大小及数据块数量，擦除 APP 运行区。
 * 3. 按扇区读取密文，逐块解密并写入运行区，随后回读校验。
 *
 * @param[in] firmware_size 收到的加密固件文件大小，单位为字节。
 *
 * @return 安装结果。
 * @retval  0 APP 安装成功。
 * @retval -1 密文长度、APP 大小无效，或 W25Q 读取失败。
 * @retval -2 Flash 擦除或写入回读校验失败。
 */
int8_t copy_backup_to_app(int32_t firmware_size);

/****************************** Declaring ******************************/

#endif /* __BOOTLOADER_H__ */
