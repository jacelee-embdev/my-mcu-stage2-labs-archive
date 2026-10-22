/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx.c
 *
 * @par dependencies
 * - w25qxx.h
 * - math.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements W25Q flash identification and data access.
 *
 * Processing flow:
 *
 * Initialize SPI1, call W25Qx_Init(), then read, write, or erase flash data.
 *
 * @version V1.0 2026-09-29
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "w25qxx.h"
/****************************** Includes ******************************/

/****************************** Variables ******************************/
/* 保存当前器件的容量与擦除参数。 */
W25Qx_Parameter W25Qx_Para;
/****************************** Variables ******************************/

/************************ Function Definitions ************************/
/**
 * @brief 复位 W25Q 器件并读取其容量参数。
 *
 * 发送复位命令，等待器件稳定后读取并保存参数。
 *
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Init(void)
{
    // 保存参数读取结果。
    uint8_t state;

    // 1. 复位器件并等待其稳定。
    W25Qx_Reset();
    delay_ms(5);

    // 2. 读取器件参数并返回结果。
    state = W25Qx_Get_Parameter(&W25Qx_Para);
    return state;
}

/**
 * @brief 发送 W25Q 复位命令。
 * @return 无。
 */
static void W25Qx_Reset(void)
{
    // 复位使能和器件复位命令。
    uint8_t cmd[2] = {RESET_ENABLE_CMD, RESET_MEMORY_CMD};

    // 保持片选有效并发送两条命令。
    W25Qx_Enable();
    SPI1_WriteByte(cmd, 2, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();
}

/**
 * @brief 读取状态寄存器中的忙标志。
 * @return 器件忙状态或正常状态。
 */
static uint8_t W25Qx_GetStatus(void)
{
    // 状态读取命令及返回值。
    uint8_t cmd[] = {READ_STATUS_REG1_CMD};
    uint8_t status;

    // 1. 读取状态寄存器 1。
    W25Qx_Enable();
    SPI1_WriteByte(cmd, 1, W25QXXXX_TIMEOUT_VALUE);
    SPI1_ReadByte(&status, 1, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();

    // 2. 根据忙标志返回当前状态。
    if ((status & W25QXXXX_FSR_BUSY) != 0)
    {
        return W25Qx_BUSY;
    }
    else
    {
        return W25Qx_OK;
    }
}

/**
 * @brief 发送写使能命令并等待器件空闲。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_WriteEnable(void)
{
    // 写使能命令与等待起始时刻。
    uint8_t cmd[] = {WRITE_ENABLE_CMD};
    uint32_t tickstart = SysTickUpdateTime;

    // 1. 选中器件并发送写使能命令。
    W25Qx_Enable();
    SPI1_WriteByte(cmd, 1, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();

    // 2. 等待器件空闲或超时。
    while (W25Qx_GetStatus() == W25Qx_BUSY)
    {
        if ((SysTickUpdateTime - tickstart) > W25QXXXX_TIMEOUT_VALUE)
        {
            return W25Qx_TIMEOUT;
        }
        delay_ms(1);
    }

    return W25Qx_OK;
}

/**
 * @brief 读取制造商和器件标识。
 *
 * 常见返回值：0xEF13、0xEF14、0xEF15、0xEF16，分别对应
 * W25Q80、W25Q16、W25Q32 和 W25Q64。
 *
 * @param[out] ID 接收标识的地址。
 * @return 无。
 */
void W25Qx_Read_ID(uint16_t *ID)
{
    // 两字节标识及读取命令。
    uint8_t idt[2];
    uint8_t cmd[4] = {READ_ID_CMD, 0x00, 0x00, 0x00};

    // 1. 发送标识读取命令并接收两个字节。
    W25Qx_Enable();
    SPI1_WriteByte(cmd, 4, W25QXXXX_TIMEOUT_VALUE);
    SPI1_ReadByte(idt, 2, W25QXXXX_TIMEOUT_VALUE);

    // 2. 拼接标识并释放片选。
    *ID = (idt[0] << 8) + idt[1];
    W25Qx_Disable();
}

/****************************** Includes ******************************/
#include <math.h>
/****************************** Includes ******************************/

/**
 * @brief 读取器件标识并填写容量参数。
 * @param[out] Para 接收器件参数的结构体地址。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Get_Parameter(W25Qx_Parameter *Para)
{
    // 器件标识、容量编码及计算后的容量。
    uint16_t id;
    uint8_t device_id;
    uint32_t size;

    // 1. 填写固定的页和擦除单元大小。
    Para->PAGE_SIZE = 256;
    Para->SUBSECTOR_SIZE = 4096;
    Para->SECTOR_SIZE = 0x10000;

    // 2. 读取标识并检查器件容量编码。
    W25Qx_Read_ID(&id);
    device_id = id & 0xff;
    if (device_id < x25Q80 || device_id > x25Q128) return W25Qx_ERROR;

    // 3. 计算总容量及擦除单元数量。
    size = (uint32_t) powf(2, (device_id - x25Q80)) * 1024 * 1024;

    Para->FLASH_Id = id;
    Para->FLASH_Size = size;
    Para->SUBSECTOR_COUNT = Para->FLASH_Size / Para->SUBSECTOR_SIZE;
    Para->SECTOR_COUNT = Para->FLASH_Size / Para->SECTOR_SIZE;

    return W25Qx_OK;
}
/**
 * @brief 从指定地址读取数据。
 * @param[out] pData 接收数据的缓冲区。
 * @param[in] ReadAddr 读取起始地址。
 * @param[in] Size 读取字节数。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
    // 四字节读取命令及地址。
    uint8_t cmd[4];

    // 1. 组装读取命令和 24 位地址。
    cmd[0] = READ_CMD;
    cmd[1] = (uint8_t)(ReadAddr >> 16);
    cmd[2] = (uint8_t)(ReadAddr >> 8);
    cmd[3] = (uint8_t)(ReadAddr);

    // 2. 发送命令并读取指定数量的数据。
    W25Qx_Enable();
    SPI1_WriteByte(cmd, 4, W25QXXXX_TIMEOUT_VALUE);
    if (SPI1_ReadByte(pData, Size, W25QXXXX_TIMEOUT_VALUE) != 1)
    {
        return W25Qx_ERROR;
    }
    W25Qx_Disable();
    return W25Qx_OK;
}

/**
 * @brief 从指定地址开始按页写入数据。
 * @param[in] pData 待写入的数据缓冲区。
 * @param[in] WriteAddr 写入起始地址。
 * @param[in] Size 写入字节数。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
    // 页编程命令、写入边界及等待起始时刻。
    uint8_t cmd[4];
    uint32_t end_addr, current_size, current_addr;
    uint32_t tickstart = SysTickUpdateTime;

    // 1. 计算首个写入页的剩余空间。
    current_addr = 0;

    while (current_addr <= WriteAddr)
    {
        current_addr += W25QXXXX_PAGE_SIZE;
    }
    current_size = current_addr - WriteAddr;

    if (current_size > Size)
    {
        current_size = Size;
    }

    // 2. 设置写入区间。
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;

    // 3. 按页发送编程命令和数据。
    do
    {
        // 3.1 组装页编程命令和当前地址。
        cmd[0] = PAGE_PROG_CMD;
        cmd[1] = (uint8_t)(current_addr >> 16);
        cmd[2] = (uint8_t)(current_addr >> 8);
        cmd[3] = (uint8_t)(current_addr);

        // 3.2 使能写入并发送命令。
        W25Qx_WriteEnable();

        W25Qx_Enable();
        if (SPI1_WriteByte(cmd, 4, W25QXXXX_TIMEOUT_VALUE) != 1)
        {
            return W25Qx_ERROR;
        }

        // 3.3 发送本页数据。
        if (SPI1_WriteByte(pData, current_size,
                           W25QXXXX_TIMEOUT_VALUE) != 1)
        {
            return W25Qx_ERROR;
        }
        W25Qx_Disable();

        // 3.4 等待页编程完成或超时。
        while (W25Qx_GetStatus() == W25Qx_BUSY)
        {
            if ((SysTickUpdateTime - tickstart) > W25QXXXX_TIMEOUT_VALUE)
            {
                return W25Qx_TIMEOUT;
            }
        }

        // 3.5 更新下一页的地址、数据指针和写入长度。
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + W25QXXXX_PAGE_SIZE) > end_addr)
                       ? (end_addr - current_addr) : W25QXXXX_PAGE_SIZE;
    } while (current_addr < end_addr);

    return W25Qx_OK;
}

/**
 * @brief 擦除指定地址所在的 4 KB 擦除单元。
 * @param[in] Address 擦除单元内的地址。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Erase_Block(uint32_t Address)
{
    // 擦除命令及等待起始时刻。
    uint8_t cmd[4];
    uint32_t tickstart = SysTickUpdateTime;

    // 1. 组装擦除命令和 24 位地址。
    cmd[0] = SECTOR_ERASE_CMD;
    cmd[1] = (uint8_t)(Address >> 16);
    cmd[2] = (uint8_t)(Address >> 8);
    cmd[3] = (uint8_t)(Address);

    // 2. 使能写入并发送擦除命令。
    W25Qx_WriteEnable();

    W25Qx_Enable();
    SPI1_WriteByte(cmd, 4, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();

    // 3. 等待擦除完成或超时。
    while (W25Qx_GetStatus() == W25Qx_BUSY)
    {
        if ((SysTickUpdateTime - tickstart) > W25QXXXX_SECTOR_ERASE_MAX_TIME)
        {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}

/**
 * @brief 擦除整个 W25Q 器件。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Erase_Chip(void)
{
    // 整片擦除命令及等待起始时刻。
    uint8_t cmd[4];
    uint32_t tickstart = SysTickUpdateTime;

    // 1. 设置整片擦除命令并使能写入。
    cmd[0] = CHIP_ERASE_CMD;

    W25Qx_WriteEnable();

    // 2. 发送整片擦除命令。
    W25Qx_Enable();
    SPI1_WriteByte(cmd, 1, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();

    // 3. 等待擦除完成或超时。
    while (W25Qx_GetStatus() == W25Qx_BUSY)
    {
        if ((SysTickUpdateTime - tickstart) > W25QXXXX_BULK_ERASE_MAX_TIME)
        {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}

/************************ Function Definitions ************************/
