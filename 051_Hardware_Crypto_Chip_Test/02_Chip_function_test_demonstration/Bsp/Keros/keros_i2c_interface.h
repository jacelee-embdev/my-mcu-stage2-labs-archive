/******************************************************************************
 * @file keros_i2c_interface.h
 * @par dependencies
 * - stdint.h
 * @brief Software I2C interface adapted from the ECS100 KEROS driver.
 * @version V0.3 2026-10-12
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

#ifndef KEROS_I2C_INTERFACE_H
#define KEROS_I2C_INTERFACE_H

/****************************** Includes ******************************/
#include <stdint.h> /* C 标准库 */
/****************************** Includes ******************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Defines ******************************/
#define KEROS_DEVID_ADDR         0x38U /* 含 R/W 位，7 位地址为 0x1C */
#define ERR_I2C_NO_ADDRESS_ACK   0x02U
#define ERR_I2C_NO_DATA_ACK      0x08U
#define ERR_I2C_INVALID_ARGUMENT 0x10U
#define MAXATTEMPTS              3U
/****************************** Defines ******************************/

/****************************** Declaring ******************************/
/** @brief 配置 PB6/PB7 开漏输出和 DWT 时基，释放软件 I2C 总线。 */
void I2CSetInitial(void);

/** @brief 生成 START 或 repeated START。 */
void I2CMasterStart(void);

/** @brief 生成 STOP，随后保持至少一个总线延时间隔。 */
void I2CMasterStop(void);

/**
 * @brief 发送一个字节并读取 ACK。
 * @param[in] data_byte 要发送的字节。
 * @return 0 表示收到 ACK，1 表示收到 NACK。
 */
uint8_t I2CMasterWrite(uint8_t data_byte);

/**
 * @brief 读取一个字节，并按参数发送 ACK 或 NACK。
 * @param[in] ack 非零发送 ACK，零发送 NACK。
 * @return 接收的字节。
 */
uint8_t I2CMasterRead(uint8_t ack);

/**
 * @brief 发送写地址，最多尝试三次。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @return 0 表示成功，否则返回 ERR_I2C_NO_ADDRESS_ACK。
 */
uint8_t I2CSendDeviceAddress(uint8_t device_address);

/**
 * @brief 生成 repeated START 并发送读地址。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @return 0 表示 ACK，1 表示 NACK。
 */
uint8_t I2CMasterReceiver(uint8_t device_address);

/**
 * @brief 写入子地址和数据；所有退出路径均发送 STOP。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @param[in] p_address 子地址；address_length 为零时允许为空。
 * @param[in] address_length 子地址字节数。
 * @param[in] p_data 数据；data_length 为零时允许为空。
 * @param[in] data_length 数据字节数。
 * @return 0 表示成功，否则返回 ERR_I2C_*。
 */
uint8_t I2CWrite(uint8_t device_address, uint8_t *p_address,
                 uint8_t address_length, uint8_t *p_data,
                 uint8_t data_length);

/**
 * @brief 写入子地址后读取数据，最后一字节发送 NACK。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @param[in] p_address 子地址；address_length 为零时允许为空。
 * @param[in] address_length 子地址字节数。
 * @param[out] p_data 接收缓冲区，不得为空。
 * @param[in] data_length 读取字节数，必须大于零。
 * @return 0 表示成功，否则返回 ERR_I2C_*。
 */
uint8_t I2CRead(uint8_t device_address, uint8_t *p_address,
                uint8_t address_length, uint8_t *p_data,
                uint8_t data_length);
/****************************** Declaring ******************************/

#ifdef __cplusplus
}
#endif
#endif /* KEROS_I2C_INTERFACE_H */
