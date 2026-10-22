/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file iwdg.h
 *
 * @par dependencies
 * - main.h
 * - stm32f4xx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the independent watchdog initialization interface.
 *
 * Processing flow:
 *
 * Call IWDG_Init() with the prescaler and reload value before launching
 * an unconfirmed APP. The first-check flow lets IWDG reset the device
 * after APP confirmation or an unresponsive startup.
 *
 * @version v1.0 2026-10-07
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __IWDG_H
#define __IWDG_H

/******************************* Includes *******************************/
#include "main.h"
#include "stm32f4xx.h"
/******************************* Includes *******************************/

/************************ Function Declarations ************************/
/**
 * @brief 配置分频和重装载值，并开启独立看门狗。
 *
 * 处理步骤：
 * 1. 允许写入预分频和重装载寄存器。
 * 2. 设置调用者提供的分频编码和重装载值。
 * 3. 重装载计数器并使能独立看门狗。
 * 首次检测状态在跳转 APP 前调用，APP 确认后等待看门狗复位。
 *
 * @param[in] prer 标准库预分频编码，例如 IWDG_Prescaler_64。
 * @param[in] rlr 计数器重装载值，范围为 0x0000 至 0x0FFF。
 *
 * @return 无；本接口不检查寄存器更新状态或返回配置结果。
 */
void IWDG_Init(u8 prer, u16 rlr);
/************************ Function Declarations ************************/

#endif /* __IWDG_H */
