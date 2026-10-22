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
 * @brief Initializes Bootloader services and selects the OTA startup path.
 *
 * Processing flow:
 *
 * Initialize peripherals and logging, wait for host preparation, then
 * process normal startup, installation, or first-start confirmation from
 * the EEPROM OTA state. IWDG is enabled only by the first-check state.
 * If startup returns, wait for a key and retry the complete download.
 *
 * @version v2.0 2026-10-07
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "main.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define TAG                        "main"
/******************************* Defines *******************************/

/************************* Function Definitions *************************/
/**
 * @brief 初始化引导服务并执行固件启动及升级流程。
 *
 * 处理步骤：
 * 1. 初始化按键、串口、日志、SPI、外部闪存和 EEPROM 服务。
 * 2. 输出启动日志并等待上位机准备。
 * 3. 按 EEPROM 状态启动 APP、执行升级或首次启动检测。
 * 4. 若启动流程返回，等待按键并重新执行完整下载和安装。
 * 正常启动和等待下载时不主动开启或喂独立看门狗。
 *
 * @return 正常运行时不返回；进入 APP 后控制权交给 APP。
 */
int main(void)
{
    key_init();
    led_init();
    USART1_Init();
    delay_init();
    debug_init();
    SPI1_Init();
    W25Q64_Init();
    ee_check_ok();
    /* 首次检测状态负责开启 IWDG，正常启动不在此处开启。 */
    // IWDG_Init(IWDG_Prescaler_64, 3000);

    /* 等待上位机连接串口，便于触发按键下载。 */
    for (size_t i = 0; i < 10; i++)
    {
        elog_i(TAG, "Bootloader running...\r\n");
        delay_ms(500);
    }
    // delay_s(8);

    OTA_state_mangner();

    elog_e(TAG, "No valid APP. Press key to download a new APP.\r\n");
    /* 启动流程返回后等待按键，每次重新执行完整升级。 */
    while (1)
    {
        if (KEY_PRESSED == key_scan())
        {
            bootloader_download_app();
            elog_e(TAG, "APP update returned. Press key to retry.\r\n");
        }
        // IWDG_ReloadCounter();
        delay_ms(50);
    }
}
/************************* Function Definitions *************************/
