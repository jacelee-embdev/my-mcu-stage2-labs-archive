/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file led_task.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the task entry point for repeated LED blink tests.
 *
 * Processing flow:
 *
 * Create LED_TaskEntry() from OS_Init() before scheduler startup.
 * The task initializes LED resources and submits repeated blink tests.
 * The handler executes queued requests while this task waits between groups.
 *
 * @version V1.0 2026-11-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __LED_TASK_H__
#define __LED_TASK_H__

/********************************* Declaring **********************************/

/**
 * @brief Initialize LED resources and repeat asynchronous blink tests.
 *
 * Processing steps:
 * 1. Construct and register LED resources through the system adapter.
 * 2. Use a loop factor to test 400, 800 and 1200 ms blink periods.
 * 3. For each period, test on/off ratios 1:1, 1:2 and 1:3, three blinks each.
 * 4. Wait for each group's nominal duration plus 2000 ms before submitting
 *    another request; log errors and delete this task on failure.
 *
 * @param[in] p_argument : Unused task argument; may be NULL.
 *
 * @return None; repeat tests continuously, or delete this task on failure.
 *
 * @note Created by OS_Init() and executed after scheduler startup.
 *       LED hardware and logging must already be initialized.
 *       Submission success only means queued; check Handler logs for execution
 *       results.
 *       The system adapter owns LED resources; this business task does not
 *       release background resources.
 */
void LED_TaskEntry(void * const p_argument);

/********************************* Declaring **********************************/

#endif  /* __LED_TASK_H__ */
