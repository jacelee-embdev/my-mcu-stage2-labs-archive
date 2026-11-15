/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file debug.h
 *
 * @par dependencies
 * - elog.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares the EasyLogger debug-module initialization interface.
 *
 * Processing flow:
 *
 * Call debug_init() once during application startup before outputting any
 * EasyLogger messages.
 *
 * @version v1.0 2026-09-17
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef EASYLOGGER_DEBUG_H
#define EASYLOGGER_DEBUG_H

/****************************** Includes ******************************/
#include "elog.h"
/****************************** Includes ******************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Declaring ******************************/

/**
 * @brief Initializes and starts the EasyLogger debug module.
 *
 * Initializes the EasyLogger core, enables colored output, configures the log
 * formats, and enables synchronous log output.
 *
 * @return None.
 */
void debug_init(void);

/****************************** Declaring ******************************/

#ifdef __cplusplus
}
#endif

#endif /* End of EASYLOGGER_DEBUG_H */
