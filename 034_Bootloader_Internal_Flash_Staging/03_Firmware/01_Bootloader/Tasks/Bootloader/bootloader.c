/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bootloader.c
 *
 * @par dependencies
 * - bootloader.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides APP image installation and application handover services.
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

/****************************** Includes ******************************/
#include "bootloader.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
typedef void (*app_entry_t)(void);
/****************************** Defines ******************************/

/**
 * @brief Jumps from the bootloader to the user application.
 *
 * Steps:
 * 1. Reads and validates the application stack pointer.
 * 2. Reads the application entry address from the vector table.
 * 3. Disables interrupts and relocates the vector table.
 * 4. Resets the clock configuration and loads the application stack pointer.
 * 5. Transfers execution to the application entry point.
 *
 * @return None. Normally does not return after a successful handover.
 */
void bootloader_jump_to_app(void)
{
    uint32_t app_sp = *(__IO uint32_t *)APP_RUN_BASE_ADDR;
    uint32_t app_entry_addr;
    app_entry_t app_entry;

    /* Step 1: Validate the application stack pointer. */
    if ((app_sp & SRAM_ADDR_MASK) != SRAM_BASE)
    {
        return;
    }

    /* Step 2: Read the application entry address from the vector table. */
    app_entry_addr = *(__IO uint32_t *)(APP_RUN_BASE_ADDR + 4U);
    app_entry = (app_entry_t)app_entry_addr;

    /* Step 3: Disable interrupts and relocate the vector table. */
    __disable_irq();
    SCB->VTOR = APP_RUN_BASE_ADDR;

    /* Step 4: Reset the clock and load the application stack pointer. */
    RCC_DeInit();
    __set_MSP(app_sp);

    /* Step 5: Transfer execution to the application entry point. */
    app_entry();
}

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
int8_t copy_backup_to_app(int32_t firmware_size)
{
    uint32_t run_flash_destination = APP_RUN_BASE_ADDR;
    uint32_t backup_flash_source = APP_BACKUP_BASE_ADDR;

    if ((0 >= firmware_size) || (APP_RUN_SIZE < firmware_size))
    {
        return -1;
    }

    for (uint32_t index = 0;
         index < firmware_size;
         index += 4)
    {
        Flash_Write(run_flash_destination,
                    *(uint32_t *)backup_flash_source);

        if (*(uint32_t *)run_flash_destination !=
            *(uint32_t *)backup_flash_source)
        {
            return -2;
        }

        run_flash_destination += 4U;
        backup_flash_source += 4U;
    }

    return 0;
}
