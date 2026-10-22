/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file delay.h
 *
 * @par dependencies
 * - stm32f4xx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares blocking microsecond, millisecond, and second delay APIs.
 *
 * Processing flow:
 *
 * Call delay_init() once before using the delay APIs. Call delay_tick() from
 * the 1 ms SysTick interrupt handler.
 *
 * @version V1.0 2026-09-18
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __DELAY_H__
#define __DELAY_H__

/****************************** Includes ******************************/
#include "stm32f4xx.h"
/****************************** Includes ******************************/

/************************ Function Declarations ************************/
/**
 * @brief Initializes the delay time bases.
 *
 * Updates the core-clock value, enables the DWT cycle counter for microsecond
 * delays, and configures SysTick to generate an interrupt every millisecond.
 *
 * @param None.
 *
 * @return None.
 */
void delay_init(void);

/**
 * @brief Provides a blocking microsecond delay.
 *
 * Measures elapsed processor cycles with the DWT cycle counter.
 * delay_init() must be called before this function.
 *
 * @param[in] us Delay duration in microseconds.
 *
 * @return None.
 */
void delay_us(uint32_t us);

/**
 * @brief Provides a blocking millisecond delay.
 *
 * Waits until delay_tick() decrements the millisecond counter to zero.
 * SysTick interrupts must remain enabled while this function is running.
 *
 * @param[in] ms Delay duration in milliseconds.
 *
 * @return None.
 */
void delay_ms(uint32_t ms);

/**
 * @brief Provides a blocking second delay.
 *
 * Repeats a 1000 ms delay for each requested second.
 *
 * @param[in] s Delay duration in seconds.
 *
 * @return None.
 */
void delay_s(uint32_t s);

/**
 * @brief Updates the millisecond delay counter.
 *
 * This function must be called exactly once from every 1 ms SysTick interrupt.
 *
 * @param None.
 *
 * @return None.
 */
void delay_tick(void);
/************************ Function Declarations ************************/

#endif /* End of __DELAY_H__ */
