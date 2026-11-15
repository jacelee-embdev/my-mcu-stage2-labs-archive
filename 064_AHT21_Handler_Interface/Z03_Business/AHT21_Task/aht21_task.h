/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aht21_task.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the AHT21 measurement task entry point.
 *
 * Processing flow:
 *
 * Register AHT21_TaskEntry in OS_Init and run it after scheduler startup.
 * The task initializes the driver and logs periodic sensor measurements.
 *
 * @version v1.0 2026-11-09
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __AHT21_TASK_H__
#define __AHT21_TASK_H__

/********************************* Declaring **********************************/

/**
 * @brief Initialize the AHT21 and log periodic temperature and humidity.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @note Start after GPIO, the bus delay counter, and logging are initialized.
 *       On initialization failure, the task logs the error and deletes itself.
 *       A failed measurement is logged before the next two-second wait.
 */
void AHT21_TaskEntry(void *p_argument);

/********************************* Declaring **********************************/

#endif  /* __AHT21_TASK_H__ */
