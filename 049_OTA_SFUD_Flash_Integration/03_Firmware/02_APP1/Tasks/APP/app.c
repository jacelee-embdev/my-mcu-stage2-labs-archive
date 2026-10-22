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
 * @brief Runs the APP1 LED and status-log task.
 *
 * Processing flow:
 *
 * Toggle PC13, print the APP1 status message, and delay for 500 RTOS ticks
 * in each loop. EEPROM and device-ID test statements remain disabled.
 *
 * @version v1.2 2026-10-06
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

/************************* Function Definitions *************************/
/**
 * @brief 周期性切换 PC13 并输出 APP1 运行日志。
 *
 * 每轮切换指示灯、输出 App1 running...，然后延时 500 个系统节拍。
 * EEPROM 和外部闪存标识测试语句处于注释中，当前不执行。
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

        // 2. 保留 EEPROM 和外部闪存测试代码，当前不执行。
        // ee_test();

        // 读取并输出外部闪存器件标识的测试代码。
        // uint16_t w25q_id = 0;  /* 本轮读取的器件标识。 */
        // W25Qx_Read_ID(&w25q_id);
        // elog_i(TAG, "w25q_id:%X\r\n", w25q_id);

        // 3. 输出 APP1 运行状态。
        elog_i(TAG, "APP1 running...\r\n");

        // 4. 延时 500 个系统节拍，再执行下一轮。
        vTaskDelay(500);
    }
}
/************************* Function Definitions *************************/
