/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx.h
 *
 * @par dependencies
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares W25Qxx SPI flash parameters, commands, and driver APIs.
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

#ifndef __W25QXX_H
#define __W25QXX_H

#ifdef __cplusplus
extern "C" {
#endif

/************************ Includes ************************/
#include "main.h"
/************************ Includes ************************/

/************************ Flash Configuration ************************/
#define W25QXXXX_FLASH_SIZE               0x0400000 /* 默认容量为 4 MiB。 */
#define W25QXXXX_SECTOR_COUNT             0x40      /* 默认包含 64 个扇区。 */
#define W25QXXXX_SECTOR_SIZE              0x10000   /* 扇区大小为 64 KiB。 */
#define W25QXXXX_SUBSECTOR_COUNT          0x0400    /* 默认 1024 个子扇区。 */
#define W25QXXXX_SUBSECTOR_SIZE           0x1000    /* 子扇区大小为 4 KiB。 */
#define W25QXXXX_PAGE_SIZE                0x100     /* 每页包含 256 字节。 */

/************************ Device Identifiers ************************/
#define W25Q80                            0XEF13    /* 1 MiB 器件标识。 */
#define W25Q16                            0XEF14    /* 2 MiB 器件标识。 */
#define W25Q32                            0XEF15    /* 4 MiB 器件标识。 */
#define W25Q64                            0XEF16    /* 8 MiB 器件标识。 */
#define W25Q128                           0XEF17    /* 16 MiB 器件标识。 */

/************************ Read Dummy Cycles ************************/
#define W25QXXXX_DUMMY_CYCLES_READ        4         /* 普通读取空周期配置值。 */
#define W25QXXXX_DUMMY_CYCLES_READ_QUAD   10        /* 四线读取空周期配置值。 */

/************************ Timeout Definitions ************************/
#define W25QXXXX_BULK_ERASE_MAX_TIME      250000    /* 整片擦除超时（毫秒）。 */
#define W25QXXXX_SECTOR_ERASE_MAX_TIME    3000      /* 扇区擦除超时（毫秒）。 */
#define W25QXXXX_SUBSECTOR_ERASE_MAX_TIME 800       /* 子扇区擦除超时，毫秒。 */
#define W25QXXXX_TIMEOUT_VALUE            10000     /* 通信和等待超时，毫秒。 */

/************************ Reset Commands ************************/
#define RESET_ENABLE_CMD                  0x66      /* 复位使能命令。 */
#define RESET_MEMORY_CMD                  0x99      /* 器件复位命令。 */

/************************ QPI Mode Commands ************************/
#define ENTER_QPI_MODE_CMD                0x38      /* 进入四线指令模式。 */
#define EXIT_QPI_MODE_CMD                 0xFF      /* 退出四线指令模式。 */

/************************ Identification Commands ************************/
#define READ_ID_CMD                       0x90      /* 读取厂商和器件标识。 */
#define DUAL_READ_ID_CMD                  0x92      /* 双线标识读取命令。 */
#define QUAD_READ_ID_CMD                  0x94      /* 四线标识读取命令。 */
#define READ_JEDEC_ID_CMD                 0x9F      /* 读取标准器件标识。 */

/************************ Read Commands ************************/
#define READ_CMD                          0x03      /* 普通读取命令。 */
#define FAST_READ_CMD                     0x0B      /* 快速读取命令。 */
#define DUAL_OUT_FAST_READ_CMD            0x3B      /* 双线输出快速读取。 */
#define DUAL_INOUT_FAST_READ_CMD          0xBB      /* 双线输入输出快速读取。 */
#define QUAD_OUT_FAST_READ_CMD            0x6B      /* 四线输出快速读取。 */
#define QUAD_INOUT_FAST_READ_CMD          0xEB      /* 四线输入输出快速读取。 */

/************************ Write Enable Commands ************************/
#define WRITE_ENABLE_CMD                  0x06      /* 写使能命令。 */
#define WRITE_DISABLE_CMD                 0x04      /* 写禁止命令。 */

/************************ Status Register Commands ************************/
#define READ_STATUS_REG1_CMD              0x05      /* 读取状态寄存器一。 */
#define READ_STATUS_REG2_CMD              0x35      /* 读取状态寄存器二。 */
#define READ_STATUS_REG3_CMD              0x15      /* 读取状态寄存器三。 */

#define WRITE_STATUS_REG1_CMD             0x01      /* 写入状态寄存器一。 */
#define WRITE_STATUS_REG2_CMD             0x31      /* 写入状态寄存器二。 */
#define WRITE_STATUS_REG3_CMD             0x11      /* 写入状态寄存器三。 */

/************************ Program Commands ************************/
#define PAGE_PROG_CMD                     0x02      /* 页编程命令。 */
#define QUAD_INPUT_PAGE_PROG_CMD          0x32      /* 四线输入页编程。 */

/************************ Erase Commands ************************/
#define SECTOR_ERASE_CMD                  0x20      /* 擦除 4 KiB 区域。 */
#define CHIP_ERASE_CMD                    0xC7      /* 整片擦除命令。 */

/************************ Suspend and Resume Commands ************************/
#define PROG_ERASE_RESUME_CMD             0x7A      /* 恢复编程或擦除。 */
#define PROG_ERASE_SUSPEND_CMD            0x75      /* 挂起编程或擦除。 */

/************************ Status Register Masks ************************/
#define W25QXXXX_FSR_BUSY                 ((uint8_t)0x01) /* 忙标志掩码。 */
#define W25QXXXX_FSR_WREN                 ((uint8_t)0x02) /* 写使能标志掩码。 */
/* 四线使能标志掩码。 */
#define W25QXXXX_FSR_QE                   ((uint8_t)0x02)

/************************ Chip Select Macros ************************/
/* 拉低片选，选中器件。 */
#define W25Qx_Enable() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
/* 拉高片选，取消选中。 */
#define W25Qx_Disable() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

/************************ Return Status Definitions ************************/
#define W25Qx_OK                          ((uint8_t)0x00) /* 操作完成或空闲。 */
#define W25Qx_ERROR                       ((uint8_t)0x01) /* 操作错误。 */
#define W25Qx_BUSY                        ((uint8_t)0x02) /* 器件忙。 */
#define W25Qx_TIMEOUT                     ((uint8_t)0x03) /* 等待超时。 */

/************************ Type Definitions ************************/
/* 器件容量和存储组织参数。 */
typedef struct
{
    uint32_t FLASH_ID;          /* 器件标识。 */
    uint32_t W25q_FLASH_SIZE;   /* 总容量，单位为字节。 */
    uint32_t SECTOR_COUNT;      /* 扇区数量。 */
    uint32_t SECTOR_SIZE;       /* 扇区大小，单位为字节。 */
    uint32_t SUBSECTOR_COUNT;   /* 子扇区数量。 */
    uint32_t SUBSECTOR_SIZE;    /* 子扇区大小，单位为字节。 */
    uint32_t PAGE_SIZE;         /* 页大小，单位为字节。 */
} W25Qx_Parameter;

/************************ Global Variable Declarations ************************/
extern W25Qx_Parameter W25Qx_Para; /* 全局器件参数。 */

/************************ Function Declarations ************************/

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
uint8_t W25Qx_Init(void);

/**
 * @brief 向器件发送复位使能和复位命令。
 *
 * 处理步骤：
 * 1. 拉低片选并连续发送两个复位命令字节。
 * 2. 拉高片选结束传输。
 *
 * @return 无返回值。
 */
static void W25Qx_Reset(void);

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
static uint8_t W25Qx_GetStatus(void);

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
uint8_t W25Qx_WriteEnable(void);

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
void W25Qx_Read_ID(uint16_t *ID);

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
uint8_t W25Qx_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);

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
uint8_t W25Qx_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);

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
uint8_t W25Qx_Erase_Block(uint32_t Address);

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
uint8_t W25Qx_Erase_Chip(void);

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
uint8_t W25Qx_Get_Parameter(W25Qx_Parameter *Para);

/************************ Function Declarations ************************/

#ifdef __cplusplus
}
#endif

#endif /* 结束头文件重复包含保护。 */
