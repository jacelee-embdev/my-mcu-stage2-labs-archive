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
 * @brief Runs the application LED task.
 *
 * Processing flow:
 *
 * Toggle the LED and delay for 500 RTOS ticks each cycle.
 * EEPROM and W25Q device-ID test calls remain commented out.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************* Includes *********************************/
#include "app.h"
/********************************* Includes *********************************/

/********************************* Defines **********************************/
#define TAG  "app"  /* 日志模块标识。 */
/********************************* Defines **********************************/

/******************************** Functions *********************************/
/**
 * @brief 运行指示灯业务线程。
 *
 * 循环切换指示灯，每轮延时 500 个系统节拍。
 * EEPROM 读写和外部闪存标识测试代码保留为注释，当前不执行。
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

        // 3. 延时 500 个系统节拍，再执行下一轮。
        vTaskDelay(500);
    }
}
/******************************** Functions *********************************/
