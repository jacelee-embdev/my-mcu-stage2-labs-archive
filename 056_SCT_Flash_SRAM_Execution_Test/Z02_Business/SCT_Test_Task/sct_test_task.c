/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file sct_test_task.c
 *
 * @par dependencies
 * - sct_test_task.h
 * - stdint.h
 * - stm32f4xx_hal.h
 * - FreeRTOS.h
 * - task.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the Flash and SRAM function-call timing tests.
 *
 * Processing flow:
 *
 * Create SCT_Test_TaskEntry() from OS_Init() before scheduler startup.
 * Measure each test loop with HAL_GetTick(), log the elapsed time,
 * then wait 1000 ms before repeating.
 * Section placement is determined by the linker configuration.
 *
 * @version V1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/

#include "sct_test_task.h"  /* 业务层：SCT 测试任务 */

#include <stdint.h>         /* C 标准库：定宽整数类型 */

#include "stm32f4xx_hal.h"  /* HAL 层：HAL_GetTick */

#include "FreeRTOS.h"       /* 中间件层：FreeRTOS、日志 */
#include "task.h"
#include "debug.h"

/****************************** Includes ******************************/

/************************ Function Definitions ************************/

/**
 * @brief Flash 测试函数 1：执行简单除法。
 * @return 无。
 * @note 使用默认代码段。
 */
static void SCT_FlashTestFunction1(void)
{
    int i = 0;  /* 除法测试变量 */

    i = i / 3;
}

/**
 * @brief Flash 测试函数 2：执行简单除法。
 * @return 无。
 * @note 使用 myflash 段。
 */
__attribute__((used, section("myflash")))
static void SCT_FlashTestFunction2(void)
{
    int i = 0;  /* 除法测试变量 */

    i = i / 3;
}

/**
 * @brief SRAM 测试函数 1：执行简单除法。
 * @return 无。
 * @note 使用 myram 段。
 */
__attribute__((used, section("myram")))
static void SCT_SramTestFunction1(void)
{
    int i = 0;  /* 除法测试变量 */

    i = i / 3;
}

/**
 * @brief SRAM 测试函数 2：执行简单除法。
 * @return 无。
 * @note 使用 myram 段。
 */
__attribute__((used, section("myram")))
static void SCT_SramTestFunction2(void)
{
    int i = 0;  /* 除法测试变量 */

    i = i / 3;
}

/**
 * @brief 循环调用两个 Flash 测试函数 10000000 次并计时。
 * @return 测试耗时，单位为毫秒。
 * @note 包含循环、调用及中断和调度开销。
 */
static uint32_t SCT_MeasureFlashTime(void)
{
    uint32_t start_tick = HAL_GetTick();  /* 起始毫秒计数 */
    uint32_t end_tick = 0U;               /* 结束毫秒计数 */
    uint32_t i_counter = 10000000U;       /* 剩余循环次数 */

    /* 固定测试入口，保留 volatile。 */
    void (* volatile pf_flash_test_1)(void) = SCT_FlashTestFunction1;
    void (* volatile pf_flash_test_2)(void) = SCT_FlashTestFunction2;

    /* 重复调用两个 Flash 测试函数。 */
    while (i_counter > 0U)
    {
        pf_flash_test_1();
        pf_flash_test_2();
        i_counter--;
    }

    /* 计算本轮耗时。 */
    end_tick = HAL_GetTick();

    return (end_tick - start_tick);
}

/**
 * @brief 循环调用两个 SRAM 测试函数 10000000 次并计时。
 * @return 测试耗时，单位为毫秒。
 * @note 使用 myram 段；包含循环、调用及中断和调度开销。
 */
__attribute__((used, section("myram")))
static uint32_t SCT_MeasureSramTime(void)
{
    uint32_t start_tick = HAL_GetTick();  /* 起始毫秒计数 */
    uint32_t end_tick = 0U;               /* 结束毫秒计数 */
    uint32_t i_counter = 10000000U;       /* 剩余循环次数 */

    /* 固定测试入口，保留 volatile。 */
    void (* volatile pf_sram_test_1)(void) = SCT_SramTestFunction1;
    void (* volatile pf_sram_test_2)(void) = SCT_SramTestFunction2;

    /* 重复调用两个 SRAM 测试函数。 */
    while (i_counter > 0U)
    {
        pf_sram_test_1();
        pf_sram_test_2();
        i_counter--;
    }

    /* 计算本轮耗时。 */
    end_tick = HAL_GetTick();

    return (end_tick - start_tick);
}

/**
 * @brief 测量并记录 Flash/SRAM 耗时，每轮延时 1000 毫秒。
 * @param[in] p_argument : 任务参数，未使用，可为空。
 * @return 无；任务持续运行。
 * @note 由 OS_Init() 创建，运行前须初始化日志。
 */
void SCT_Test_TaskEntry(void *p_argument)
{
    uint32_t flash_elapsed_ms = 0U;  /* Flash 测试耗时，单位为毫秒 */
    uint32_t sram_elapsed_ms = 0U;   /* SRAM 测试耗时，单位为毫秒 */

    (void)p_argument;

    for (;;)
    {
        /* 1. 测量并记录 Flash 耗时。 */
        flash_elapsed_ms = SCT_MeasureFlashTime();
        elog_i("sct_test_task",
               "Flash execution time = [%lu] ms\r\n",
               (unsigned long)flash_elapsed_ms);

        /* 2. 测量并记录 SRAM 耗时。 */
        sram_elapsed_ms = SCT_MeasureSramTime();
        elog_i("sct_test_task",
               "SRAM execution time = [%lu] ms\r\n",
               (unsigned long)sram_elapsed_ms);

        /* 3. 延时 1000 毫秒后重复。 */
        vTaskDelay(pdMS_TO_TICKS(1000U));
    }
}

/************************ Function Definitions ************************/
