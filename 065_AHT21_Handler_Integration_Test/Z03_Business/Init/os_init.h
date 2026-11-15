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
 * The AHT21 business task initializes resources and creates its worker.
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
 * @brief Create the status task and the AHT21 business task.
 *
 * @par Processing steps
 * 1. Create the status task.
 * 2. Create the AHT21 business task.
 * 3. Log successful creation.
 *
 * @return None.
 *
 * @note Call once after LED and logging initialization and before starting
 *       the scheduler.
 *       Returns immediately if either task creation fails.
 *       The task stack depth is measured in StackType_t elements.
 */
void OS_Init(void);

/********************************* Declaring **********************************/

#endif  /* __OS_INIT_H__ */
