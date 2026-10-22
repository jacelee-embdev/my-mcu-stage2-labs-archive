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
 * @brief Declares encrypted APP installation and application handover APIs.
 *
 * Processing flow:
 *
 * Call copy_backup_to_app() to decrypt the image from the backup region into
 * the run region, then call bootloader_jump_to_app() to start the APP.
 *
 * @version V1.2 2026-09-27
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

/****************************** Includes ******************************/
#include "main.h"
#include "aes.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/* APP 运行区与加密固件备份区在内部 Flash 中的起始地址。 */
#define APP_RUN_BASE_ADDR       ((uint32_t)0x08008000U)
#define APP_BACKUP_BASE_ADDR    ((uint32_t)0x08020000U)

/* APP 运行区与加密固件备份区的可用容量。 */
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
 * @brief 从备份区读取加密固件，解密后安装到 APP 运行区。
 *
 * 处理步骤：
 * 1. 校验加密包长度，并从解密后的头部读取 APP 明文大小。
 * 2. 校验明文大小及数据块数量，擦除 APP 运行区。
 * 3. 逐块解密固件，以 32 位字写入运行区并回读校验。
 *
 * @param[in] firmware_size 收到的加密固件文件大小，单位为字节。
 *
 * @return 安装结果。
 * @retval  0 APP 安装成功。
 * @retval -1 加密包长度或解密得到的 APP 大小无效。
 * @retval -2 Flash 擦除或写入回读校验失败。
 */
int8_t copy_backup_to_app(int32_t firmware_size);

/****************************** Declaring ******************************/

#endif /* End of __BOOTLOADER_H */
