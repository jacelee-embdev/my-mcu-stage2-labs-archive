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
 * Create Status_TaskEntry() after logging initialization.
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
 * @brief Log the task status at a fixed tick interval.
 *
 * Processing steps:
 * 1. Output the status message.
 * 2. Wait 5000 ticks, then repeat.
 *
 * @param[in] p_argument : Unused task argument; may be NULL.
 *
 * @return None; the task runs continuously.
 *
 * @note Initialize logging before this task starts.
 */
void Status_TaskEntry(void * const p_argument);

/********************************* Declaring **********************************/

#endif  /* __STATUS_TASK_H__ */
