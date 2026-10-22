/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file hardfault_handler_demo.h
 *
 * @par dependencies
 * - None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the HardFault demo entry point.
 *
 * Processing flow:
 *
 * Run after board initialization; break in HardFault_Handler().
 *
 * @version V1.0 2026-10-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __HARDFAULT_HANDLER_DEMO_H__
#define __HARDFAULT_HANDLER_DEMO_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Declaring ******************************/

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
void HardFaultDemo_Run(void);

/****************************** Declaring ******************************/

#ifdef __cplusplus
}
#endif

#endif /* HARDFAULT_HANDLER_DEMO_H */
