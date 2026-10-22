/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file iic.c
 *
 * @par dependencies
 * - iic.h
 * - elog.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides software I2C operations for the EEPROM interface.
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

/****************************** Includes ******************************/
#include "iic.h"
#include "elog.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG "iic"     /* 模块日志标签。 */
/****************************** Defines ******************************/

/************************ Private Function Definitions ************************/

/**
 * @brief 配置软件总线使用的引脚。
 *
 * 处理步骤：
 * 1. 使能端口时钟，配置时钟线和数据线为开漏输出。
 * 2. 发送停止信号，使总线进入空闲状态。
 *
 * @return 无。
 */
static void i2c_config_gpio(void)
{
    GPIO_InitTypeDef gpio_init_config = {0}; /* 引脚初始化配置。 */

    /* 1. 使能端口时钟。 */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* 2. 配置时钟线和数据线。 */
    gpio_init_config.Pin = EEPROM_I2C_SCL_PIN | EEPROM_I2C_SDA_PIN;
    gpio_init_config.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_config.Pull = GPIO_NOPULL;
    gpio_init_config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(EEPROM_I2C_GPIO_PORT, &gpio_init_config);

    /* 3. 发送停止信号，使总线上的设备回到待机状态。 */
    i2c_stop();
}

/**
 * @brief 执行软件总线的位操作延时。
 *
 * 使用固定次数的空循环提供延时。
 *
 * @return 无。
 */
static void i2c_delay(void)
{
    uint8_t delay_index; /* 延时循环计数。 */

    for (delay_index = 0; delay_index < 40; delay_index++);
}

/************************ Private Function Definitions ************************/

/************************ Public Function Definitions ************************/

/**
 * @brief 发送总线起始信号。
 *
 * 时钟线保持高电平时，使数据线产生下降沿，然后拉低时钟线。
 *
 * @return 无。
 */
void i2c_start(void)
{
    /* 时钟线为高电平时，数据线的下降沿表示起始信号。 */
    EEPROM_I2C_SCL_1();
    EEPROM_I2C_SDA_1();
    i2c_delay();
    EEPROM_I2C_SDA_0();
    i2c_delay();
    EEPROM_I2C_SCL_0();
    i2c_delay();
}

/**
 * @brief 发送总线停止信号。
 *
 * 时钟线保持高电平时，使数据线产生上升沿。
 *
 * @return 无。
 */
void i2c_stop(void)
{
    /* 时钟线为高电平时，数据线的上升沿表示停止信号。 */
    EEPROM_I2C_SDA_0();
    EEPROM_I2C_SCL_1();
    i2c_delay();
    EEPROM_I2C_SDA_1();
    i2c_delay();
}

/**
 * @brief 发送应答信号。
 *
 * 在应答时钟期间将数据线保持为低电平，随后释放数据线。
 *
 * @return 无。
 */
void i2c_send_ack(void)
{
    EEPROM_I2C_SDA_0();
    i2c_delay();
    EEPROM_I2C_SCL_1();
    i2c_delay();

    EEPROM_I2C_SCL_0();
    i2c_delay();

    EEPROM_I2C_SDA_1(); /* 释放数据线。 */
    i2c_delay();
}

/**
 * @brief 发送非应答信号。
 *
 * 在应答时钟期间将数据线保持为高电平。
 *
 * @return 无。
 */
void i2c_send_nack(void)
{
    EEPROM_I2C_SDA_1();
    i2c_delay();
    EEPROM_I2C_SCL_1();
    i2c_delay();
    EEPROM_I2C_SCL_0();
    i2c_delay();
}

/**
 * @brief 产生应答时钟并读取设备的应答状态。
 *
 * 处理步骤：
 * 1. 释放数据线并拉高时钟线。
 * 2. 读取数据线电平，记录应答状态。
 * 3. 拉低时钟线并返回应答状态。
 *
 * @return 设备应答状态。
 * @retval 0 设备已应答。
 * @retval 1 设备未应答。
 */
uint8_t i2c_wait_ack(void)
{
    uint8_t ack_status; /* 设备应答状态。 */

    /* 1. 释放数据线。 */
    EEPROM_I2C_SDA_1();
    i2c_delay();

    /* 2. 拉高时钟线，使设备输出应答信号。 */
    EEPROM_I2C_SCL_1();
    i2c_delay();

    /* 3. 读取数据线电平。 */
    if (EEPROM_I2C_SDA_READ())
    {
        ack_status = 1;
    }
    else
    {
        ack_status = 0;
    }
    EEPROM_I2C_SCL_0();
    i2c_delay();

    return ack_status;
}

/**
 * @brief 向总线设备发送一个字节。
 *
 * 按最高位优先的顺序发送八位数据。
 *
 * @param[in] data_byte 待发送的数据字节。
 *
 * @return 无。
 */
void i2c_send_byte(uint8_t data_byte)
{
    uint8_t bit_index; /* 当前发送位的索引。 */

    /* 先发送字节的最高位。 */
    for (bit_index = 0; bit_index < 8; bit_index++)
    {
        if (data_byte & 0x80)
        {
            EEPROM_I2C_SDA_1();
        }
        else
        {
            EEPROM_I2C_SDA_0();
        }
        i2c_delay();

        EEPROM_I2C_SCL_1();
        i2c_delay();
        EEPROM_I2C_SCL_0();
        i2c_delay();

        if (bit_index == 7)
        {
            EEPROM_I2C_SDA_1();
        }
        data_byte <<= 1;
        i2c_delay();
    }
}

/**
 * @brief 从总线设备读取一个字节。
 *
 * 按最高位优先的顺序采样数据线，将八位数据组合为一个字节。
 *
 * @return 读取到的数据字节。
 */
uint8_t i2c_read_byte(void)
{
    uint8_t bit_index; /* 当前读取位的索引。 */
    uint8_t data_byte; /* 接收的数据字节。 */

    /* 第一次采样的数据作为字节的最高位。 */
    data_byte = 0;
    for (bit_index = 0; bit_index < 8; bit_index++)
    {
        data_byte <<= 1;
        EEPROM_I2C_SCL_1();
        i2c_delay();
        if (EEPROM_I2C_SDA_READ())
        {
            data_byte++;
        }
        EEPROM_I2C_SCL_0();
        i2c_delay();
    }
    return data_byte;
}

/**
 * @brief 发送设备地址并检测设备应答。
 *
 * 处理步骤：
 * 1. 配置总线引脚并发送起始信号。
 * 2. 发送带写方向位的设备地址，读取应答状态。
 * 3. 发送停止信号并返回应答状态。
 *
 * @param[in] device_address 设备地址字节，包含最低位的读写方向位。
 *
 * @return 设备检测结果。
 * @retval 0 设备已应答。
 * @retval 1 未检测到设备应答。
 */
uint8_t i2c_check_device(uint8_t device_address)
{
    uint8_t ack_status; /* 设备地址发送后的应答状态。 */

    i2c_config_gpio(); /* 配置总线引脚。 */
    i2c_start();       /* 发送起始信号。 */

    /* 先发送最高位，最低位表示读写方向。 */
    i2c_send_byte(device_address | EEPROM_I2C_WR);
    ack_status = i2c_wait_ack(); /* 读取设备应答。 */
    i2c_stop();                  /* 发送停止信号。 */
    return ack_status;
}

/************************ Public Function Definitions ************************/
