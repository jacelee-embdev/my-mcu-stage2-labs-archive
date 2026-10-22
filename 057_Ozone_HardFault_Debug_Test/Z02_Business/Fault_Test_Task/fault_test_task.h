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
 * @brief Declares fault test cases and the task entry.
 *
 * Processing flow:
 *
 * Create Fault_Test_TaskEntry() from OS_Init() before scheduler startup.
 * Configure the internal selector in fault_test_task.c to choose a test.
 * The task logs activity before running the selected test.
 * NONE and unknown selector values skip the fault tests.
 * Delay 1000 ms after each normal loop; faults usually stop normal execution.
 *
 * @version 1.0 2026-10-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __FAULT_TEST_TASK_H__
#define __FAULT_TEST_TASK_H__

/****************************** Defines ******************************/

/* 故障测试用例；可在调试器中按数值选择。 */
typedef enum
{
    FAULT_TEST_NONE           = 0,  /* 输出运行日志并等待选择 */
    FAULT_TEST_INVALID_WRITE  = 1,  /* 向 0x60000000 写入 8 位数据 */
    FAULT_TEST_DIVIDE_BY_ZERO = 2,  /* 开启除零陷阱并执行 10 / 0 */
    FAULT_TEST_INVALID_READ   = 3   /* 从 0xFFFFFFFF 读取 32 位数据 */
} fault_test_case_t;

/****************************** Defines ******************************/

/****************************** Declaring ******************************/

/**
 * @brief 根据选择标志运行故障测试任务。
 *
 * 处理步骤：
 * 1. 输出线程运行日志。
 * 2. 按选择标志执行一个测试；未选择或数值无效时跳过。
 * 3. 正常流程延时 1000 毫秒后，重新读取选择标志。
 *
 * @param[in] p_argument : 任务参数，未使用，可为空。
 *
 * @return 无返回值；任务持续运行，不返回调用方。
 *
 * @note 由 OS_Init() 创建，运行前须完成日志初始化。
 *       选择标志不自动清除；测试返回且选择不变时，下一轮重复测试。
 *       故障触发后通常进入异常处理，需复位后再测试其他用例。
 */
void Fault_Test_TaskEntry(void *p_argument);

/****************************** Declaring ******************************/

#endif /* __FAULT_TEST_TASK_H__ */
