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
 * @brief Declares the task that requests joint AHT21 measurements.
 *
 * Processing flow:
 *
 * Register AHT21_TaskEntry in OS_Init and run it after scheduler startup.
 * Initialize resources, create the handler worker and submit joint requests.
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
 * @brief Submit periodic joint temperature and humidity reading requests.
 *
 * @param[in] p_argument : Unused task argument. It may be NULL.
 *
 * @note Creates the handler worker after explicit resource initialization.
 *       Requests use a 10000 ms freshness threshold and a 2000 ms delay.
 */
void AHT21_TaskEntry(void *p_argument);

/********************************* Declaring **********************************/

#endif  /* __AHT21_TASK_H__ */
