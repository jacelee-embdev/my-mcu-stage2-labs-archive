/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file iic.h
 *
 * @par dependencies
 * - stdint.h
 * - stm32f4xx_hal.h
 *
 * @author JesMicro
 *
 * @brief Declares GPIO-based IIC bus configuration and register access.
 *
 * Processing flow:
 *
 * Enable the selected GPIO port clocks and prepare a valid bus descriptor.
 * Call IIC_Init to configure the pins and Delay_Init before timed transfers.
 * Use signaling primitives or register access functions as required.
 *
 * @version v1.0 2026-11-06
 *
 * @note 1 tab == 4 spaces!
 *       Calls are blocking and must not overlap on the same bus.
 *       Clock stretching waits and multi-controller arbitration are not
 *       implemented.
 *
 ******************************************************************************/

#ifndef __IIC_H__
#define __IIC_H__

/********************************** Includes **********************************/

#include <stdint.h>               /* C standard library                       */

#include "stm32f4xx_hal.h"        /* MCU SDK / HAL                            */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

typedef struct
{
    GPIO_TypeDef * p_sda_port;        /* SDA GPIO port.                       */
    GPIO_TypeDef * p_scl_port;        /* SCL GPIO port.                       */
    uint16_t          sda_pin;        /* SDA GPIO pin mask.                   */
    uint16_t          scl_pin;        /* SCL GPIO pin mask.                   */
} iic_bus_t;  /* Caller-supplied GPIO bus configuration */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

/**
 * @brief Generate an IIC START or repeated START condition.
 *
 * @param[in]  p_bus : Valid bus descriptor with GPIO outputs configured.
 *
 * @return None.
 *
 * @note Requires an initialized, running DWT delay counter.
 */
void IIC_Start(iic_bus_t *p_bus);

/**
 * @brief Generate an IIC STOP condition.
 *
 * @param[in]  p_bus : Valid bus descriptor with GPIO outputs configured.
 *
 * @return None.
 *
 * @note Requires an initialized, running DWT delay counter.
 */
void IIC_Stop(iic_bus_t *p_bus);

/**
 * @brief Wait for the target to acknowledge a transmitted byte.
 *
 * @param[in]  p_bus : Valid bus descriptor for the active transfer.
 *
 * @return unsigned char : Acknowledgment result.
 * @retval SUCCESS SDA was sampled low before the retry count expired.
 * @retval ERROR SDA remained high until the retry count expired.
 *
 * @note Each high sample consumes one of five retries and a 1 us delay.
 *       On timeout, restores SDA output mode and generates STOP.
 *       On success, restores SDA output mode before lowering SCL.
 */
unsigned char IIC_WaitAck(iic_bus_t *p_bus);

/**
 * @brief Send an ACK clock pulse with SDA low.
 *
 * @param[in]  p_bus : Valid bus descriptor for the active read transfer.
 *
 * @return None.
 *
 * @note Leaves SCL low after the acknowledgment pulse.
 */
void IIC_SendAck(iic_bus_t *p_bus);

/**
 * @brief Send a NACK clock pulse with SDA high.
 *
 * @param[in]  p_bus : Valid bus descriptor for the active read transfer.
 *
 * @return None.
 *
 * @note Leaves SCL low after the acknowledgment pulse.
 */
void IIC_SendNack(iic_bus_t *p_bus);

/**
 * @brief Transmit eight bits, most significant bit first.
 *
 * @param[in]  p_bus   : Valid bus descriptor for the active transfer.
 * @param[in]  tx_byte : Byte to transmit.
 *
 * @return None.
 *
 * @note Does not generate or sample the acknowledgment clock.
 *       Call IIC_WaitAck separately when an acknowledgment is required.
 */
void IIC_SendByte(iic_bus_t *p_bus, unsigned char tx_byte);

/**
 * @brief Receive eight bits, most significant bit first.
 *
 * @param[in]  p_bus : Valid bus descriptor for the active transfer.
 *
 * @return unsigned char : Received byte.
 *
 * @note Restores SDA output mode after receiving the byte.
 *       The caller sends ACK or NACK separately.
 */
unsigned char IIC_ReadByte(iic_bus_t *p_bus);

/**
 * @brief Configure the SDA and SCL pins as push-pull outputs.
 *
 * @param[in]  p_bus : Valid descriptor with both GPIO port clocks enabled.
 *
 * @return None.
 *
 * @note Enables internal pull-ups and selects high GPIO output speed.
 *       Does not set output latch levels or initialize the DWT delay counter.
 */
void IIC_Init(iic_bus_t *p_bus);

/**
 * @brief Write one byte to a target register.
 *
 * @param[in]  p_bus            : Valid bus descriptor.
 * @param[in]  device_address   : Unshifted 7-bit target address.
 * @param[in]  register_address : 8-bit register address.
 * @param[in]  tx_byte          : Byte to write.
 *
 * @return uint8_t : Initial write-address acknowledgment result.
 * @retval 0 The function reached the end of the write sequence.
 * @retval 1 The initial write address was not acknowledged.
 *
 * @note Only the initial write-address ACK controls the return value.
 *       Register and data ACK results are not checked.
 */
uint8_t IIC_WriteRegByte(
                            iic_bus_t *                                   p_bus,
                            uint8_t                              device_address,
                            uint8_t                            register_address,
                            uint8_t                                     tx_byte
                                                                              );

/**
 * @brief Write a sequence of bytes starting at a target register.
 *
 * @param[in]  p_bus            : Valid bus descriptor.
 * @param[in]  device_address   : Unshifted 7-bit target address.
 * @param[in]  register_address : 8-bit starting register address.
 * @param[in]  size             : Number of data bytes to transmit.
 * @param[in]  p_tx_data        : Readable buffer containing at least size
 *                                bytes.
 *
 * @return uint8_t : Initial write-address acknowledgment result.
 * @retval 0 The function reached the end of the write sequence.
 * @retval 1 The initial write address was not acknowledged.
 *
 * @note Only the initial write-address ACK controls the return value.
 *       Register and data ACK results are not checked.
 *       A zero size still sends the address and register phases.
 */
uint8_t IIC_WriteRegBytes(
                            iic_bus_t *                                   p_bus,
                            uint8_t                              device_address,
                            uint8_t                            register_address,
                            uint8_t                                        size,
                            uint8_t                                 p_tx_data[]
                                                                              );

/**
 * @brief Read one byte from a target register using a repeated START.
 *
 * @param[in]  p_bus            : Valid bus descriptor.
 * @param[in]  device_address   : Unshifted 7-bit target address.
 * @param[in]  register_address : 8-bit register address.
 *
 * @return unsigned char : Received byte; no transaction status is returned.
 *
 * @note ACK results are not checked; a failed ACK wait does not abort this
 *       function.
 *       Sends NACK after the received byte, then generates STOP.
 */
unsigned char IIC_ReadRegByte(
                            iic_bus_t *                                   p_bus,
                            uint8_t                              device_address,
                            uint8_t                            register_address
                                                                              );

/**
 * @brief Read a sequence of register bytes using a repeated START.
 *
 * @param[in]  p_bus            : Valid bus descriptor.
 * @param[in]  device_address   : Unshifted 7-bit target address.
 * @param[in]  register_address : 8-bit starting register address.
 * @param[in]  size             : Number of data bytes to receive.
 * @param[out] p_rx_data        : Writable buffer with capacity for at least
 *                                size bytes.
 *
 * @return uint8_t : Initial write-address acknowledgment result.
 * @retval 0 The function reached the end of the read sequence.
 * @retval 1 The initial write address was not acknowledged.
 *
 * @note Only the initial write-address ACK controls the return value.
 *       Other ACK results are not checked. Earlier bytes receive ACK; the final
 *       byte receives NACK.
 *       A zero size still sends the address and register phases, NACK and STOP.
 */
uint8_t IIC_ReadRegBytes(
                            iic_bus_t *                                   p_bus,
                            uint8_t                              device_address,
                            uint8_t                            register_address,
                            uint8_t                                        size,
                            uint8_t                                 p_rx_data[]
                                                                              );

/********************************* Declaring **********************************/

#endif  /* __IIC_H__ */
