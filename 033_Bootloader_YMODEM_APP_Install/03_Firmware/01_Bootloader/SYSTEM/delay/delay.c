/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file delay.c
 *
 * @par dependencies
 * - delay.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements blocking microsecond, millisecond, and second delays.
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

/****************************** Includes ******************************/
#include "delay.h"
/****************************** Includes ******************************/

/*************************** Private Variables ***************************/
static __IO uint32_t delay_ms_count;
static uint32_t delay_cycles_per_us;
/*************************** Private Variables ***************************/

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
void delay_init(void)
{
    // 1. Update the core-clock value used by both delay time bases.
    SystemCoreClockUpdate();

    // 2. Reset the delay state and calculate the cycles in one microsecond.
    delay_ms_count = 0U;
    delay_cycles_per_us = SystemCoreClock / 1000000U;

    // 3. Enable and reset the DWT cycle counter.
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // 4. Configure SysTick to generate an interrupt every millisecond.
    SysTick_Config(SystemCoreClock / 1000U);
}

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
void delay_us(uint32_t us)
{
    uint32_t start;
    uint32_t cycles;
    uint32_t chunk;
    uint32_t max_chunk;

    // 1. Ignore empty delays and calls made before delay initialization.
    if ((us == 0U) || (delay_cycles_per_us == 0U))
    {
        return;
    }

    // 2. Limit each chunk to prevent multiplication overflow.
    max_chunk = 0xFFFFFFFFU / delay_cycles_per_us;

    // 3. Wait for the requested number of processor cycles.
    while (us != 0U)
    {
        chunk = (us > max_chunk) ? max_chunk : us;
        cycles = chunk * delay_cycles_per_us;
        start = DWT->CYCCNT;

        while ((uint32_t)(DWT->CYCCNT - start) < cycles)
        {
        }

        us -= chunk;
    }
}

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
void delay_ms(uint32_t ms)
{
    // 1. Load the requested millisecond delay.
    delay_ms_count = ms;

    // 2. Wait for the SysTick interrupt to decrement the counter to zero.
    while (delay_ms_count != 0U)
    {
    }
}

/**
 * @brief Provides a blocking second delay.
 *
 * Repeats a 1000 ms delay for each requested second.
 *
 * @param[in] s Delay duration in seconds.
 *
 * @return None.
 */
void delay_s(uint32_t s)
{
    // 1. Convert every requested second into a 1000 ms delay.
    while (s != 0U)
    {
        delay_ms(1000U);
        s--;
    }
}

/**
 * @brief Updates the millisecond delay counter.
 *
 * This function must be called exactly once from every 1 ms SysTick interrupt.
 *
 * @param None.
 *
 * @return None.
 */
void delay_tick(void)
{
    // 1. Decrement the active millisecond delay without underflowing.
    if (delay_ms_count != 0U)
    {
        delay_ms_count--;
    }
}
