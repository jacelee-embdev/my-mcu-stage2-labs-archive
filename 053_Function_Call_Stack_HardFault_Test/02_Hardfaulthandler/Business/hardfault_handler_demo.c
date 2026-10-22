/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file hardfault_handler_demo.c
 *
 * @par dependencies
 * - hardfault_handler_demo.h
 * - stddef.h
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Demonstrates an invalid-write HardFault.
 *
 * Processing flow:
 *
 * Run HardFaultDemo_Run(); inspect HardFault_Handler() in the debugger.
 *
 * @version V1.0 2026-10-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "hardfault_handler_demo.h"  /* 用户应用层：故障演示 */

#include <stddef.h>                 /* C 标准库 */
#include <stdint.h>

/****************************** Includes ******************************/

/****************************** Defines ******************************/

/* F411 保留地址：见 DS10314 Rev 8，Figure 14。 */
#define HARDFAULT_DEMO_INVALID_ADDRESS 0x60000000UL

/****************************** Defines ******************************/

/****************************** Declaring ******************************/

static void HardFaultDemo_TriggerInvalidWrite(const int8_t *p_operand_a,
                                            const int8_t *p_operand_b);

/****************************** Declaring ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 向保留地址写入求和结果，触发故障。
 *
 * 处理步骤：
 * 1. 准备操作数 1 和 2。
 * 2. 调用非法写入函数。
 *
 * @return 无；预期停在 HardFault_Handler()。
 * @note 仅用于 STM32F411 故障演示。
 * @note BusFault 未使能时，故障升级为 HardFault。
 * @note 故障后需复位。
 */
void HardFaultDemo_Run(void)
{
    const int8_t operand_a = 1;              /* 操作数 1 */
    const int8_t operand_b = 2;              /* 操作数 2 */

    /* 传入有效地址，执行故障测试。 */
    HardFaultDemo_TriggerInvalidWrite(&operand_a, &operand_b);
}

/**
 * @brief 将两数之和写入非法地址。
 *
 * @param[in] p_operand_a : 操作数 1 的地址。
 * @param[in] p_operand_b : 操作数 2 的地址。
 *
 * @return 无；空指针返回，否则预期触发故障。
 * @note p_fault_target 故意指向保留区域。
 */
static void HardFaultDemo_TriggerInvalidWrite(
    const int8_t *p_operand_a,
    const int8_t *p_operand_b)
{
    int8_t sum_value = 0;                    /* 求和结果 */
    volatile int8_t * const p_fault_target =
        (volatile int8_t *)HARDFAULT_DEMO_INVALID_ADDRESS;

    /* 检查输入指针；目标地址故意非法。 */
    if ((NULL == p_operand_a) || (NULL == p_operand_b))
    {
        return;
    }

    sum_value = (int8_t)(*p_operand_a + *p_operand_b);

    /* 断点：单步写入 0x60000000。 */
    *p_fault_target = sum_value;
}

/************************ Function Definitions ************************/
