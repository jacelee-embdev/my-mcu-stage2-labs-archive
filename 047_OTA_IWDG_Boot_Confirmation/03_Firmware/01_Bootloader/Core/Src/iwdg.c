/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file iwdg.c
 *
 * @par dependencies
 * - iwdg.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Configures and enables IWDG for APP first-start confirmation.
 *
 * Processing flow:
 *
 * Enable register writes, set the supplied prescaler and reload value,
 * reload the counter, and enable IWDG. Bootloader calls this interface
 * before launching an APP with EEPROM confirmation state 0x44.
 *
 * @version v1.0 2026-10-07
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "iwdg.h"
/******************************* Includes *******************************/

/************************* Function Definitions *************************/
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
void IWDG_Init(u8 prer, u16 rlr)
{
    /* 1. 允许写入预分频和重装载寄存器。 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* 2. 设置分频编码和计数器重装载值。 */
    IWDG_SetPrescaler(prer);
    IWDG_SetReload(rlr);

    /* 3. 重装载计数器并启动独立看门狗。 */
    IWDG_ReloadCounter();
    IWDG_Enable();
}
/************************* Function Definitions *************************/
