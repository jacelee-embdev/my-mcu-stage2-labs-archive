/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bootloader.h
 *
 * @par dependencies
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the interface for jumping to the user application.
 *
 * Processing flow:
 *
 * Call bootloader_jump_to_app() to validate the application stack pointer,
 * configure the execution environment, and jump to the application.
 *
 * @version v1.0 2026-09-17
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

/****************************** Includes ******************************/
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define APP_BASE_ADDR      ((uint32_t)0x08008000U)
#define SRAM_ADDR_MASK     ((uint32_t)0x2FFE0000U)
/****************************** Defines ******************************/

/****************************** Declaring ******************************/

/**
 * @brief Jumps from the bootloader to the user application.
 *
 * Validates the application stack pointer, disables interrupts, relocates the
 * vector table, resets the clock configuration, loads the application stack
 * pointer, and transfers execution to the application entry point.
 *
 * @return None. Normally does not return after a successful handover.
 */
void bootloader_jump_to_app(void);

/****************************** Declaring ******************************/

#endif /* End of __BOOTLOADER_H */
