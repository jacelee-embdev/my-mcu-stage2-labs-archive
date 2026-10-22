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
 * @brief Provides the Bootloader peripheral-verification entry point.
 *
 * Processing flow:
 *
 * Initialize the key, LED, USART, delay, and logging modules; verify internal
 * Flash access; then report the key state through the LED, USART, and RTT log.
 *
 * @version V1.1 2026-09-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "main.h"
#include "bootloader.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG    "main"
/****************************** Defines ******************************/

/**
 * @brief Runs the Bootloader peripheral-verification program.
 *
 * Steps:
 * 1. Initializes the GPIO, USART, delay, and logging modules.
 * 2. Reports that the Bootloader is running.
 * 3. Erases the test sector and programs one 32-bit test word.
 * 4. Keeps the application jump disabled during peripheral verification.
 * 5. Reports the key state continuously through the LED and USART1.
 *
 * @return This function does not return during normal operation.
 */
int main(void)
{
    FLASH_Status flash_status = FLASH_ERROR_OPERATION;

    // 1. Initialize the hardware and software modules used by this test.
    key_init();
    led_init();
    USART1_Init();
    delay_init();
    debug_init();

    // 2. Report that the Bootloader peripheral test is running.
    elog_i(TAG, "Bootloader running...\r\n");

    // 3. Erase the test sector before programming one 32-bit test word.
    flash_status = flash_erase_app_sector(FLASH_Sector_3);
    if (FLASH_COMPLETE == flash_status)
    {
        flash_status = flash_write(0x0800C000U, 0x00000055U);
    }

    // 3.1 Report any Flash erase or programming failure.
    if (FLASH_COMPLETE != flash_status)
    {
        elog_e(TAG, "Flash test failed, status=%d\r\n", (int)flash_status);
    }

    // 4. Keep the application jump disabled during peripheral verification.
#if 0
    bootloader_jump_to_app();
#endif

    // 5. Report the key state through the LED, USART1, and RTT log.
    while (1)
    {
        elog_e(TAG, "elog TEST 1143!\r\n");

        if (KEY_PRESSED == key_scan())
        {
            USART_SendChar(USART1, 'A');
            LED_ON();
        }
        else
        {
            USART_SendChar(USART1, 'B');
            LED_OFF();
        }
    }
}
