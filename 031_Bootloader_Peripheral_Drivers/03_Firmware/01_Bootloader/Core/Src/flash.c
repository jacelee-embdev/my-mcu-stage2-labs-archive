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
 * Call flash_erase_app_sector() before using flash_write() to program a word
 * into a sector that contains existing data.
 *
 * @version V1.0 2026-09-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "flash.h"
/****************************** Includes ******************************/

/********************** Private Function Declarations **********************/
static void flash_unlock(void);
static void flash_lock(void);
/********************** Private Function Declarations **********************/

/**
 * @brief Unlocks the internal Flash controller.
 *
 * Unlocks the Flash control register and waits for any ongoing Flash
 * operation to finish.
 *
 * @param None.
 *
 * @return None.
 */
static void flash_unlock(void)
{
    // 1. Unlock access to the Flash control register.
    FLASH_Unlock();

    // 2. Wait until the Flash controller is no longer busy.
    while (FLASH_BUSY == FLASH_GetStatus())
    {
    }
}

/**
 * @brief Locks the internal Flash controller.
 *
 * Prevents unintended writes to the Flash control register.
 *
 * @param None.
 *
 * @return None.
 */
static void flash_lock(void)
{
    // 1. Lock access to the Flash control register.
    FLASH_Lock();
}

/**
 * @brief Erases one application Flash sector.
 *
 * Unlocks the Flash controller, erases the specified sector with voltage
 * range 3, and locks the Flash controller again.
 *
 * @param[in] flash_sector SPL sector identifier, such as FLASH_Sector_3.
 *
 * @return FLASH_Status Result reported by the SPL Flash driver.
 * @retval FLASH_COMPLETE The sector was erased successfully.
 * @retval Other          The SPL Flash driver reported an error.
 */
FLASH_Status flash_erase_app_sector(uint32_t flash_sector)
{
    FLASH_Status flash_status = FLASH_ERROR_OPERATION;

    // 1. Unlock the Flash controller before erasing the sector.
    flash_unlock();

    // 2. Erase the selected sector for a 2.7 V to 3.6 V supply.
    flash_status = FLASH_EraseSector(flash_sector, VoltageRange_3);

    // 3. Lock the Flash controller after the erase operation.
    flash_lock();

    return flash_status;
}

/**
 * @brief Programs one 32-bit word into internal Flash.
 *
 * Unlocks the Flash controller, programs one word at the specified address,
 * and locks the Flash controller again.
 *
 * @param[in] address Destination address. It must be valid and word-aligned.
 * @param[in] data    The 32-bit word to program.
 *
 * @return FLASH_Status Result reported by the SPL Flash driver.
 * @retval FLASH_COMPLETE The word was programmed successfully.
 * @retval Other          The SPL Flash driver reported an error.
 */
FLASH_Status flash_write(uint32_t address, uint32_t data)
{
    FLASH_Status flash_status = FLASH_ERROR_OPERATION;

    // 1. Unlock the Flash controller before programming the word.
    flash_unlock();

    // 2. Program one 32-bit word at the destination address.
    flash_status = FLASH_ProgramWord(address, data);

    // 3. Lock the Flash controller after the programming operation.
    flash_lock();

    return flash_status;
}
