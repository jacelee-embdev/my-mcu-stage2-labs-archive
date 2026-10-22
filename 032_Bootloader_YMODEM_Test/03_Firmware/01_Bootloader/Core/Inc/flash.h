/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file flash.h
 *
 * @par dependencies
 * - stm32f4xx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the interfaces for erasing and programming internal Flash.
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

#ifndef __FLASH_H__
#define __FLASH_H__

/****************************** Includes ******************************/
#include "stm32f4xx.h"
/****************************** Includes ******************************/

/************************ Function Declarations ************************/
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
FLASH_Status flash_erase_app_sector(uint32_t flash_sector);

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
FLASH_Status flash_write(uint32_t address, uint32_t data);
/************************ Function Declarations ************************/

#endif /* End of __FLASH_H__ */
