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
 * @brief Provides the Bootloader update loop and AT24C02 read/write test.
 *
 * Processing flow:
 *
 * Initialize the Bootloader services and detect the EEPROM. Receive and
 * install encrypted firmware when requested; otherwise run one EEPROM
 * test before waiting for another firmware update request.
 *
 * @version v1.7 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG                        "main"  /* 模块日志标签。 */
#define YMODEM_RECEIVE_BUFFER_SIZE  (1024U) /* 固件接收缓冲区字节数。 */
/****************************** Defines ******************************/

/****************************** Variables ******************************/
/* 保存固件接收过程使用的数据包缓冲区。 */
static uint8_t g_ymodem_receive_buffer[YMODEM_RECEIVE_BUFFER_SIZE] = {0U};
/****************************** Variables ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 运行引导程序的固件升级流程及存储器读写测试。
 *
 * 处理步骤：
 * 1. 初始化按键、指示灯、串口、时基、日志、外部闪存及存储器总线。
 * 2. 打印启动日志，检查按键触发的固件升级请求。
 * 3. 接收密文到外部闪存，解密安装到内部应用运行区并尝试跳转。
 * 4. 若执行继续到达测试位置，对串行存储器最后一页执行一次读写测试。
 * 5. 留在等待循环中，接受后续按键升级请求。
 *
 * @note 存储器检测和测试的返回值未在本函数中处理。
 *       读写测试会覆盖最后一页的八个字节，结果通过日志输出。
 *
 * @return 正常运行时不返回。
 */
int main(void)
{
    /* 1. 初始化引导程序使用的硬件和软件服务。 */
    key_init();
    led_init();
    USART1_Init();
    delay_init();
    debug_init();
    SPI1_Init();
    W25Q64_Init();
    /* 初始化软件总线引脚并检测存储器应答。 */
    ee_check_ok();

    /* 2. 打印引导程序启动日志。 */
    elog_i(TAG, "Bootloader running...\r\n");

    /* 3. 检查启动时是否按下升级键。 */
    if (KEY_PRESSED == key_scan())
    {
        /* 3.1 接收应用固件密文到外部闪存。 */
        /* 记录接收结果，供安装接口检查固件大小或错误状态。 */
        int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

        /* 3.2 解密并安装固件到内部应用运行区。 */
        if (0 == copy_backup_to_app(firmware_size))
        {
            /* 3.3 尝试跳转到已安装的应用入口。 */
            bootloader_jump_to_app();
        }
        else
        {
            elog_e(TAG,
                   "Copy backup data to the run area: copy failed.\r\n");
        }
    }

    /* 4. 若尚未跳转离开，对存储器最后一页执行一轮读写测试。 */
    ee_test();

    /* 5. 留在引导程序中等待后续升级请求。 */
    while (1)
    {
        /* 此提示在驻留循环中打印，本循环未再次执行应用有效性检查。 */
        elog_e(TAG,
               "There is no valid app,Please press the key and download "
               "a new app.\r\n");

        /* 5.1 检测按键，触发新一轮固件下载。 */
        if (KEY_PRESSED == key_scan())
        {
            /* 5.2 接收应用固件密文到外部闪存。 */
            /* 记录接收结果，供安装接口检查固件大小或错误状态。 */
            int32_t firmware_size = Ymodem_Receive(g_ymodem_receive_buffer);

            /* 5.3 解密并安装固件到内部应用运行区。 */
            if (0 == copy_backup_to_app(firmware_size))
            {
                elog_i(TAG, "Data copied successfully. Launching app.\r\n");

                /* 5.4 尝试跳转到已安装的应用入口。 */
                bootloader_jump_to_app();
            }
            else
            {
                elog_e(TAG,
                       "Copy backup data to the run area: copy failed.\r\n");
            }
        }

        /* 5.5 通过五十毫秒延时限制驻留循环的轮询频率。 */
        delay_ms(50);
    }
}

/************************ Function Definitions ************************/
