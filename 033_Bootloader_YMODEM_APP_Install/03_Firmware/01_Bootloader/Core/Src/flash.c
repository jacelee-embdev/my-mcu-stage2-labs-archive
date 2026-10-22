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
 * @brief Implements internal Flash sector erase and word programming.
 *
 * Processing flow:
 *
 * Resolve the sectors covered by an image, erase each selected sector, and
 * program received data into Flash one 32-bit word at a time.
 *
 * @version V1.0 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "flash.h"
/****************************** Includes ******************************/

/****************************** Variables ******************************/
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
 * Resolves the first and last sector identifiers from the supplied address
 * range, then erases every matching sector in the sector table.
 *
 * @param[in] addr Start address of the Flash image region.
 * @param[in] size Image size in bytes.
 *
 * @return 0U when all selected sectors are erased; otherwise 1U.
 */
uint8_t Flash_erase(u32 addr, u32 size)
{
    uint32_t flash_start_sector = 0;
    uint32_t flash_end_sector = 0;

    flash_start_sector = STMFLASH_GetFlashSector(addr);
    flash_end_sector = STMFLASH_GetFlashSector(addr + size);

    for (uint8_t i = 0; i <= 12; i++)
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
 * @brief Resolves the SPL sector identifier containing an address.
 *
 * @param[in] addr Internal Flash address to classify.
 *
 * @return SPL sector identifier containing the supplied address.
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
 * @brief Unlocks the Flash controller and waits until it is ready.
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
 * @brief Erases one STM32F4 Flash sector.
 *
 * @param[in] FLASH_Sector SPL sector identifier to erase.
 *
 * @return Status reported by the STM32F4 SPL Flash driver.
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
 * @brief Programs one 32-bit word into internal Flash.
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
