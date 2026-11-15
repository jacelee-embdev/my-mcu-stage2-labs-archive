/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file status_task.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the status test task.
 *
 * Processing flow:
 *
 * Create Status_TaskEntry() after LED and logging initialization.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __STATUS_TASK_H__
#define __STATUS_TASK_H__

/********************************* Declaring **********************************/

/**
 * @brief Run the status LED and logging test.
 *
 * Processing steps:
 * 1. Toggle the status LED.
 * 2. Log a status message.
 * 3. Delay for 5000 ms and repeat.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @return None. The task runs indefinitely.
 *
 * @note Initialize the LED and logging before starting this task.
 *       Keep each task in its own directory with its matching .c and .h files.
 */
void Status_TaskEntry(void *p_argument);

/********************************* Declaring **********************************/

#endif  /* __STATUS_TASK_H__ */
