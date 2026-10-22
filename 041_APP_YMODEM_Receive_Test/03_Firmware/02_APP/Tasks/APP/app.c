/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file app.c
 *
 * @par dependencies
 * - app.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the application demonstration task.
 *
 * Processing flow:
 *
 * Toggle the LED, test EEPROM, and log the W25Q device ID.
 * Repeat after a delay of 500 RTOS ticks.
 *
 * @version v1.0 2026-10-02
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "app.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define TAG  "app"  /* 日志模块标识。 */
/******************************* Defines *******************************/

/****************************** Functions ******************************/
/**
 * @brief 运行业务演示线程。
 *
 * 循环切换指示灯、执行 EEPROM 测试并打印 W25Q 器件标识。
 * 每轮延时 500 个系统节拍，让其他线程获得运行机会。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；线程持续运行。
 */
void app_task(void *p_argument)
{
    (void)p_argument;

    for (;;)
    {
        // 1. 切换指示灯，显示业务线程仍在运行。
        HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);

        // 2. 执行 EEPROM 读写测试。
        ee_test();

        // 3. 读取并输出外部 Flash 的器件标识。
        uint16_t w25q_id = 0;  /* 本轮读取的器件标识。 */
        W25Qx_Read_ID(&w25q_id);
        elog_i(TAG, "w25q_id:%X\r\n", w25q_id);

        // 4. 按系统节拍延时。
        vTaskDelay(500);
    }
}
/****************************** Functions ******************************/
