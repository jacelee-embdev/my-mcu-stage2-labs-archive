/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file app.h
 *
 * @par dependencies
 * - FreeRTOS.h
 * - task.h
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the application demonstration task.
 *
 * Processing flow:
 *
 * Create app_task() as an RTOS task during initialization.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __APP_H__
#define __APP_H__

/********************************* Includes *********************************/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
/********************************* Includes *********************************/

/************************** Function Declarations ***************************/
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
void app_task(void *p_argument);
/************************** Function Declarations ***************************/

#endif /* 结束头文件保护。 */
