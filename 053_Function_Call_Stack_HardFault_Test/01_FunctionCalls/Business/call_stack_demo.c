/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file call_stack_demo.c
 *
 * @par dependencies
 * - call_stack_demo.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Demonstrates nested calls, argument passing, and stack markers.
 *
 * Processing flow:
 *
 * Call CallStackDemo_EnterLevel1() from main after board initialization.
 * Follow three nested call levels and inspect markers in the debugger.
 *
 * @version V1.0 2026-10-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "call_stack_demo.h"  /* 用户应用层：函数调用栈演示接口 */

/****************************** Includes ******************************/

/****************************** Defines ******************************/

/* 当前 ARMCC 工具链的禁止内联属性，用于保留各级函数调用。 */
#define CALL_STACK_DEMO_NOINLINE __attribute__((noinline))

/****************************** Defines ******************************/

/****************************** Declaring ******************************/

/* 内部调用层仅供本模块使用。 */
static void CallStackDemo_EnterLevel2(uint32_t argument_marker_1,
                                    uint32_t argument_marker_2);
static void CallStackDemo_EnterLevel3(uint32_t argument_marker_1,
                                    uint32_t argument_marker_2);

/****************************** Declaring ******************************/

/************************ Variable Definitions ************************/

/* 已初始化的全局数据标记，用于与栈上的局部标记作对照。 */
static volatile uint8_t g_call_stack_demo_data_marker = 123U;

/************************ Variable Definitions ************************/

/************************ Function Definitions ************************/

/**
 * @brief 进入第一级调用，建立局部标记并调用第二级函数。
 *
 * @param[in] argument_marker_1 : 第一个参数标记，仅供调试观测。
 * @param[in] argument_marker_2 : 第二个参数标记，仅供调试观测。
 *
 * @return 无返回值。
 * @note 在第三级标记读取处暂停，可沿调用栈检查本级局部变量。
 */
CALL_STACK_DEMO_NOINLINE void CallStackDemo_EnterLevel1(
    uint32_t argument_marker_1,
    uint32_t argument_marker_2)
{
    volatile uint32_t level1_marker = 0xA1A2A3A4U; /* 第一级局部标记 */

    /* 参数仅供调试观测，显式标记为已处理以避免未使用告警。 */
    (void)argument_marker_1;
    (void)argument_marker_2;

    /* 读取全局数据标记，保留与局部变量进行对照的对象。 */
    (void)g_call_stack_demo_data_marker;

    CallStackDemo_EnterLevel2(0xB123U, 0xB456U);

    /* 子调用返回后原值读写本级标记，保留可观测的返回路径。 */
    level1_marker = level1_marker;
}

/**
 * @brief 进入第二级调用，建立局部标记并调用第三级函数。
 *
 * @param[in] argument_marker_1 : 第一个参数标记，仅供调试观测。
 * @param[in] argument_marker_2 : 第二个参数标记，仅供调试观测。
 *
 * @return 无返回值。
 */
static CALL_STACK_DEMO_NOINLINE void CallStackDemo_EnterLevel2(
    uint32_t argument_marker_1,
    uint32_t argument_marker_2)
{
    volatile uint32_t level2_marker = 0xB1B2B3B4U; /* 第二级局部标记 */

    /* 参数标记用于观察本级的参数传递。 */
    (void)argument_marker_1;
    (void)argument_marker_2;

    CallStackDemo_EnterLevel3(0xC123U, 0xC456U);

    /* 返回后原值读写本级标记，便于观察栈帧的保留和释放。 */
    level2_marker = level2_marker;
}

/**
 * @brief 进入第三级调用，提供完整调用链的断点观测位置。
 *
 * @param[in] argument_marker_1 : 第一个参数标记，仅供调试观测。
 * @param[in] argument_marker_2 : 第二个参数标记，仅供调试观测。
 *
 * @return 无返回值。
 * @note 参数可能位于寄存器或栈中，以当前反汇编为准。
 */
static CALL_STACK_DEMO_NOINLINE void CallStackDemo_EnterLevel3(
    uint32_t argument_marker_1,
    uint32_t argument_marker_2)
{
    volatile uint32_t level3_marker = 0xC1C2C3C4U; /* 第三级局部标记 */

    (void)argument_marker_1;
    (void)argument_marker_2;

    /* 在下一行设置断点，检查各级局部标记、SP 和 LR。 */
    level3_marker = level3_marker;
}

/************************ Function Definitions ************************/
