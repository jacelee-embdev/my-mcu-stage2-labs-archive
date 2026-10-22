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
 * @brief Implements W25Qxx SPI flash identification, read, write, and erase.
 *
 * Processing flow:
 *
 * Initialize SPI1 and the chip-select GPIO before calling W25Qx_Init().
 * Then use the read, page-program, and erase APIs as required.
 *
 * @version V1.0 2026-10-02
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/************************ Includes ************************/
#include "w25qxx.h"
/************************ Includes ************************/

/************************ Defines ************************/
#define delay(ms)  HAL_Delay(ms) /* 毫秒延时。 */
#define get_tick() HAL_GetTick() /* 获取毫秒计数。 */
/************************ Defines ************************/

/************************ Global Variables ************************/
extern SPI_HandleTypeDef hspi1; /* 底层串行接口句柄。 */

W25Qx_Parameter W25Qx_Para; /* 初始化后保存器件容量参数。 */
/************************ Global Variables ************************/

/************************ Function Definitions ************************/

/**
 * @brief 复位器件并初始化全局容量参数。
 *
 * 处理步骤：
 * 1. 发送复位命令。
 * 2. 读取器件标识并填充全局参数结构体。
 *
 * @return 初始化结果。
 * @retval W25Qx_OK 参数读取成功。
 * @retval W25Qx_ERROR 器件标识不在支持范围内。
 */
uint8_t W25Qx_Init(void)
{
    /* 参数获取结果。 */
    uint8_t state;

    // 1. 复位器件。
    W25Qx_Reset();

    // 2. 获取容量参数并返回获取结果。
    state = W25Qx_Get_Parameter(&W25Qx_Para);

    return state;
}

/**
 * @brief 向器件发送复位使能和复位命令。
 *
 * 处理步骤：
 * 1. 拉低片选并连续发送两个复位命令字节。
 * 2. 拉高片选结束传输。
 *
 * @return 无返回值。
 */
static void W25Qx_Reset(void)
{
    /* 复位命令序列。 */
    uint8_t cmd[2] = {RESET_ENABLE_CMD, RESET_MEMORY_CMD};

    // 1. 拉低片选并发送复位命令序列。
    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 2, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();
}

/**
 * @brief 读取状态寄存器一并判断器件是否忙。
 *
 * 处理步骤：
 * 1. 发送状态寄存器一读取命令并接收一个字节。
 * 2. 根据忙标志返回器件状态。
 *
 * @return 器件忙状态。
 * @retval W25Qx_BUSY 忙标志置位。
 * @retval W25Qx_OK 忙标志清零。
 */
static uint8_t W25Qx_GetStatus(void)
{
    /* 状态寄存器一读取命令。 */
    uint8_t cmd[] = {READ_STATUS_REG1_CMD};
    /* 接收的状态寄存器值。 */
    uint8_t status;

    // 1. 拉低片选，读取状态寄存器一。
    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);

    HAL_SPI_Receive(&hspi1, &status, 1, W25QXXXX_TIMEOUT_VALUE);
    W25Qx_Disable();

    // 2. 检查忙标志并返回状态。
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
 *
 * 处理步骤：
 * 1. 发送写使能命令。
 * 2. 轮询忙标志，忙时延时一毫秒，并检查等待是否超时。
 *
 * @return 写使能命令发送后的等待结果。
 * @retval W25Qx_OK 器件已空闲。
 * @retval W25Qx_TIMEOUT 等待空闲超时。
 * @note 本函数轮询忙标志，未检查写使能锁存标志。
 */
uint8_t W25Qx_WriteEnable(void)
{
    /* 写使能命令。 */
    uint8_t cmd[] = {WRITE_ENABLE_CMD};
    /* 本次调用的超时计时起点。 */
    uint32_t tickstart = get_tick();

    // 1. 拉低片选并发送写使能命令。
    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);

    W25Qx_Disable();

    // 2. 等待器件空闲，忙时检查超时并延时。
    while (W25Qx_GetStatus() == W25Qx_BUSY)
    {
        if ((get_tick() - tickstart) > W25QXXXX_TIMEOUT_VALUE)
        {
            return W25Qx_TIMEOUT;
        }
        delay(1);
    }

    return W25Qx_OK;
}

/**
 * @brief 读取并组合厂商标识与器件标识。
 *
 * 处理步骤：
 * 1. 发送标识读取命令和三个零地址字节。
 * 2. 接收两个标识字节，以厂商标识为高字节组合结果。
 *
 * @param[out] ID 接收标识的指针，调用者须提供有效地址。
 *
 * @return 无返回值。
 * @note 支持的标识为 0xEF13～0xEF17，对应容量为 1～16 MiB。
 */
void W25Qx_Read_ID(uint16_t *ID)
{
    /* 厂商标识和器件标识字节。 */
    uint8_t idt[2];

    /* 标识读取命令序列。 */
    uint8_t cmd[4] = {READ_ID_CMD, 0x00, 0x00, 0x00};

    // 1. 发送标识读取命令并接收两个字节。
    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);

    HAL_SPI_Receive(&hspi1, idt, 2, W25QXXXX_TIMEOUT_VALUE);

    // 2. 将厂商标识与器件标识组合为 16 位结果。
    *ID = (idt[0] << 8) + idt[1];

    W25Qx_Disable();
}

/************************ Function Definitions ************************/

/************************ Includes ************************/
#include <math.h>
/************************ Includes ************************/

/************************ Function Definitions ************************/

/**
 * @brief 根据器件标识填写容量和存储组织参数。
 *
 * 处理步骤：
 * 1. 设置页、子扇区和扇区大小。
 * 2. 读取并检查器件标识范围。
 * 3. 计算总容量、子扇区数量和扇区数量。
 *
 * @param[out] Para 接收参数的结构体指针，调用者须提供有效地址。
 *
 * @return 参数获取结果。
 * @retval W25Qx_OK 参数填写完成。
 * @retval W25Qx_ERROR 器件标识不在支持范围内。
 */
uint8_t W25Qx_Get_Parameter(W25Qx_Parameter *Para)
{
    /* 组合后的器件标识。 */
    uint16_t id;
    /* 计算得到的总容量，单位为字节。 */
    uint32_t size;

    // 1. 设置页、子扇区和扇区的字节大小。
    Para->PAGE_SIZE = 256;
    Para->SUBSECTOR_SIZE = 4096;
    Para->SECTOR_SIZE = 0x10000;

    // 2. 读取标识并检查支持范围。
    W25Qx_Read_ID(&id);
    if (id < W25Q80 || id > W25Q128)
        return W25Qx_ERROR;

    // 3. 根据标识计算总容量及各类区域数量。
    size = (uint32_t) powf(2, (id - 0xEF13)) * 1024 * 1024;

    Para->FLASH_ID = id;
    Para->W25q_FLASH_SIZE = size;
    Para->SUBSECTOR_COUNT = Para->W25q_FLASH_SIZE / Para->SUBSECTOR_SIZE;
    Para->SECTOR_COUNT = Para->W25q_FLASH_SIZE / Para->SECTOR_SIZE;

    return W25Qx_OK;
}

/**
 * @brief 通过普通串行接口读取指定地址的数据。
 *
 * 处理步骤：
 * 1. 组装读取命令和三字节地址。
 * 2. 发送命令并将接收数据写入缓冲区。
 *
 * @param[out] pData 接收缓冲区指针，须有效且容量足够。
 * @param[in] ReadAddr 读取起始字节地址，仅发送低 24 位。
 * @param[in] Size 读取字节数，须符合底层接收接口的长度限制。
 *
 * @return 数据读取结果。
 * @retval W25Qx_OK 数据接收完成。
 * @retval W25Qx_ERROR 底层接收接口返回非零状态。
 */
uint8_t W25Qx_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
    /* 命令字节及三字节地址缓冲区。 */
    uint8_t cmd[4];

    // 1. 组装读取命令和三字节地址。
    cmd[0] = READ_CMD;
    cmd[1] = (uint8_t)(ReadAddr >> 16);
    cmd[2] = (uint8_t)(ReadAddr >> 8);
    cmd[3] = (uint8_t)(ReadAddr);

    // 2. 发送命令并接收指定长度的数据。
    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);

    if (HAL_SPI_Receive(&hspi1, pData, Size, W25QXXXX_TIMEOUT_VALUE) != 0x00)
    {
        return W25Qx_ERROR;
    }
    W25Qx_Disable();
    return W25Qx_OK;
}

/**
 * @brief 按页边界拆分并写入指定长度的数据。
 *
 * 处理步骤：
 * 1. 计算起始页可写长度和结束地址。
 * 2. 逐页发送写使能、页编程命令和数据，并等待器件空闲。
 * 3. 更新地址、数据指针和下一页写入长度。
 *
 * @param[in] pData 待写数据缓冲区指针，须有效且数据长度足够。
 * @param[in] WriteAddr 写入起始字节地址，仅发送低 24 位。
 * @param[in] Size 写入字节数，应大于零且地址范围有效。
 *
 * @return 分页写入结果。
 * @retval W25Qx_OK 各页写入及等待完成。
 * @retval W25Qx_ERROR 命令或数据发送返回非零状态。
 * @retval W25Qx_TIMEOUT 等待空闲时超过本次调用的超时限制。
 * @note 各页等待共用函数入口记录的起始时刻；写入前需完成擦除。
 */
uint8_t W25Qx_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
    /* 命令字节及三字节地址缓冲区。 */
    uint8_t cmd[4];
    /* 结束地址、本页长度和当前地址。 */
    uint32_t end_addr, current_size, current_addr;
    /* 本次调用的超时计时起点。 */
    uint32_t tickstart = get_tick();

    // 1. 计算起始地址所在页的剩余空间。
    current_addr = 0;

    while (current_addr <= WriteAddr)
    {
        current_addr += W25QXXXX_PAGE_SIZE;
    }
    current_size = current_addr - WriteAddr;

    // 2. 将首段长度限制为本次待写长度。
    if (current_size > Size)
    {
        current_size = Size;
    }

    // 3. 设置写入起始地址和结束地址。
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;

    // 4. 逐页执行编程并等待器件空闲。
    do
    {
        // 4.1 组装本页的编程命令和地址。
        cmd[0] = PAGE_PROG_CMD;
        cmd[1] = (uint8_t)(current_addr >> 16);
        cmd[2] = (uint8_t)(current_addr >> 8);
        cmd[3] = (uint8_t)(current_addr);

        // 4.2 发送写使能命令，然后发送编程命令和数据。
        W25Qx_WriteEnable();

        W25Qx_Enable();

        if (HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE) != 0x00)
        {
            return W25Qx_ERROR;
        }

        if (HAL_SPI_Transmit(&hspi1, pData, current_size,
                             W25QXXXX_TIMEOUT_VALUE) != 0x00)
        {
            return W25Qx_ERROR;
        }
        W25Qx_Disable();

        // 4.3 使用本次调用的计时起点检查等待超时。
        while (W25Qx_GetStatus() == W25Qx_BUSY)
        {
            if ((get_tick() - tickstart) > W25QXXXX_TIMEOUT_VALUE)
            {
                return W25Qx_TIMEOUT;
            }
        }

        // 4.4 更新下一页的地址、数据指针和写入长度。
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + W25QXXXX_PAGE_SIZE) > end_addr)
                       ? (end_addr - current_addr) : W25QXXXX_PAGE_SIZE;
    } while (current_addr < end_addr);

    return W25Qx_OK;
}

/**
 * @brief 发送擦除命令，擦除指定地址所在的 4 KiB 区域。
 *
 * 处理步骤：
 * 1. 组装擦除命令和三字节地址并发送写使能命令。
 * 2. 发送擦除命令，轮询忙标志直至空闲或超时。
 *
 * @param[in] Address 待擦除区域内的字节地址，仅发送低 24 位。
 *
 * @return 擦除等待结果。
 * @retval W25Qx_OK 器件已空闲。
 * @retval W25Qx_TIMEOUT 等待空闲超过区域擦除超时限制。
 * @note 擦除范围由命令字 0x20 决定；等待使用现有扇区超时宏。
 */
uint8_t W25Qx_Erase_Block(uint32_t Address)
{
    /* 命令字节及三字节地址缓冲区。 */
    uint8_t cmd[4];
    /* 本次调用的超时计时起点。 */
    uint32_t tickstart = get_tick();

    // 1. 组装 4 KiB 区域擦除命令和三字节地址。
    cmd[0] = SECTOR_ERASE_CMD;
    cmd[1] = (uint8_t)(Address >> 16);
    cmd[2] = (uint8_t)(Address >> 8);
    cmd[3] = (uint8_t)(Address);

    // 2. 发送写使能命令和区域擦除命令。
    W25Qx_WriteEnable();

    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);

    W25Qx_Disable();

    // 3. 等待擦除完成并检查超时。
    while (W25Qx_GetStatus() == W25Qx_BUSY)
    {
        if ((get_tick() - tickstart) > W25QXXXX_SECTOR_ERASE_MAX_TIME)
        {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}

/**
 * @brief 发送整片擦除命令并等待器件空闲。
 *
 * 处理步骤：
 * 1. 发送写使能命令和整片擦除命令。
 * 2. 轮询忙标志直至器件空闲或等待超时。
 *
 * @return 整片擦除等待结果。
 * @retval W25Qx_OK 器件已空闲。
 * @retval W25Qx_TIMEOUT 等待空闲超过整片擦除超时限制。
 */
uint8_t W25Qx_Erase_Chip(void)
{
    /* 命令字节及三字节地址缓冲区。 */
    uint8_t cmd[4];
    /* 本次调用的超时计时起点。 */
    uint32_t tickstart = get_tick();

    // 1. 设置整片擦除命令。
    cmd[0] = CHIP_ERASE_CMD;

    // 2. 发送写使能命令和整片擦除命令。
    W25Qx_WriteEnable();

    W25Qx_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);

    W25Qx_Disable();

    // 3. 等待整片擦除完成并检查超时。
    while (W25Qx_GetStatus() == W25Qx_BUSY)
    {
        if ((get_tick() - tickstart) > W25QXXXX_BULK_ERASE_MAX_TIME)
        {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}
/************************ Function Definitions ************************/
