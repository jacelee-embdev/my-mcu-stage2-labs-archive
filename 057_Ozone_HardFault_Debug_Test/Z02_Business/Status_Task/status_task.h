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
 * Toggle LED, log status, then delay 10000 ms on each loop.
 *
 * @version 1.0 2026-10-20
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
 * 3. 阻塞 10 秒后重复。
 *
 * @param[in] p_argument : 任务参数，未使用，可为空。
 *
 * @return 无返回值；任务持续运行，不返回调用方。
 *
 * @note 运行前完成 LED_Init() 和日志初始化。
 *       由 OS_Init() 创建，通过 pdMS_TO_TICKS 将毫秒换算为系统节拍。
 */
void Status_TaskEntry(void *p_argument);

/****************************** Declaring ******************************/

#endif /* __STATUS_TASK_H__ */
