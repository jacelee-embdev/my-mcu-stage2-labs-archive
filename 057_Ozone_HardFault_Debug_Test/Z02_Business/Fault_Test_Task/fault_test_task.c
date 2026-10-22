/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file fault_test_task.c
 *
 * @par dependencies
 * - fault_test_task.h
 * - stdint.h
 * - main.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs minimal tests for invalid writes, integer division by zero,
 *        and invalid reads.
 *
 * Processing flow:
 *
 * Run Fault_Test_TaskEntry() as a FreeRTOS task.
 * Log task activity, then select a test with g_fault_test_case.
 * NONE and unknown selector values skip the fault tests.
 * Delay 1000 ms after each normal loop; faults usually stop normal execution.
 *
 * @version 1.0 2026-10-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "fault_test_task.h"  /* 业务层：故障测试任务 */

#include <stdint.h>           /* C 标准库：定宽整数类型 */

#include "main.h"             /* Core 层：STM32、SCB 寄存器定义 */

#include "FreeRTOS.h"         /* 中间件层：FreeRTOS、日志 */
#include "task.h"
#include "debug.h"

/****************************** Includes ******************************/

/****************************** Defines ******************************/

#define TAG  "fault_test_task"  /* 日志标识 */

/****************************** Defines ******************************/

/************************ Variable Definitions ************************/

/* 模块内测试选择标志；当前选择非法写入，可调试修改或更改初值。 */
static volatile fault_test_case_t g_fault_test_case = FAULT_TEST_INVALID_WRITE;

/************************ Variable Definitions ************************/

/************************ Function Definitions ************************/

/**
 * @brief 将数值 3 写入 STM32F411 的保留地址 0x60000000。
 *
 * @return 无返回值。
 *
 * @note 故意执行 8 位非法写入，预期触发 BusFault。
 *       BusFault 处理未使能时，故障升级为 HardFault。
 */
static void FaultTest_InvalidWrite(void)
{
    *(volatile uint8_t *)0x60000000UL = 3U;
}

/**
 * @brief 开启除零陷阱并执行整数除零。
 *
 * 处理步骤：
 * 1. 设置 CCR.DIV_0_TRP，使能整数除零陷阱。
 * 2. 执行 10 / 0，预期在除法指令处触发故障。
 *
 * @return 无返回值。
 *
 * @note 预期触发 UsageFault，原因为 DIVBYZERO。
 *       UsageFault 处理未使能时，故障升级为 HardFault。
 */
static void FaultTest_DivideByZero(void)
{
    volatile int denominator = 0;  /* 故意设置为零，保留运行时读取 */
    volatile int result      = 0;  /* 接收除法结果 */

    /* 1. 使能整数除零陷阱。 */
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;

    /* 2. 执行整数除零，预期在此处触发故障。 */
    result = 10 / denominator;

    (void)result;
}

/**
 * @brief 从非法地址 0xFFFFFFFF 读取一个 32 位数据。
 *
 * @return 无返回值。
 *
 * @note 此地址未对齐且读取跨越地址空间边界，实际原因查看 CFSR。
 *       本函数保留非法读取操作，未包含原表达式后续的除法。
 */
static void FaultTest_InvalidRead(void)
{
    /* 接收读取结果；目标地址故意非法。 */
    volatile uint32_t value = *(volatile uint32_t *)0xFFFFFFFFUL;

    (void)value;
}

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
void Fault_Test_TaskEntry(void *p_argument)
{
    (void)p_argument;

    for (;;)
    {
        /* 1. 输出线程运行日志。 */
        elog_i(TAG, "Fault_Test_Task running...\r\n");

        /* 2. 按选择标志执行一个故障测试。 */
        switch (g_fault_test_case)
        {
            case FAULT_TEST_INVALID_WRITE:
            {
                FaultTest_InvalidWrite();
                break;
            }

            case FAULT_TEST_DIVIDE_BY_ZERO:
            {
                FaultTest_DivideByZero();
                break;
            }

            case FAULT_TEST_INVALID_READ:
            {
                FaultTest_InvalidRead();
                break;
            }

            case FAULT_TEST_NONE:
            default:
            {
                /* 未选择测试或数值无效时，跳过故障触发。 */
                break;
            }
        }

        /* 3. 正常流程延时 1000 毫秒后，再次检查选择标志。 */
        vTaskDelay(pdMS_TO_TICKS(1000U));
    }
}

/************************ Function Definitions ************************/
