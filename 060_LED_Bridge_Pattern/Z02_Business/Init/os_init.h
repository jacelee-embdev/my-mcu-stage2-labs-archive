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
 * @brief Declares task initialization.
 *
 * Processing flow:
 *
 * Create status_task and led_task once before scheduler startup.
 *
 * @version v1.0 2026-10-28
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __OS_INIT_H__
#define __OS_INIT_H__

/********************************* Declaring **********************************/

/**
 * @brief Create the application tasks before scheduler startup.
 *
 * Processing steps:
 * 1. Create the status task without retaining its handle.
 * 2. Create the LED test task without retaining its handle.
 * 3. Log and return on failure, or report that both tasks are ready.
 *
 * @return None.
 *
 * @note Call once after LED and logging initialization, before scheduler start.
 *       Stack depth is measured in StackType_t elements.
 *       A creation failure leaves previously created tasks in place.
 */
void OS_Init(void);

/********************************* Declaring **********************************/

#endif  /* __OS_INIT_H__ */
