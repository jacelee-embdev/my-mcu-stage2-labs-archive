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
 * @brief Handles retained reset requests and initializes Bootloader services.
 *
 * Processing flow:
 *
 * Check the retained RAM request before the peripheral initializers in main().
 * Clear a matching request and try the APP entry. If this path returns or no
 * request matches, initialize services, wait for the host, and process the
 * EEPROM OTA state. Normal startup requests another reset for an early jump.
 * IWDG is enabled only by the first-check state. If startup returns, wait for
 * a key and retry the complete download.
 *
 * @version v2.1 2026-10-08
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "main.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define TAG "main"
/******************************* Defines *******************************/

/****************************** Variables ******************************/
/* 状态机写入、main() 消费的复位跳转请求，固定在 RAM 0x2001FFF0。 */
/* NO_INIT 节映射到 UNINIT 区域，启动时不清零；此处不设置初值。 */
volatile uint32_t g_app_jump_request
    __attribute__((section("NO_INIT"), zero_init));
/****************************** Variables ******************************/

/************************* Function Definitions *************************/
/**
 * @brief 处理复位跳转请求，并执行固件启动及升级流程。
 *
 * 处理步骤：
 * 1. 在本函数的外设初始化前读取 RAM 请求，命中后清除并尝试跳转 APP。
 * 2. 无请求或入口无效时，初始化按键、串口、日志、SPI 和存储服务。
 * 3. 输出启动日志并等待约 5 秒，供上位机准备串口连接。
 * 4. 按 EEPROM 状态处理正常启动、安装或首次运行确认。
 * 5. 若状态处理返回，等待按键并重新执行完整下载和安装。
 * 正常启动分支写入 RAM 请求后软复位，下一次进入本函数时提前跳转。
 * 该检查位于 SystemInit() 和 C 运行库初始化之后。
 * 正常启动和等待下载时不主动开启或喂独立看门狗。
 *
 * @return 正常运行时不返回；进入 APP 后控制权交给 APP。
 */
int main(void)
{
    /* 消费一次性跳转请求；先清除，避免下次复位重复命中。 */
    if (0x55AA55AAU == g_app_jump_request)
    {
        g_app_jump_request = 0U;
        bootloader_jump_to_app();
    }

    /* 入口无效时跳转函数返回，继续初始化并保留下载入口。 */
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

    /* 普通启动保留约 5 秒准备时间，复位跳转路径跳过本段。 */
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
