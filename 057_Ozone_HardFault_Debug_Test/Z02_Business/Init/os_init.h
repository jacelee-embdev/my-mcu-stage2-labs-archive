/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file os_init.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares task initialization.
 *
 * Processing flow:
 *
 * Create status_task and fault_test_task once before scheduler startup.
 * Stop on task creation failure; otherwise log successful resource creation.
 *
 * @version 1.0 2026-10-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __OS_INIT_H__
#define __OS_INIT_H__

/****************************** Declaring ******************************/

/**
 * @brief 创建业务任务。
 *
 * 处理步骤：
 * 1. 创建状态任务；失败时记录错误并返回。
 * 2. 创建故障测试任务；失败时记录错误并返回。
 * 3. 两个任务均创建成功时，输出资源创建成功日志。
 *
 * @return 无返回值。
 *
 * @note LED、日志初始化后，调度器启动前调用一次。
 *       两个任务的栈深度均为 128 个 StackType_t 元素，优先级均为 1。
 *       创建失败时保留此前已创建的任务，不执行回滚。
 */
void OS_Init(void);

/****************************** Declaring ******************************/

#endif /* __OS_INIT_H__ */
