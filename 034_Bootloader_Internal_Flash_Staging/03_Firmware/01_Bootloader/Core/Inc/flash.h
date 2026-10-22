/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file flash.h
 *
 * @par dependencies
 * - stm32f4xx.h
 * - stm32f4xx_flash.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares internal Flash sector erase and word-programming services.
 *
 * Processing flow:
 *
 * Use Flash_erase() to erase every sector covered by an image, then call
 * Flash_Write() for each 32-bit word that must be programmed.
 *
 * @version V1.0 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __FLASH_H__
#define __FLASH_H__

/****************************** Includes ******************************/
#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) /* 16-Kbyte sector. */
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) /* 16-Kbyte sector. */
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) /* 16-Kbyte sector. */
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) /* 16-Kbyte sector. */
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) /* 64-Kbyte sector. */
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) /* 128-Kbyte sector. */
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) /* 128-Kbyte sector. */
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) /* 128-Kbyte sector. */
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) /* 128-Kbyte sector. */
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) /* 128-Kbyte sector. */
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) /* 128-Kbyte sector. */
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) /* 128-Kbyte sector. */
/****************************** Defines ******************************/

/****************************** Declaring ******************************/

/**
 * @brief Erases every Flash sector covered by an image range.
 *
 * @param[in] addr Start address of the Flash image region.
 * @param[in] size Image size in bytes.
 *
 * @return 0U when all selected sectors are erased; otherwise 1U.
 */
uint8_t      Flash_erase           (u32 addr, u32 size);

/**
 * @brief Erases one STM32F4 Flash sector.
 *
 * @param[in] FLASH_Sector SPL sector identifier to erase.
 *
 * @return Status reported by the STM32F4 SPL Flash driver.
 */
FLASH_Status EreaseAppSector       (uint32_t FLASH_Sector);

/**
 * @brief Programs one 32-bit word into internal Flash.
 *
 * @param[in] address Destination Flash address.
 * @param[in] data    Word to program.
 *
 * @return None.
 */
void         Flash_Write           (uint32_t address, uint32_t data);

/****************************** Declaring ******************************/

#endif /* End of __FLASH_H__ */
