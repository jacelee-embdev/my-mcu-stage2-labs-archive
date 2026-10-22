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
 * @brief Provides the Bootloader entry point and firmware update loop.
 *
 * Processing flow:
 *
 * Initialize the Bootloader services, receive an APP image into W25Q
 * when requested, install it in internal flash, and retry on request.
 *
 * @version v1.6 2026-09-30
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG                           "main"
#define YMODEM_RECEIVE_BUFFER_SIZE    (1024U)
/****************************** Defines ******************************/

/****************************** Variables ******************************/
static uint8_t g_ymodem_receive_buffer[YMODEM_RECEIVE_BUFFER_SIZE] = {0U};
/****************************** Variables ******************************/

/**
 * @brief Runs the Bootloader firmware-update process.
 *
 * Steps:
 * 1. Initializes GPIO, USART, delay, logging, SPI1, and W25Q services.
 * 2. Reports that the Bootloader is ready for a firmware transfer.
 * 3. Checks whether the update key is pressed.
 * 4. Receives the APP image into external W25Q flash when requested.
 * 5. Installs the image in the internal APP run region and starts it.
 * 6. Waits for another update request if the APP was not started.
 *
 * @return This function does not return during normal operation.
 */
int main(void)
{
    // 1. Initialize the hardware and software used by the Bootloader.
    key_init();
    led_init();
    USART1_Init();
    delay_init();
    debug_init();
    SPI1_Init();
    W25Q64_Init();

    // 2. Report that the Bootloader is ready to receive the APP image.
    elog_i(TAG, "Bootloader running...\r\n");

    // 3. Check whether a firmware update was requested during startup.
    if (KEY_PRESSED == key_scan())
    {
        // 3.1 Receive the APP image into external W25Q flash.
        int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

        // 3.2 Install the image in the internal APP run region.
        if (0 == copy_backup_to_app(firmware_size))
        {
            // 3.3 Transfer execution to the copied APP image.
            bootloader_jump_to_app();
        }
        else
        {
            elog_e(TAG,
                   "Copy backup data to the run area: copy failed.\r\n");
        }
    }

    // 4. Remain in the Bootloader and wait for another update request.
    while (1)
    {
        elog_e(TAG,
               "There is no valid app,Please press the key and download "
               "a new app.\r\n");

        // 4.1 Start another download when the update key is pressed.
        if (KEY_PRESSED == key_scan())
        {
            // 4.2 Receive the APP image into external W25Q flash.
            int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

            // 4.3 Install the image in the internal APP run region.
            if (0 == copy_backup_to_app(firmware_size))
            {
                elog_i(TAG, "Data copied successfully. Launching app.\r\n");

                // 4.4 Transfer execution to the copied APP image.
                bootloader_jump_to_app();
            }
            else
            {
                elog_e(TAG,
                       "Copy backup data to the run area: copy failed.\r\n");
            }
        }

        // 4.5 Limit the fallback-loop polling rate.
        delay_ms(50);
    }
}
