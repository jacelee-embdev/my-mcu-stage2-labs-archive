/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file sct_test_task.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the SCT function-call timing test task entry.
 *
 * Processing flow:
 *
 * Create SCT_Test_TaskEntry() from OS_Init() before scheduler startup.
 * Initialize logging before the task runs.
 *
 * @version V1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SCT_TEST_TASK_H__
#define __SCT_TEST_TASK_H__

/****************************** Declaring ******************************/

/**
 * @brief 测量并记录 Flash/SRAM 耗时，每轮延时 1000 毫秒。
 * @param[in] p_argument : 任务参数，未使用，可为空。
 * @return 无；任务持续运行。
 * @note 由 OS_Init() 创建，运行前须初始化日志。
 */
void SCT_Test_TaskEntry(void *p_argument);

/****************************** Declaring ******************************/

#endif /* __SCT_TEST_TASK_H__ */
