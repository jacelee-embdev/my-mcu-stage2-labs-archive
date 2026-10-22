/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bootloader.c
 *
 * @par dependencies
 * - bootloader.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Manages APP installation, reset requests, and first-start checks.
 *
 * Processing flow:
 *
 * With EEPROM state 0x00, a pressed key starts a download. Otherwise, validate
 * the APP entry, save a retained RAM request, and reset for the early jump in
 * main(). The jump routine also serves first checks and recovery paths.
 * Receive a package into A or restore the length of a completed package.
 * Decrypt A into B, back up a valid old APP into A, and install B into
 * internal Flash. Save its length, mark state 0x33, and reset. The first
 * check writes 0x44, enables IWDG, and launches the installed APP.
 * The APP clears the state to 0x00 before waiting for the watchdog reset.
 * Re-entering with 0x44 clears the state and resets without restoring A.
 * Installation errors can restore a valid backup in the same execution.
 *
 * @version v1.7 2026-10-08
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "bootloader.h"
#include "debug.h"
/******************************* Includes *******************************/

/************************** External Variables **************************/
/* main.c 定义的保留 RAM 请求；正常启动分支写入后执行软复位。 */
extern volatile uint32_t g_app_jump_request;
/************************** External Variables **************************/

/******************************* Defines *******************************/
#define TAG "bootloader"
#define YMODEM_RECEIVE_BUFFER_SIZE (1024U)
typedef void (*app_entry_t)(void);
/******************************* Defines *******************************/

/****************************** Variables ******************************/
/* 下载数据暂存区，避免占用启动栈。 */
static uint8_t g_ymodem_receive_buffer[YMODEM_RECEIVE_BUFFER_SIZE] = {0U};
/* 外部闪存搬运缓冲区，每次最多读取一个扇区。 */
uint8_t Mem_Read_Buffer[4096] = {0U};
/****************************** Variables ******************************/

/************************* Function Definitions *************************/
/**
 * @brief 请求 MCU 系统软件复位，重新进入 Bootloader。
 *
 * 调用 CMSIS 系统复位接口；调用者须先保存所需的 EEPROM 状态或 RAM 请求。
 * 普通全局变量会由启动代码初始化，跳转请求依靠 UNINIT 区域保留。
 *
 * @return 无；系统复位后不返回。
 */
void System_SoftwareReset(void)
{
    // __NVIC_SystemReset();
    NVIC_SystemReset();
}

/**
 * @brief 校验 APP 向量表中的初始栈顶和复位入口。
 *
 * 检查镜像长度、栈顶范围及对齐，并检查入口地址和 Thumb 状态。
 * 只校验启动入口，不代表整个固件的完整性校验。
 *
 * @param[in] p_app_data 指向包含初始向量表的镜像数据。
 * @param[in] app_size 用于限制复位入口范围的镜像字节数。
 *
 * @return 向量表检查结果。
 * @retval 1 启动入口有效。
 * @retval 0 参数或启动入口无效。
 */
static uint8_t bootloader_check_app(const uint8_t *p_app_data,
                                    uint32_t app_size)
{
    uint32_t app_sp;         /* APP 向量表中的初始栈顶地址。 */
    uint32_t app_entry_addr; /* APP 向量表中的复位入口地址。 */

    if ((NULL == p_app_data) || (app_size < 8U) || (app_size > APP_RUN_SIZE))
    {
        return 0U;
    }
    memcpy(&app_sp, p_app_data, sizeof(app_sp));
    memcpy(&app_entry_addr, p_app_data + 4U, sizeof(app_entry_addr));
    if ((app_sp <= SRAM_BASE) || (app_sp > SRAM_BASE + APP_SRAM_SIZE) ||
        ((app_sp & 7U) != 0U) || ((app_entry_addr & 1U) == 0U) ||
        ((app_entry_addr & ~1U) < APP_RUN_BASE_ADDR) ||
        ((app_entry_addr & ~1U) >= APP_RUN_BASE_ADDR + app_size))
    {
        return 0U;
    }
    return 1U;
}

/**
 * @brief 校验内部 APP 启动入口并交接控制权。
 *
 * 处理步骤：
 * 1. 检查初始栈顶、复位入口地址和 Thumb 状态。
 * 2. 关闭中断并将向量表基址切换到 APP 运行区。
 * 3. 复位时钟配置，设置主堆栈指针并调用 APP 入口。
 * 本函数可在 main() 的外设和日志初始化之前调用，入口无效时静默返回。
 * APP 启动代码负责初始化运行环境并重新使能中断。
 *
 * @return 无；入口无效时返回，正常启动 APP 后通常不会返回。
 */
void bootloader_jump_to_app(void)
{
    /* APP 向量表中的初始栈顶地址。 */
    uint32_t app_sp = *(__IO uint32_t *)APP_RUN_BASE_ADDR;
    uint32_t app_entry_addr; /* APP 向量表中的复位入口地址。 */
    app_entry_t app_entry;   /* APP 复位入口函数指针。 */

    if (0U ==
        bootloader_check_app((const uint8_t *)APP_RUN_BASE_ADDR, APP_RUN_SIZE))
    {
        /* 提前跳转时日志尚未初始化，失败后交由调用者处理。 */
        return;
    }
    app_entry_addr = *(__IO uint32_t *)(APP_RUN_BASE_ADDR + 4U);
    app_entry      = (app_entry_t)app_entry_addr;

    /* 关闭中断后交接；APP 负责初始化外设并重新使能中断。 */
    __disable_irq();
    SCB->VTOR = APP_RUN_BASE_ADDR;
    RCC_DeInit();
    __set_MSP(app_sp);
    app_entry();
}

/**
 * @brief 将 A 区加密固件解密为 B 区的有效 APP 明文。
 *
 * 处理步骤：
 * 1. 检查加密包长度和 A 区管理长度。
 * 2. 用初始向量解密文件头，获取并校验明文长度。
 * 3. 连续解密数据块，只将有效明文字节写入 B 区。
 * 4. 刷新尾部数据，校验明文长度及 APP 启动入口。
 * 每次更新重新设置初始向量；本阶段不修改内部 APP。
 *
 * @param[in] firmware_size A 区加密文件的实际字节数。
 *
 * @return 解密结果。
 * @retval 0 解密、写入和启动入口检查完成。
 * @retval -1 长度记录、包格式或 A 区读取无效。
 * @retval -2 B 区写入、刷新或长度检查失败。
 * @retval -3 解密后的 APP 启动入口无效。
 */
int8_t exitA_to_extiB(int32_t firmware_size)
{
    static uint8_t backup_read_buffer[4096]; /* 每次读取一个外部扇区。 */
    const uint8_t *p_backup_flash_source; /* 当前待解密数据块的源地址。 */
    uint16_t backup_read_length = 0U; /* 本次从 A 区读取的有效字节数。 */
    uint32_t backup_read_index = 16U; /* 当前读取缓冲区内的解密位置。 */
    uint8_t decrypted_block[16]; /* 当前 AES 解密块。 */
    uint32_t decrypted_app_size; /* 文件头记录的有效 APP 明文长度。 */
    uint32_t block_count; /* 需要解密的数据块数，不包含文件头。 */
    uint32_t bytes_written = 0U; /* 本次已搬运的有效字节数。 */
    /* 每次更新都从相同的初始向量开始，后续数据块连续使用 CBC 状态。 */
    uint8_t iv[16]  = {0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
                       0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U};
    uint8_t key[32] = {0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
                       0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
                       0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U,
                       0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U, 0x31U, 0x32U};

    Erase_Flash_Block(BLOCK_2);
    if ((firmware_size <= 16) || ((uint32_t)firmware_size > APP_BACKUP_SIZE) ||
        (((uint32_t)firmware_size % 16U) != 0U) ||
        (Read_BlockSize(BLOCK_1) != (uint32_t)firmware_size))
    {
        return -1;
    }

    /* 密文头部和后续数据始终从 A 区读取。 */
    W25Q64_ReadData_Reset(BLOCK_1);
    if ((0U !=
         W25Q64_ReadData(BLOCK_1, backup_read_buffer, &backup_read_length)) ||
        (backup_read_length < 16U))
    {
        return -1;
    }
    memcpy(decrypted_block, backup_read_buffer, 16U);
    Aes_IV_key256bit_Decode(iv, decrypted_block, key);
    decrypted_app_size = (uint32_t)decrypted_block[12] |
                         ((uint32_t)decrypted_block[13] << 8U) |
                         ((uint32_t)decrypted_block[14] << 16U) |
                         ((uint32_t)decrypted_block[15] << 24U);
    if ((decrypted_app_size < 8U) || (decrypted_app_size > APP_RUN_SIZE) ||
        (decrypted_app_size > BLOCK_SIZE))
    {
        return -1;
    }
    block_count = (decrypted_app_size + 15U) / 16U;
    if ((uint32_t)firmware_size != (16U + block_count * 16U))
    {
        return -1;
    }
    elog_i(TAG, "Decrypt A to B: APP size=%lu.\r\n",
           (unsigned long)decrypted_app_size);

    for (uint32_t block_index = 0U; block_index < block_count; block_index++)
    {
        /* 当前解密块需要保存的有效字节数。 */
        uint32_t bytes_in_block = decrypted_app_size - bytes_written;
        if (bytes_in_block > 16U)
        {
            bytes_in_block = 16U;
        }
        if (backup_read_index == backup_read_length)
        {
            if (0U != W25Q64_ReadData(BLOCK_1, backup_read_buffer,
                                      &backup_read_length))
            {
                Erase_Flash_Block(BLOCK_2);
                return -1;
            }
            backup_read_index = 0U;
        }
        if ((backup_read_length < backup_read_index) ||
            ((backup_read_length - backup_read_index) < 16U))
        {
            Erase_Flash_Block(BLOCK_2);
            return -1;
        }
        p_backup_flash_source = backup_read_buffer + backup_read_index;
        memcpy(decrypted_block, p_backup_flash_source, 16U);
        backup_read_index += 16U;
        Aes_IV_key256bit_Decode(iv, decrypted_block, key);
        if (0U != W25Q64_WriteData(BLOCK_2, decrypted_block, bytes_in_block))
        {
            Erase_Flash_Block(BLOCK_2);
            return -2;
        }
        bytes_written += bytes_in_block;
    }

    /* 全部解密完成后统一刷新，仅记录有效明文，不保留 AES 填充。 */
    if ((0U != W25Q64_WriteData_End(BLOCK_2)) ||
        (Read_BlockSize(BLOCK_2) != decrypted_app_size))
    {
        Erase_Flash_Block(BLOCK_2);
        return -2;
    }
    W25Q64_ReadData_Reset(BLOCK_2);
    if ((0U !=
         W25Q64_ReadData(BLOCK_2, backup_read_buffer, &backup_read_length)) ||
        (0U == bootloader_check_app(backup_read_buffer, decrypted_app_size)))
    {
        Erase_Flash_Block(BLOCK_2);
        return -3;
    }
    W25Q64_ReadData_Reset(BLOCK_2);
    return 0;
}

/**
 * @brief 按有效长度将内部旧 APP 备份到外部 A 区。
 *
 * 检查旧 APP 的长度和启动入口，再重置 A 区管理状态。
 * 将旧 APP 明文完整写入 A 区并刷新尾部；失败时作废 A 区长度。
 *
 * @param[in] app_size EEPROM 记录的旧 APP 明文字节数。
 *
 * @return 旧 APP 备份结果。
 * @retval 0 备份完成。
 * @retval -1 旧 APP 长度或启动入口无效。
 * @retval -2 A 区写入或刷新失败。
 */
int8_t internal_APP_to_exti_A(int32_t app_size)
{
    uint32_t flash_destination = APP_RUN_BASE_ADDR; /* 内部旧 APP 读取地址。 */

    if ((app_size < 8) || ((uint32_t)app_size > APP_RUN_SIZE) ||
        ((uint32_t)app_size > BLOCK_SIZE) ||
        (0U == bootloader_check_app((const uint8_t *)flash_destination,
                                    (uint32_t)app_size)))
    {
        return -1;
    }
    Erase_Flash_Block(BLOCK_1);
    if ((0U != W25Q64_WriteData(BLOCK_1, (uint8_t *)flash_destination,
                                (uint32_t)app_size)) ||
        (0U != W25Q64_WriteData_End(BLOCK_1)))
    {
        Erase_Flash_Block(BLOCK_1);
        return -2;
    }
    return 0;
}

/**
 * @brief 将外部分区的 APP 明文安装到内部运行区。
 *
 * 处理步骤：
 * 1. 校验源长度，并读取首段检查 APP 启动入口。
 * 2. 擦除内部 APP 对应范围。
 * 3. 逐字写入有效数据并回读比较。
 * 4. 检查安装后的 APP 启动入口。
 * 尾部不足一个字时使用 0xFF 补齐。
 *
 * @param[in] block_index 分区编号：BLOCK_1 为 A 区，BLOCK_2 为 B 区。
 *
 * @return 内部 APP 安装结果。
 * @retval 0 安装及入口检查完成。
 * @retval -1 长度或后续读取无效。
 * @retval -2 内部擦除或写入回读失败。
 * @retval -3 首段读取失败，或源/目标 APP 启动入口无效。
 */
static int8_t exti_to_internal_APP(uint8_t block_index)
{
    uint32_t flash_destination = APP_RUN_BASE_ADDR; /* 内部 APP 写入地址。 */
    uint32_t flash_size = Read_BlockSize(block_index); /* 源区有效长度。 */
    uint32_t bytes_written   = 0U; /* 本次已搬运的有效字节数。 */
    uint16_t Read_MemorySize = 0U; /* 当前读取段的有效字节数。 */

    if ((flash_size < 8U) || (flash_size > APP_RUN_SIZE) ||
        (flash_size > BLOCK_SIZE))
    {
        return -1;
    }

    /* 先读并检查入口，确认源数据有效后才擦除内部 Flash。 */
    W25Q64_ReadData_Reset(block_index);
    if ((0U !=
         W25Q64_ReadData(block_index, Mem_Read_Buffer, &Read_MemorySize)) ||
        (0U == bootloader_check_app(Mem_Read_Buffer, flash_size)))
    {
        return -3;
    }
    if (0U != Flash_erase(flash_destination, flash_size))
    {
        return -2;
    }

    for (;;)
    {
        if ((0U == Read_MemorySize) ||
            (Read_MemorySize > flash_size - bytes_written))
        {
            return -1;
        }
        for (uint16_t write_times = 0U; write_times < Read_MemorySize;
             write_times += 4U)
        {
            uint32_t word = 0xFFFFFFFFU; /* 当前待写入内部闪存的四字节数据。 */
            /* 尾部不足一个字时用 0xFF 补齐后写入。 */
            for (uint32_t byte_index = 0U;
                 (byte_index < 4U) &&
                 ((write_times + byte_index) < Read_MemorySize);
                 byte_index++)
            {
                uint32_t shift = byte_index * 8U; /* 当前字节的位移。 */
                word           = (word & ~(0xFFU << shift)) |
                       ((uint32_t)Mem_Read_Buffer[write_times + byte_index]
                        << shift);
            }
            Flash_Write(flash_destination, word);
            if (*(volatile uint32_t *)flash_destination != word)
            {
                return -2;
            }
            flash_destination += 4U;
        }
        bytes_written += Read_MemorySize;
        if (bytes_written == flash_size)
        {
            break;
        }
        if (0U !=
            W25Q64_ReadData(block_index, Mem_Read_Buffer, &Read_MemorySize))
        {
            return -1;
        }
    }
    return (0U != bootloader_check_app((const uint8_t *)APP_RUN_BASE_ADDR,
                                       flash_size))
               ? 0
               : -3;
}

/**
 * @brief 将 B 区新 APP 安装到内部运行区。
 *
 * 调用公共搬运接口，从 B 区读取已解密的有效明文。
 *
 * @return 安装结果，沿用公共搬运接口的状态值。
 * @retval 0 安装完成。
 * @retval -1 长度或后续读取无效。
 * @retval -2 擦除或写入回读失败。
 * @retval -3 首段读取或启动入口检查失败。
 */
int8_t exti_B_to_internal_APP(void)
{
    return exti_to_internal_APP(BLOCK_2);
}

/**
 * @brief 将 A 区旧 APP 备份恢复到内部运行区。
 *
 * 调用公共搬运接口，从 A 区读取本次已经完成的有效旧 APP 备份。
 *
 * @return 恢复结果，沿用公共搬运接口的状态值。
 * @retval 0 恢复完成。
 * @retval -1 长度或后续读取无效。
 * @retval -2 擦除或写入回读失败。
 * @retval -3 首段读取或启动入口检查失败。
 */
int8_t exti_A_to_internal_APP(void)
{
    return exti_to_internal_APP(BLOCK_1);
}

/**
 * @brief 保存并回读确认当前 APP 的明文长度。
 *
 * 将长度写到 EEPROM 0x05～0x08，等待 10 毫秒后回读比较。
 *
 * @param[in] app_size 已安装 APP 的实际明文字节数。
 *
 * @return 长度保存结果。
 * @retval 1 写入和回读比较完成。
 * @retval 0 写入、读取或比较失败。
 */
static uint8_t bootloader_save_app_size(uint32_t app_size)
{
    uint32_t read_app_size = 0U; /* EEPROM 回读的 APP 明文长度。 */

    if (0U == ee_write_bytes((uint8_t *)&app_size, 0x05U, sizeof(app_size)))
    {
        return 0U;
    }
    delay_ms(10U);
    if ((0U == ee_read_bytes((uint8_t *)&read_app_size, 0x05U,
                             sizeof(read_app_size))) ||
        (read_app_size != app_size))
    {
        return 0U;
    }
    return 1U;
}

/**
 * @brief 备份旧 APP，安装新 APP 后复位进入首次启动检测。
 *
 * 处理步骤：
 * 1. 读取旧 APP 长度，检查有效性后将旧 APP 备份到 A 区。
 * 2. 若有效旧 APP 的备份失败，尝试启动旧 APP 并返回。
 * 3. 将 B 区新 APP 安装到内部，并保存新 APP 的明文长度。
 * 4. 写入首次检测状态 0x33，等待 EEPROM 写入后软件复位。
 * 5. 安装或长度保存失败时，尝试恢复本次有效旧 APP 备份。
 * 没有有效旧 APP 时允许首次安装；仅使用本次成功完成的备份回滚。
 * 本函数的恢复路径不处理看门狗复位后的首次启动失败。
 *
 * @return 无；安装成功后软件复位，失败处理可能跳转 APP 或返回。
 */
static void bootloader_install_app(void)
{
    uint8_t backup_valid = 0U; /* 本次旧 APP 备份已经完整成功的标记。 */
    uint32_t app_size = 0U;   /* 当前旧 APP 的实际明文长度。 */
    int8_t install_state = 0; /* 当前安装或回滚操作的执行结果。 */

    /* 有有效旧 APP 和长度记录时，完整备份到 A 区。 */
    if ((0U != ee_read_bytes((uint8_t *)&app_size, 0x05U, sizeof(app_size))) &&
        (app_size >= 8U) && (app_size <= APP_RUN_SIZE) &&
        (app_size <= BLOCK_SIZE) &&
        (0U !=
         bootloader_check_app((const uint8_t *)APP_RUN_BASE_ADDR, app_size)))
    {
        install_state = internal_APP_to_exti_A((int32_t)app_size);
        if (0 != install_state)
        {
            elog_e(TAG, "Old APP backup failed: result=%d.\r\n",
                   (int)install_state);
            bootloader_jump_to_app();
            return;
        }
        backup_valid = 1U;
        elog_i(TAG, "Old APP backed up to A: size=%lu.\r\n",
               (unsigned long)app_size);
    }
    else
    {
        elog_w(TAG, "No valid old APP/size: "
                    "installing without rollback.\r\n");
    }

    /* 安装 B 区新 APP，记录明文长度后进入首次检测复位流程。 */
    install_state = exti_B_to_internal_APP();
    if (0 == install_state)
    {
        if (0U != bootloader_save_app_size(Read_BlockSize(BLOCK_2)))
        {
            uint8_t ota_state = APP_FIRST_CHECK; /* 待执行首次检测的状态。 */

            elog_i(TAG, "APP installed. Start first check.\r\n");

            /* 安装成功，标记需要首次检测，然后复位。 */
            ee_write_bytes(&ota_state, 0x00U, 1U);
            delay_ms(10U); /* 等待 EEPROM 完成写入。 */
            System_SoftwareReset();
        }
        else
        {
            elog_e(TAG, "Failed to save installed APP size.\r\n");
        }
    }
    else
    {
        elog_e(TAG, "APP installation failed: result=%d.\r\n",
               (int)install_state);
    }

    /* 仅使用本次有效旧 APP 备份回滚。 */
    if (0U != backup_valid)
    {
        install_state = exti_A_to_internal_APP();
        if (0 == install_state)
        {
            if (0U == bootloader_save_app_size(app_size))
            {
                elog_e(TAG, "Failed to restore old APP size record.\r\n");
            }
            elog_w(TAG, "Old APP restored. Launching APP.\r\n");
            bootloader_jump_to_app();
        }
        else
        {
            elog_e(TAG, "APP rollback failed: result=%d.\r\n",
                   (int)install_state);
        }
    }
}

/**
 * @brief 解密已有 A 区加密包，再执行备份、安装和首次检测复位。
 *
 * 先将 A 区密文解密到 B 区，成功后才允许备份覆盖 A 区。
 * 解密失败时尝试启动当前 APP；解密成功后进入安装及回滚流程。
 *
 * @param[in] firmware_size A 区完整加密包的实际字节数。
 *
 * @return 无；安装成功后软件复位，解密或安装失败可能跳转 APP 或返回。
 */
static void bootloader_update_app(int32_t firmware_size)
{
    int8_t install_state = exitA_to_extiB(firmware_size); /* 解密结果。 */

    /* 解密完成后才允许旧 APP 备份覆盖 A 区密文，并安装新 APP。 */
    if (0 != install_state)
    {
        elog_e(TAG, "Download/decrypt failed: size=%ld, result=%d.\r\n",
               (long)firmware_size, (int)install_state);
        bootloader_jump_to_app();
        return;
    }
    bootloader_install_app();
}

/**
 * @brief 接收新加密包并执行完整安装流程。
 *
 * 通过静态缓冲区接收 YMODEM 文件到 A 区，然后解密、备份及安装。
 * 安装成功后写入首次检测状态 0x33，并通过软件复位重新进入 Bootloader。
 * 下载或安装失败返回后，主循环可等待按键并重新发起下载。
 *
 * @return 无；安装成功后软件复位，失败处理可能跳转 APP 或返回。
 */
void bootloader_download_app(void)
{
    /* 1. 共用静态接收缓冲区，避免占用 1 KB 的启动栈。 */
    /* 本次接收的加密文件长度，失败时由安装流程处理返回值。 */
    int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

    bootloader_update_app(firmware_size);
}

/**
 * @brief 按 EEPROM 状态选择 APP 启动或固件升级路径。
 *
 * 处理步骤：
 * 1. 从 EEPROM 0x00 读取升级状态。
 * 2. 状态 0x00：按键有效时下载；否则检查入口，写入 RAM 请求并软复位。
 *    请求由下一次 main() 在外设初始化前消费，入口无效则返回等待下载。
 * 3. 状态 0x11：尝试启动旧 APP；返回后重新下载并完整安装。
 * 4. 状态 0x22：读取已有包长度，恢复 A 区管理参数并直接升级。
 * 5. 状态 0x33：写入 0x44，开启独立看门狗并启动已安装的 APP。
 * 6. 状态 0x44：上次启动未完成确认，清为 0x00 后软件复位。
 * APP 首次确认由 APP 写入 0x00 并等待看门狗复位完成。
 * 当前 0x44 分支仅清状态并复位，不执行旧 APP 恢复。
 * 读取失败或包长度无效时尝试启动当前 APP。
 * 状态处理返回后，主循环等待按键并提供重新下载。
 *
 * @return 无；成功进入 APP 后通常不会返回。
 */
void OTA_state_mangner(void)
{
    uint8_t flag_ota_state = NO_APP_UPDATE; /* EEPROM 保存的升级状态。 */
    uint32_t firmware_size = 0U; /* EEPROM 保存的加密包有效长度。 */

    if (0U == ee_read_bytes(&flag_ota_state, 0x00U, 1U))
    {
        elog_e(TAG, "Failed to read OTA state from EEPROM.\r\n");
        bootloader_jump_to_app();
        return;
    }

    elog_i(TAG, "Boot state=0x%02X\r\n", flag_ota_state);
    switch (flag_ota_state)
    {
        case NO_APP_UPDATE:
            if (KEY_PRESSED == key_scan())
            {
                bootloader_download_app();
            }
            else
            {
                if (0U != bootloader_check_app(
                              (const uint8_t *)APP_RUN_BASE_ADDR, APP_RUN_SIZE))
                {
                    /* 保留请求后复位，使下一次 main() 在外设初始化前跳转。 */
                    g_app_jump_request = 0x55AA55AAU;
                    System_SoftwareReset();
                }
                else
                {
                    elog_e(TAG, "No valid APP.\r\n");
                }
            }
            break;

        case APP_DOWNLOADING_FAULT:
            elog_e(TAG, "App download failed.\r\n");
            bootloader_jump_to_app();
            elog_w(TAG, "No valid APP. Download a new APP.\r\n");
            bootloader_download_app();
            break;

        case APP_DOWNLOAD_COMPLETE:
            /* 已有完整密文包，恢复 A 区长度后直接升级，无须重新下载。 */
            if (0U == ee_read_bytes((uint8_t *)&firmware_size, 0x01U,
                                    sizeof(firmware_size)))
            {
                elog_e(TAG, "Failed to read pending firmware size.\r\n");
                bootloader_jump_to_app();
                break;
            }
            if ((firmware_size <= 16U) || (firmware_size > APP_BACKUP_SIZE) ||
                (0U != (firmware_size % 16U)))
            {
                elog_e(TAG, "Invalid pending firmware size: %lu.\r\n",
                       (unsigned long)firmware_size);
                bootloader_jump_to_app();
                break;
            }
            SetBlockParmeter(BLOCK_1, firmware_size);
            bootloader_update_app((int32_t)firmware_size);
            break;

        case APP_FIRST_CHECK:
            flag_ota_state = APP_FIRST_CHECKING;
            ee_write_bytes(&flag_ota_state, 0x00U, 1U);
            delay_ms(10U);

            /* 开启看门狗，试运行已经安装的新 APP。 */
            IWDG_Init(IWDG_Prescaler_64, 3000);
            bootloader_jump_to_app();
            break;

        case APP_FIRST_CHECKING:
            /* 保留现有失败处理：清除首次确认标记后软件复位。 */
            flag_ota_state = NO_APP_UPDATE;
            ee_write_bytes(&flag_ota_state, 0x00, 1);
            System_SoftwareReset();
            break;

        default:
            elog_e(TAG, "Invalid OTA state: 0x%02X.\r\n",
                   (unsigned int)flag_ota_state);
            break;
    }
}
/************************* Function Definitions *************************/
