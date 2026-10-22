/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file fault_test_task.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the fault test task.
 *
 * Processing flow:
 *
 * Created by OS_Init().
 * Run the divide-by-zero test.
 *
 * @version V1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __FAULT_TEST_TASK_H__
#define __FAULT_TEST_TASK_H__

/****************************** Declaring ******************************/

/**
 * @brief 除零异常测试任务。
 *
 * 处理步骤：
 * 1. 打印开始日志。
 * 2. 执行除零测试。
 * 3. 若返回，打印结束日志并延时。
 *
 * @param[in] p_argument : 未使用，可为 NULL。
 *
 * @return 无；不返回。
 *
 * @note 启动前初始化日志和 CmBacktrace。
 *       异常未恢复时，后续流程不执行。
 */
void Fault_Test_TaskEntry(void *p_argument);

/****************************** Declaring ******************************/

#endif /* __FAULT_TEST_TASK_H__ */
