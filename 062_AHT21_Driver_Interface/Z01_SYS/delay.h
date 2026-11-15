/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file delay.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author JesMicro
 *
 * @brief Declares blocking delays based on the DWT cycle counter.
 *
 * Usage:
 *
 * Configure the core clock and keep SystemCoreClock up to date.
 * Call Delay_Init once before using Delay_Us or Delay_Ms.
 * Keep the core clock and the DWT cycle counter running during each delay.
 *
 * @version v1.0 2026-11-06
 *
 * @note 1 tab == 4 spaces! Delay functions busy-wait and do not yield.
 *
 ******************************************************************************/

#ifndef __DELAY_H__
#define __DELAY_H__

/********************************** Includes **********************************/

#include <stdint.h>               /* C standard library                       */

/********************************** Includes **********************************/

/********************************* Declaring **********************************/

/**
 * @brief Enable and reset the DWT cycle counter.
 *
 * @return None.
 *
 * @note Call before using Delay_Us or Delay_Ms.
 *       Calling again resets the shared DWT cycle counter.
 */
void Delay_Init(void);

/**
 * @brief Wait for the requested number of microseconds using DWT.
 *
 * @param[in] us : Delay duration in microseconds; zero performs no wait.
 *
 * @return None.
 *
 * @note Initialize DWT first and keep it running throughout the delay.
 *       SystemCoreClock must be current and at least 1000000 Hz.
 *       The duration multiplied by cycles per microsecond must fit uint32_t.
 *       Clock conversion uses integer division. Interrupts can extend the wait.
 *       This function does not validate parameters or hardware state.
 */
void Delay_Us(uint32_t us);

/**
 * @brief Wait for the requested number of milliseconds using DWT.
 *
 * @param[in] ms : Delay duration in milliseconds; zero performs no wait.
 *
 * @return None.
 *
 * @note Uses one Delay_Us(1000U) call per millisecond.
 *       The initialization and clock requirements of Delay_Us also apply.
 *       Loop overhead and interrupts can extend the total delay.
 */
void Delay_Ms(uint32_t ms);

/********************************* Declaring **********************************/

#endif  /* __DELAY_H__ */
