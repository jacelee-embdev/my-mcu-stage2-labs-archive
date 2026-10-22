/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file status_task.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the status test task.
 *
 * Processing flow:
 *
 * Create Status_TaskEntry() after LED and logging initialization.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __STATUS_TASK_H__
#define __STATUS_TASK_H__

/****************************** Declaring ******************************/

/**
 * @brief 运行状态测试任务。
 *
 * 处理步骤：
 * 1. 翻转 LED。
 * 2. 输出状态日志。
 * 3. 延时 500 个节拍后重复。
 *
 * @param[in] p_argument : 任务参数，未使用，可为空。
 *
 * @return 无；任务不返回。
 *
 * @note 运行前完成 LED_Init() 和日志初始化。
 *       任务模板：每个任务使用独立目录及配套 .c/.h。
 */
void Status_TaskEntry(void *p_argument);

/****************************** Declaring ******************************/

#endif /* __STATUS_TASK_H__ */
