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
 * @brief Declares APP image installation and application handover interfaces.
 *
 * Processing flow:
 *
 * Call copy_backup_to_app() to copy an image from the backup region into the
 * run region, then call bootloader_jump_to_app() to validate and start it.
 *
 * @version V1.1 2026-09-21
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
/* Internal Flash start addresses of the APP run and backup regions. */
#define APP_RUN_BASE_ADDR       ((uint32_t)0x08008000U)
#define APP_BACKUP_BASE_ADDR    ((uint32_t)0x08020000U)

/* Available capacity of the APP run and backup regions. */
#define APP_RUN_SIZE            ((uint32_t)0x00018000U)
#define APP_BACKUP_SIZE         ((uint32_t)0x00020000U)

/* Mask used to validate that the APP stack pointer is in internal SRAM. */
#define SRAM_ADDR_MASK          ((uint32_t)0x2FFE0000U)
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

/**
 * @brief Copies an APP image from the backup region to the run region.
 *
 * Steps:
 * 1. Validates that the firmware size fits in the APP run region.
 * 2. Programs each 32-bit word from the backup region into the run region.
 * 3. Reads back each programmed word and verifies its value.
 *
 * @param[in] firmware_size Size of the APP image in bytes.
 *
 * @return int8_t Result of the copy operation.
 * @retval  0 The APP image was copied successfully.
 * @retval -1 The firmware size is invalid.
 * @retval -2 Flash read-back verification failed.
 */
int8_t copy_backup_to_app(int32_t firmware_size);

/****************************** Declaring ******************************/

#endif /* End of __BOOTLOADER_H */
