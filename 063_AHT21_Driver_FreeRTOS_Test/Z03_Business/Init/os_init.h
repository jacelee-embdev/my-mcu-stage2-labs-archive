/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file os_init.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares startup registration for the application tasks.
 *
 * Processing flow:
 *
 * Create status_task and aht21_task before scheduler startup.
 *
 * @version v1.0 2026-10-19
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __OS_INIT_H__
#define __OS_INIT_H__

/********************************* Declaring **********************************/

/**
 * @brief Create the status and AHT21 tasks.
 *
 * @par Processing steps
 * 1. Create the status task.
 * 2. Create the AHT21 task.
 * 3. Log successful creation.
 *
 * @return None.
 *
 * @note Call once after LED and logging initialization and before starting
 *       the scheduler.
 *       Log the error and return if either task fails to be created.
 *       The task stack depth is measured in StackType_t elements.
 */
void OS_Init(void);

/********************************* Declaring **********************************/

#endif  /* __OS_INIT_H__ */
