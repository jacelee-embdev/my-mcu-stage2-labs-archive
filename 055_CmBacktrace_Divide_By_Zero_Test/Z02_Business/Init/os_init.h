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
 * Create status_task and fault_test_task before scheduler startup.
 *
 * @version v1.0 2026-10-19
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
 * 1. 创建状态测试任务并记录结果。
 * 2. 创建故障测试任务并记录结果。
 * 3. 任一任务创建失败时记录错误并返回。
 *
 * @return 无。
 *
 * @note LED、日志初始化后，调度器启动前调用一次。
 *       栈深度按 StackType_t 元素计。
 */
void OS_Init(void);

/****************************** Declaring ******************************/

#endif /* __OS_INIT_H__ */
