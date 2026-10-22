/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bootloader.c
 *
 * @par dependencies
 * - bootloader.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides encrypted APP installation and application handover.
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

/****************************** Includes ******************************/
#include "bootloader.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
typedef void (*app_entry_t)(void);
/****************************** Defines ******************************/

/**
 * @brief 校验 APP 初始栈顶地址，并将控制权交给 APP。
 *
 * 处理步骤：
 * 1. 读取并校验 APP 的初始栈顶地址。
 * 2. 从 APP 向量表读取复位入口地址。
 * 3. 关闭中断，并将向量表基址切换到 APP 运行区。
 * 4. 复位时钟配置，设置 APP 的主堆栈指针。
 * 5. 跳转到 APP 入口地址。
 *
 * @return 无返回值；跳转成功后通常不会返回。
 */
void bootloader_jump_to_app(void)
{
    uint32_t app_sp = *(__IO uint32_t *)APP_RUN_BASE_ADDR;
    uint32_t app_entry_addr;
    app_entry_t app_entry;

    /* 1. 校验 APP 的初始栈顶地址。 */
    if ((app_sp & SRAM_ADDR_MASK) != SRAM_BASE)
    {
        return;
    }

    /* 2. 从向量表读取 APP 的复位入口地址。 */
    app_entry_addr = *(__IO uint32_t *)(APP_RUN_BASE_ADDR + 4U);
    app_entry = (app_entry_t)app_entry_addr;

    /* 3. 关闭中断，并切换向量表基址。 */
    __disable_irq();
    SCB->VTOR = APP_RUN_BASE_ADDR;

    /* 4. 复位时钟配置，并设置 APP 的主堆栈指针。 */
    RCC_DeInit();
    __set_MSP(app_sp);

    /* 5. 跳转到 APP 入口地址。 */
    app_entry();
}

/**
 * @brief 从备份区读取加密固件，解密后安装到 APP 运行区。
 *
 * 处理步骤：
 * 1. 校验加密包长度，并解密首个 16 字节数据块以获取 APP 明文大小。
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
int8_t copy_backup_to_app(int32_t firmware_size)
{
    uint32_t run_flash_destination = APP_RUN_BASE_ADDR;
    const uint8_t *backup_flash_source =
        (const uint8_t *)APP_BACKUP_BASE_ADDR;
    /* 每次安装固件时重新初始化 AES-CBC 向量和 256 位密钥。 */
    uint8_t iv[16] = {
        0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
        0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U
    };
    uint8_t key[32] = {
        0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
        0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
        0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
        0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U
    };
    /* 在 RAM 中缓存当前解密的数据块及其长度信息。 */
    uint8_t decrypted_block[16];
    uint32_t decrypted_app_size;
    uint32_t block_count;
    uint32_t bytes_written = 0U;

    /* 1. 加密包由 16 字节头部及完整的 AES 数据块组成。 */
    if ((firmware_size <= 16) ||
        ((uint32_t)firmware_size > APP_BACKUP_SIZE) ||
        (((uint32_t)firmware_size % 16U) != 0U))
    {
        return -1;
    }

    /* 2. 先解密头部，再决定是否擦除运行区。 */
    for (uint32_t index = 0U; index < 16U; index++)
    {
        decrypted_block[index] = backup_flash_source[index];
    }
    backup_flash_source += 16U;
    Aes_IV_key256bit_Decode(iv, decrypted_block, key);

    /* 头部最后 4 字节按小端序保存 APP 明文大小。 */
    decrypted_app_size = (uint32_t)decrypted_block[12] |
                         ((uint32_t)decrypted_block[13] << 8U) |
                         ((uint32_t)decrypted_block[14] << 16U) |
                         ((uint32_t)decrypted_block[15] << 24U);

    if ((0U == decrypted_app_size) ||
        (decrypted_app_size > APP_RUN_SIZE))
    {
        return -1;
    }

    /* 3. 校验接收长度与明文数据块数量是否一致。 */
    block_count = (decrypted_app_size + 15U) / 16U;
    if ((uint32_t)firmware_size != (16U + (block_count * 16U)))
    {
        return -1;
    }

    /* 4. 擦除 APP 运行区中存放明文的扇区。 */
    if (0U != Flash_erase(APP_RUN_BASE_ADDR, decrypted_app_size))
    {
        return -2;
    }

    /* 5. 逐块解密，并将明文写入 APP 运行区。 */
    for (uint32_t block_index = 0U;
         block_index < block_count;
         block_index++)
    {
        uint32_t bytes_in_block = decrypted_app_size - bytes_written;

        if (bytes_in_block > 16U)
        {
            bytes_in_block = 16U;
        }

        for (uint32_t index = 0U; index < 16U; index++)
        {
            decrypted_block[index] = backup_flash_source[index];
        }
        backup_flash_source += 16U;
        Aes_IV_key256bit_Decode(iv, decrypted_block, key);

        /* 最后不足 4 字节的部分用 0xFF 补齐后按字写入。 */
        for (uint32_t index = 0U; index < bytes_in_block; index += 4U)
        {
            uint32_t word = 0xFFFFFFFFU;

            for (uint32_t byte_index = 0U;
                 (byte_index < 4U) &&
                 ((index + byte_index) < bytes_in_block);
                 byte_index++)
            {
                uint32_t shift = byte_index * 8U;
                word = (word & ~(0xFFU << shift)) |
                       ((uint32_t)decrypted_block[index + byte_index] <<
                        shift);
            }

            Flash_Write(run_flash_destination, word);

            if (*(volatile uint32_t *)run_flash_destination != word)
            {
                return -2;
            }

            run_flash_destination += 4U;
        }

        bytes_written += bytes_in_block;
    }

    return 0;
}
