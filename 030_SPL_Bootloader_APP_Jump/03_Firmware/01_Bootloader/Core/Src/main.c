/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file main.c
 *
 * @par dependencies
 * - main.h
 * - bootloader.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides the bootloader entry point and application handover.
 *
 * Processing flow:
 *
 * Initialize debug logging, report the bootloader state, and attempt to jump
 * to the user application. If control remains in the bootloader, output
 * diagnostic logs.
 *
 * @version v1.0 2026-09-17
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "main.h"
#include "bootloader.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG     "main"
/****************************** Defines ******************************/

/**
 * @brief Runs the bootloader entry point.
 *
 * Steps:
 * 1. Initializes EasyLogger through the debug module.
 * 2. Reports that the bootloader is running.
 * 3. Attempts to jump to the user application.
 * 4. Outputs diagnostic logs if control remains in the bootloader.
 *
 * @return This function does not return during normal operation.
 */
int main(void)
{
    /* Step 1: Initialize the debug logging module. */
    debug_init();

    /* Step 2: Report that the bootloader is running. */
    elog_i(TAG, "Bootloader running...\r\n");

    /* Step 3: Attempt to jump to the user application. */
    bootloader_jump_to_app();

    /* Step 4: Output logs if control remains in the bootloader. */
    while (1)
    {
        SEGGER_RTT_printf(0, "RTT TEST 1142!\r\n");
        elog_e(TAG, "elog TEST 1143!\r\n");
    }
}
