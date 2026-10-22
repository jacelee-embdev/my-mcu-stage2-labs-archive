/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file call_stack_demo.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the entry point for the nested call-stack demonstration.
 *
 * Processing flow:
 *
 * Call CallStackDemo_EnterLevel1() once after board initialization.
 * Inspect argument and local-variable markers with the debugger.
 *
 * @version V1.0 2026-10-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __CALL_STACK_DEMO_H__
#define __CALL_STACK_DEMO_H__

/****************************** Includes ******************************/

#include <stdint.h>  /* C 标准库：固定宽度整数类型 */

/****************************** Includes ******************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Declaring ******************************/

/**
 * @brief 进入第一级调用，建立局部标记并调用第二级函数。
 *
 * @param[in] argument_marker_1 : 第一个参数标记，仅供调试观测。
 * @param[in] argument_marker_2 : 第二个参数标记，仅供调试观测。
 *
 * @return 无返回值。
 * @note 在第三级标记读取处暂停，可沿调用栈检查本级局部变量。
 */
void CallStackDemo_EnterLevel1(uint32_t argument_marker_1,
                             uint32_t argument_marker_2);

/****************************** Declaring ******************************/

#ifdef __cplusplus
}
#endif

#endif /* CALL_STACK_DEMO_H */
