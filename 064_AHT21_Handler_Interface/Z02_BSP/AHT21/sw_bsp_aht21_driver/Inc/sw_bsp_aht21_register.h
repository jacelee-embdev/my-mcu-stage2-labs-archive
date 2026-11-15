/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file sw_bsp_aht21_register.h
 *
 * @par dependencies
 * None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Defines the AHT21 IIC address bytes and measurement command.
 *
 * Processing flow:
 *
 * Use the write address byte to send the measurement command and its two
 * arguments in order. Use the read address byte to receive sensor data.
 *
 * @version v1.0 2026-11-09
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SW_BSP_AHT21_REG_H__
#define __SW_BSP_AHT21_REG_H__

/*********************************** Defines **********************************/

#define AHT21_IIC_ADDRESS     (0x38U)                           /* 7-bit addr. */
#define AHT21_REG_READ_ADDR   ((AHT21_IIC_ADDRESS << 1U) | 1U)  /* Read byte.  */
#define AHT21_REG_WRITE_ADDR  (AHT21_IIC_ADDRESS << 1U)         /* Write byte. */

#define AHT21_REG_MEASURE_CMD       0XAC /* Trigger a measurement.            */
#define AHT21_REG_MEASURE_CMD_ARGS1 0X33 /* First measurement argument.       */
#define AHT21_REG_MEASURE_CMD_ARGS2 0X00 /* Second measurement argument.      */

/*********************************** Defines **********************************/

#endif  /* __SW_BSP_AHT21_REG_H__ */
