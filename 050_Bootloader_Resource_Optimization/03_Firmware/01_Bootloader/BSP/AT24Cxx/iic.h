/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file iic.h
 *
 * @par dependencies
 * - main.h
 * - stm32f4xx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares SPL software I2C operations and EEPROM GPIO definitions.
 *
 * Processing flow:
 *
 * Configure PB8/PB9 as open-drain outputs through i2c_check_device(),
 * then use the bus signal and byte transfer functions for EEPROM access.
 *
 * @version V1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef _IIC_H__
#define _IIC_H__

/****************************** Includes ******************************/
#include "main.h"
#include "stm32f4xx.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/* 设备地址字节中的写方向位。 */
#define EEPROM_I2C_WR        0
/* 设备地址字节中的读方向位。 */
#define EEPROM_I2C_RD        1

#define EEPROM_I2C_GPIO_PORT GPIOB               /* 总线引脚所在端口。 */
#define EEPROM_I2C_GPIO_CLK  RCC_AHB1Periph_GPIOB /* 端口时钟标识。 */
#define EEPROM_I2C_SCL_PIN   GPIO_Pin_8          /* 时钟线引脚。 */
#define EEPROM_I2C_SDA_PIN   GPIO_Pin_9          /* 数据线引脚。 */

/* 释放开漏时钟线，由外部上拉电阻拉高。 */
#define EEPROM_I2C_SCL_1() \
    GPIO_SetBits(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SCL_PIN);
/* 将开漏时钟线拉低。 */
#define EEPROM_I2C_SCL_0() \
    GPIO_ResetBits(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SCL_PIN);

/* 释放开漏数据线，由外部上拉电阻拉高或由设备输出。 */
#define EEPROM_I2C_SDA_1() \
    GPIO_SetBits(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SDA_PIN);
/* 将开漏数据线拉低。 */
#define EEPROM_I2C_SDA_0() \
    GPIO_ResetBits(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SDA_PIN);

/* 读取数据线引脚的实际电平：低返回零，高返回一。 */
#define EEPROM_I2C_SDA_READ() \
    GPIO_ReadInputDataBit(EEPROM_I2C_GPIO_PORT, EEPROM_I2C_SDA_PIN)
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
 * 每次调用均重新配置引脚，并以停止信号结束本次检测。
 *
 * @param[in] device_address 设备地址字节，包含最低位的读写方向位。
 *
 * @return 设备检测结果。
 * @retval 0 设备已应答。
 * @retval 1 未检测到设备应答。
 */
uint8_t i2c_check_device(uint8_t device_address);
/************************ Function Declarations ************************/

#endif /* 结束头文件重复包含保护。 */
