/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file iic.h
 *
 * @par dependencies
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares software I2C operations and EEPROM GPIO definitions.
 *
 * Processing flow:
 *
 * Configure GPIO through i2c_check_device(), then use the bus signal
 * and byte transfer functions for EEPROM access.
 *
 * @version V1.0 2026-10-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef _IIC_H__
#define _IIC_H__

/****************************** Includes ******************************/
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/* 设备地址字节中的写方向位。 */
#define EEPROM_I2C_WR        0
/* 设备地址字节中的读方向位。 */
#define EEPROM_I2C_RD        1

#define EEPROM_I2C_GPIO_PORT GPIOB               /* 总线引脚所在端口。 */
#define EEPROM_I2C_GPIO_CLK  RCC_AHB1Periph_GPIOB /* 端口时钟标识。 */
#define EEPROM_I2C_SCL_PIN   GPIO_PIN_8          /* 时钟线引脚。 */
#define EEPROM_I2C_SDA_PIN   GPIO_PIN_9          /* 数据线引脚。 */

/* 将时钟线设置为高电平。 */
#define EEPROM_I2C_SCL_1() \
    HAL_GPIO_WritePin(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SCL_PIN, GPIO_PIN_SET);
/* 将时钟线设置为低电平。 */
#define EEPROM_I2C_SCL_0() \
    HAL_GPIO_WritePin(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SCL_PIN, GPIO_PIN_RESET);

/* 将数据线设置为高电平。 */
#define EEPROM_I2C_SDA_1() \
    HAL_GPIO_WritePin(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SDA_PIN, GPIO_PIN_SET);
/* 将数据线设置为低电平。 */
#define EEPROM_I2C_SDA_0() \
    HAL_GPIO_WritePin(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SDA_PIN, GPIO_PIN_RESET);

/* 读取数据线引脚的电平状态。 */
#define EEPROM_I2C_SDA_READ() \
    HAL_GPIO_ReadPin(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SDA_PIN)
/****************************** Defines ******************************/

/************************ Function Declarations ************************/

/**
 * @brief 发送总线起始信号。
 *
 * @return 无。
 */
void i2c_start(void);

/**
 * @brief 发送总线停止信号。
 *
 * @return 无。
 */
void i2c_stop(void);

/**
 * @brief 向总线设备发送一个字节，最高位优先。
 *
 * @param[in] data_byte 待发送的数据字节。
 *
 * @return 无。
 */
void i2c_send_byte(uint8_t data_byte);

/**
 * @brief 从总线设备读取一个字节，最高位优先。
 *
 * @return 读取到的数据字节。
 */
uint8_t i2c_read_byte(void);

/**
 * @brief 产生应答时钟并读取设备的应答状态。
 *
 * @return 设备应答状态。
 * @retval 0 设备已应答。
 * @retval 1 设备未应答。
 */
uint8_t i2c_wait_ack(void);

/**
 * @brief 发送应答信号。
 *
 * @return 无。
 */
void i2c_send_ack(void);

/**
 * @brief 发送非应答信号。
 *
 * @return 无。
 */
void i2c_send_nack(void);

/**
 * @brief 配置总线引脚，发送设备地址并检测设备应答。
 *
 * @param[in] device_address 设备地址字节，包含最低位的读写方向位。
 *
 * @return 设备检测结果。
 * @retval 0 设备已应答。
 * @retval 1 未检测到设备应答。
 */
uint8_t i2c_check_device(uint8_t device_address);
/************************ Function Declarations ************************/

#endif /* _IIC_H__ */
