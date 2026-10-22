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
 * @brief Provides the Bootloader entry point and W25Q test loop.
 *
 * Processing flow:
 *
 * Initialize the Bootloader services, handle a requested firmware update,
 * and run the enabled W25Q erase, write, and read test loop.
 *
 * @version V1.5 2026-09-29
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
 * 4. Receives the APP image into the backup region when requested.
 * 5. Copies the received image into the run region and starts the APP.
 * 6. Prints the W25Q ID and repeatedly tests erase, write, and read.
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
    W25Qx_Init();

    // 2. Report that the Bootloader is ready to receive the APP image.
    elog_i(TAG, "Bootloader running...\r\n");

    // 3. Check whether a firmware update was requested during startup.
    if (KEY_PRESSED == key_scan())
    {
        // 3.1 Receive the APP image into the B (backup) region.
        int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

        // 3.2 Copy the image from the B region to the A (run) region.
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

// Run the temporary W25Q flash test.
#if 1
    // Read and print the chip ID once at startup.
    uint16_t id = 0U;
    W25Qx_Read_ID(&id);
    elog_i(TAG, "ID:%u\r\n", (unsigned int)id);

    while (1)
    {
        static uint32_t test_value = 0U;
        const uint32_t test_addr = 0x007FF000U;
        uint32_t write_value = 0U;
        uint32_t read_value = 0U;
        uint8_t status = W25Qx_OK;

        // Erase the test sector before its first write.
        if (0U == test_value)
        {
            status = W25Qx_Erase_Block(test_addr);
        }

        // Write one increasing value, erase, then write the next value.
        if (W25Qx_OK == status)
        {
            write_value = ++test_value;
            status = W25Qx_Write((uint8_t *)&write_value,
                                 test_addr + sizeof(write_value),
                                 sizeof(write_value));
        }
        if (W25Qx_OK == status)
        {
            status = W25Qx_Erase_Block(test_addr);
        }
        if (W25Qx_OK == status)
        {
            write_value = ++test_value;
            status = W25Qx_Write((uint8_t *)&write_value,
                                 test_addr, sizeof(write_value));
        }
        if (W25Qx_OK == status)
        {
            status = W25Qx_Read((uint8_t *)&read_value,
                                test_addr, sizeof(read_value));
        }

        if (W25Qx_OK != status)
        {
            elog_e(TAG, "W25Q test failed: %u\r\n", (unsigned int)status);
        }
        else
        {
            elog_i(TAG, "W25Q write=%lu, read=%lu, %s\r\n",
                   (unsigned long)write_value, (unsigned long)read_value,
                   (write_value == read_value) ? "OK" : "FAIL");
        }
        delay_ms(1000U);
    }
#endif

    // Keep the original fallback loop disabled during SPI and W25Q testing.
#if 0
    // 4. Remain in the Bootloader until a valid APP can be started.
    while (1)
    {
        elog_e(TAG,
               "There is no valid app,Please press the key and download "
               "a new app.\r\n");

        // 4.1 Start another download when the update key is pressed.
        if (KEY_PRESSED == key_scan())
        {
            // 4.2 Receive the APP image into the B (backup) region.
            int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

            // 4.3 Copy the image from the B region to the A (run) region.
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
#endif

}
