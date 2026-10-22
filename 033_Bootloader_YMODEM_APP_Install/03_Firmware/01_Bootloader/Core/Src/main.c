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
 * - ymodem.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides the Bootloader firmware-update entry point.
 *
 * Processing flow:
 *
 * Initialize the Bootloader peripherals and logging service, receive an APP
 * image through YMODEM, program it into the application Flash region, and
 * transfer execution to the downloaded application. If the handover is
 * rejected, remain in the Bootloader fallback loop.
 *
 * @version V1.3 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "main.h"
#include "bootloader.h"
#include "ymodem.h"
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
 * 1. Initializes the GPIO, USART, delay, and logging modules.
 * 2. Reports that the Bootloader is ready for a firmware transfer.
 * 3. Receives and programs an APP image through YMODEM.
 * 4. Transfers execution to the programmed application.
 * 5. Enters the fallback loop if the application handover returns.
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

    // 2. Report that the Bootloader is ready to receive the APP image.
    elog_i(TAG, "Bootloader running...\r\n");

    // 3. Receive the APP image and program it into the application region.
    Ymodem_Receive(g_ymodem_receive_buffer);

    // 4. Transfer execution to the programmed application.
#if 1
    bootloader_jump_to_app();
#endif

    // 5. Remain in the Bootloader if the application handover is rejected.
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
