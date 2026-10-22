/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file w25qxx.h
 *
 * @par dependencies
 * - stm32f4xx_it.h
 * - main.h
 * - spi.h
 * - delay.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares W25Q flash commands, parameters, and access APIs.
 *
 * Processing flow:
 *
 * Initialize SPI1, call W25Qx_Init(), then use the read, write, and erase APIs.
 *
 * @version V1.0 2026-09-29
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __W25QXX_H__
#define __W25QXX_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Includes ******************************/
#include "stm32f4xx_it.h"
#include "main.h"
#include "spi.h"
#include "delay.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/* W25Q32 的容量与扇区参数。 */
#define W25QXXXX_FLASH_SIZE                  0x0400000
#define W25QXXXX_SECTOR_COUNT                0x40
#define W25QXXXX_SECTOR_SIZE                 0x10000
#define W25QXXXX_SUBSECTOR_COUNT             0x0400
#define W25QXXXX_SUBSECTOR_SIZE              0x1000
#define W25QXXXX_PAGE_SIZE                   0x100

#define x25Q80  0X13
#define x25Q16  0X14
#define x25Q32  0X15
#define x25Q64  0X16
#define x25Q128 0X17

#define W25QXXXX_DUMMY_CYCLES_READ           4
#define W25QXXXX_DUMMY_CYCLES_READ_QUAD      10

#define W25QXXXX_BULK_ERASE_MAX_TIME         250000
#define W25QXXXX_SECTOR_ERASE_MAX_TIME       3000
#define W25QXXXX_SUBSECTOR_ERASE_MAX_TIME    800
#define W25QXXXX_TIMEOUT_VALUE 1000

/* 复位命令。 */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* 器件识别命令。 */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* 数据读取命令。 */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* 写入使能与禁用命令。 */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* 状态寄存器命令。 */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* 页编程命令。 */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* 擦除命令。 */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* 状态寄存器标志位。 */
#define W25QXXXX_FSR_BUSY                    ((uint8_t)0x01) /* 忙标志。 */
#define W25QXXXX_FSR_WREN                    ((uint8_t)0x02) /* 写使能标志。 */
#define W25QXXXX_FSR_QE                      ((uint8_t)0x02) /* 四线使能标志。 */


/* 软件控制片选引脚的高低电平。 */
#define W25Qx_Enable()   GPIO_ResetBits(F_CS_GPIO_Port, F_CS_Pin)
#define W25Qx_Disable()  GPIO_SetBits(F_CS_GPIO_Port, F_CS_Pin)

/* 驱动接口的返回状态。 */
#define W25Qx_OK            ((uint8_t)0x00)
#define W25Qx_ERROR         ((uint8_t)0x01)
#define W25Qx_BUSY          ((uint8_t)0x02)
#define W25Qx_TIMEOUT       ((uint8_t)0x03)
/****************************** Defines ******************************/

/****************************** Types ******************************/
/* 保存器件识别结果及容量、擦除单元和页大小。 */
typedef struct
{
    uint32_t FLASH_Id;
    uint32_t FLASH_Size;
    uint32_t SECTOR_COUNT;
    uint32_t SECTOR_SIZE;
    uint32_t SUBSECTOR_COUNT;
    uint32_t SUBSECTOR_SIZE;
    uint32_t PAGE_SIZE;
} W25Qx_Parameter;
/****************************** Types ******************************/

/****************************** Variables ******************************/
/* 当前 W25Q 器件的容量参数。 */
extern W25Qx_Parameter W25Qx_Para;
/****************************** Variables ******************************/

/************************ Function Declarations ************************/
/**
 * @brief 复位 W25Q 器件并读取容量参数。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Init(void);

/**
 * @brief 发送器件复位命令。
 * @return 无。
 */
static void W25Qx_Reset(void);

/**
 * @brief 读取状态寄存器中的忙标志。
 * @return 器件忙状态或正常状态。
 */
static uint8_t W25Qx_GetStatus(void);

/**
 * @brief 使能器件写入并等待器件空闲。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_WriteEnable(void);

/**
 * @brief 读取制造商和器件标识。
 * @param[out] ID 接收标识的地址。
 * @return 无。
 */
void W25Qx_Read_ID(uint16_t *ID);

/**
 * @brief 从指定地址读取数据。
 * @param[out] pData 接收数据的缓冲区。
 * @param[in] ReadAddr 读取起始地址。
 * @param[in] Size 读取字节数。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);

/**
 * @brief 从指定地址写入数据。
 * @param[in] pData 待写入的数据缓冲区。
 * @param[in] WriteAddr 写入起始地址。
 * @param[in] Size 写入字节数。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);

/**
 * @brief 擦除指定地址所在的 4 KB 擦除单元。
 * @param[in] Address 擦除单元内的地址。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Erase_Block(uint32_t Address);

/**
 * @brief 擦除整个 W25Q 器件。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Erase_Chip(void);

/**
 * @brief 读取器件标识并填写容量参数。
 * @param[out] Para 接收器件参数的结构体地址。
 * @return 驱动接口的执行状态。
 */
uint8_t W25Qx_Get_Parameter(W25Qx_Parameter *Para);
/************************ Function Declarations ************************/

#ifdef __cplusplus
}
#endif

#endif /* End of __W25QXX_H__ */
