/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file main.c
 *
 * @par dependencies
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides the application entry point and logging demonstration.
 *
 * Processing flow:
 *
 * Initialize the debug logging module, then continuously output test messages
 * through SEGGER RTT and EasyLogger.
 *
 * @version v1.0 2026-09-17
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG     "main"
/****************************** Defines ******************************/

/**
 * @brief Runs the application entry point.
 *
 * Steps:
 * 1. Initializes EasyLogger through the debug module.
 * 2. Continuously outputs SEGGER RTT and EasyLogger test messages.
 *
 * @return This function does not return during normal operation.
 */
int main(void)
{
    debug_init();
    
    while (1)
    {
        SEGGER_RTT_printf(0,"RTT TEST 1142!\r\n");
        elog_e(TAG,"elog TEST 1143!\r\n");
    }
}
