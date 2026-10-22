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
 * @brief Provides the handover from the bootloader to the user application.
 *
 * Processing flow:
 *
 * Validate the application stack pointer, prepare the execution environment,
 * and transfer execution to the application entry point.
 *
 * @version v1.0 2026-09-17
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
    uint32_t app_sp = *(__IO uint32_t *)APP_BASE_ADDR;
    uint32_t app_entry_addr;
    app_entry_t app_entry;

    /* Step 1: Validate the application stack pointer. */
    if ((app_sp & SRAM_ADDR_MASK) != SRAM_BASE)
    {
        return;
    }

    /* Step 2: Read the application entry address from the vector table. */
    app_entry_addr = *(__IO uint32_t *)(APP_BASE_ADDR + 4U);
    app_entry = (app_entry_t)app_entry_addr;

    /* Step 3: Disable interrupts and relocate the vector table. */
    __disable_irq();
    SCB->VTOR = APP_BASE_ADDR;

    /* Step 4: Reset the clock and load the application stack pointer. */
    RCC_DeInit();
    __set_MSP(app_sp);

    /* Step 5: Transfer execution to the application entry point. */
    app_entry();
}
