/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file flash.c
 *
 * @par dependencies
 * - flash.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides internal Flash sector erase and word-programming services.
 *
 * Processing flow:
 *
 * Call Flash_erase() to erase the sectors covered by an image range, then
 * call Flash_Write() to program image data one 32-bit word at a time.
 *
 * @version V1.1 2026-09-21
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "flash.h"
/****************************** Includes ******************************/

/****************************** Variables ******************************/
/* STM32F4 Flash-sector identifiers in ascending address order. */
const uint32_t Flash_Sectorsize[12] =
{
    FLASH_Sector_0,
    FLASH_Sector_1,
    FLASH_Sector_2,
    FLASH_Sector_3,
    FLASH_Sector_4,
    FLASH_Sector_5,
    FLASH_Sector_6,
    FLASH_Sector_7,
    FLASH_Sector_8,
    FLASH_Sector_9,
    FLASH_Sector_10,
    FLASH_Sector_11,
};
/****************************** Variables ******************************/

/********************** Private Function Declarations **********************/
static uint16_t STMFLASH_GetFlashSector(u32 addr);
/********************** Private Function Declarations **********************/

/**
 * @brief Erases every Flash sector covered by an image range.
 *
 * Steps:
 * 1. Rejects an empty image range.
 * 2. Resolves the first and last sectors covered by the image.
 * 3. Erases each sector in the resolved range.
 *
 * @param[in] addr Start address of the Flash image region.
 * @param[in] size Image size in bytes.
 *
 * @return uint8_t Result of the erase operation.
 * @retval 0U All selected sectors were erased successfully.
 * @retval 1U The image size is zero or a sector erase failed.
 */
uint8_t Flash_erase(u32 addr, u32 size)
{
    uint32_t flash_start_sector = 0;
    uint32_t flash_end_sector = 0;

    if (0U == size)
    {
        return 1U;
    }

    flash_start_sector = STMFLASH_GetFlashSector(addr);
    flash_end_sector = STMFLASH_GetFlashSector(addr + size - 1U);

    for (uint8_t i = 0U;
         i < (sizeof(Flash_Sectorsize) / sizeof(Flash_Sectorsize[0]));
         i++)
    {
        if ((Flash_Sectorsize[i] >= flash_start_sector) &&
            (Flash_Sectorsize[i] <= flash_end_sector))
        {
            if (FLASH_COMPLETE != EreaseAppSector(Flash_Sectorsize[i]))
            {
                return 1;
            }
        }
    }

    return 0;
}

/**
 * @brief Resolves the Flash sector containing a specified address.
 *
 * @param[in] addr Internal Flash address to classify.
 *
 * @return uint16_t SPL sector identifier containing the supplied address.
 */
static uint16_t STMFLASH_GetFlashSector(u32 addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
    {
        return FLASH_Sector_0;
    }
    else if (addr < ADDR_FLASH_SECTOR_2)
    {
        return FLASH_Sector_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_3)
    {
        return FLASH_Sector_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_4)
    {
        return FLASH_Sector_3;
    }
    else if (addr < ADDR_FLASH_SECTOR_5)
    {
        return FLASH_Sector_4;
    }
    else if (addr < ADDR_FLASH_SECTOR_6)
    {
        return FLASH_Sector_5;
    }
    else if (addr < ADDR_FLASH_SECTOR_7)
    {
        return FLASH_Sector_6;
    }
    else if (addr < ADDR_FLASH_SECTOR_8)
    {
        return FLASH_Sector_7;
    }
    else if (addr < ADDR_FLASH_SECTOR_9)
    {
        return FLASH_Sector_8;
    }
    else if (addr < ADDR_FLASH_SECTOR_10)
    {
        return FLASH_Sector_9;
    }
    else if (addr < ADDR_FLASH_SECTOR_11)
    {
        return FLASH_Sector_10;
    }

    return FLASH_Sector_11;
}

/**
 * @brief Unlocks the Flash controller and waits until it becomes ready.
 *
 * @return None.
 */
void Flash_Unlock(void)
{
    FLASH_Unlock();

    while (FLASH_BUSY == FLASH_GetStatus())
    {
    }
}

/**
 * @brief Locks the Flash controller.
 *
 * @return None.
 */
void Flash_Lock(void)
{
    FLASH_Lock();
}

/**
 * @brief Erases one STM32F4 internal Flash sector.
 *
 * @param[in] FLASH_Sector SPL sector identifier to erase.
 *
 * @return FLASH_Status Status reported by the STM32F4 SPL Flash driver.
 */
FLASH_Status EreaseAppSector(uint32_t FLASH_Sector)
{
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    Flash_Unlock();
    FLASHStatus = FLASH_EraseSector(FLASH_Sector, VoltageRange_3);
    Flash_Lock();

    return FLASHStatus;
}

/**
 * @brief Programs one 32-bit word into the internal Flash.
 *
 * @param[in] address Destination Flash address.
 * @param[in] data    Word to program.
 *
 * @return None.
 */
void Flash_Write(uint32_t address, uint32_t data)
{
    FLASH_Status status = FLASH_COMPLETE;

    Flash_Unlock();
    status = FLASH_ProgramWord(address, data);

    if (FLASH_COMPLETE == status)
    {
        /* The requested word was programmed successfully. */
    }
    else
    {
        /* The caller verifies the programmed word after this function. */
    }

    Flash_Lock();
}
