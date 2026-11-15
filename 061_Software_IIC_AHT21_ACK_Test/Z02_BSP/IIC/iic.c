/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file iic.c
 *
 * @par dependencies
 * - iic.h
 * - delay.h
 *
 * @author JesMicro
 *
 * @brief Implements GPIO-based IIC signaling and register transfers.
 *
 * Processing flow:
 *
 * Configure the bus pins through the supplied GPIO port and pin descriptor.
 * Generate START, STOP, data and acknowledgment sequences with DWT delays.
 * Provide single-byte and multi-byte target register access.
 *
 * @version v1.0 2026-11-06
 *
 * @note 1 tab == 4 spaces!
 *       Enable the GPIO port clocks and call Delay_Init before transfers.
 *       The caller supplies valid pointers and buffer sizes; inputs are not
 *       checked.
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "iic.h"                  /* Module header                            */

#include "delay.h"                /* User modules                             */

/********************************** Includes **********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Configure SDA as an input with an internal pull-up.
 *
 * @param[in]  p_bus : Valid bus descriptor with the SDA port clock enabled.
 *
 * @return None.
 *
 * @note Does not validate the descriptor or its pin configuration.
 */
void IIC_SetSdaInputMode(iic_bus_t *p_bus)
{
    GPIO_InitTypeDef gpio_init = {0};

    gpio_init.Pin   =       p_bus->sda_pin;
    gpio_init.Mode  =      GPIO_MODE_INPUT;
    gpio_init.Pull  =          GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p_bus->p_sda_port, &gpio_init);
}

/**
 * @brief Configure SDA as an open-drain output without an internal pull-up.
 *
 * @param[in]  p_bus : Valid bus descriptor with the SDA port clock enabled.
 *
 * @return None.
 *
 * @note Does not change the SDA output latch.
 */
void IIC_SetSdaOutputMode(iic_bus_t *p_bus)
{
    GPIO_InitTypeDef gpio_init = {0};

    gpio_init.Pin   =       p_bus->sda_pin;
    gpio_init.Mode  =  GPIO_MODE_OUTPUT_OD;
    gpio_init.Pull  =          GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p_bus->p_sda_port, &gpio_init);
}

/**
 * @brief Set the SDA output latch according to the requested level.
 *
 * @param[in]  p_bus : Valid bus descriptor.
 * @param[in]  level : Zero clears the output; any nonzero value sets it.
 *
 * @return None.
 *
 * @note The electrical pin state also depends on its current GPIO mode.
 */
void IIC_WriteSda(iic_bus_t *p_bus, uint16_t level)
{
    if (level)
    {
        p_bus->p_sda_port->BSRR |= p_bus->sda_pin;
    }
    else
    {
        p_bus->p_sda_port->BSRR = (uint32_t)p_bus->sda_pin << 16U;
    }
}

/**
 * @brief Set the SCL output latch according to the requested level.
 *
 * @param[in]  p_bus : Valid bus descriptor.
 * @param[in]  level : Zero clears the output; any nonzero value sets it.
 *
 * @return None.
 *
 * @note The electrical pin state also depends on its current GPIO mode.
 */
void IIC_WriteScl(iic_bus_t *p_bus, uint16_t level)
{
    if (level)
    {
        p_bus->p_scl_port->BSRR |= p_bus->scl_pin;
    }
    else
    {
        p_bus->p_scl_port->BSRR = (uint32_t)p_bus->scl_pin << 16U;
    }
}

/**
 * @brief Read the current SDA input level.
 *
 * @param[in]  p_bus : Valid bus descriptor.
 *
 * @return uint8_t : Sampled SDA level.
 * @retval 1 SDA is high.
 * @retval 0 SDA is low.
 */
uint8_t IIC_ReadSda(iic_bus_t *p_bus)
{
    if (HAL_GPIO_ReadPin(p_bus->p_sda_port, p_bus->sda_pin) == GPIO_PIN_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Generate an IIC START or repeated START condition.
 *
 * @param[in]  p_bus : Valid bus descriptor with GPIO outputs configured.
 *
 * @return None.
 *
 * @note Requires an initialized, running DWT delay counter.
 */
void IIC_Start(iic_bus_t *p_bus)
{
    /* SDA falls while SCL is high to generate the START condition. */
    IIC_WriteSda(p_bus, 1);
    Delay_Us(2);
    IIC_WriteScl(p_bus, 1);
    Delay_Us(1);
    IIC_WriteSda(p_bus, 0);
    Delay_Us(1);
    IIC_WriteScl(p_bus, 0);
    Delay_Us(1);
}

/**
 * @brief Generate an IIC STOP condition.
 *
 * @param[in]  p_bus : Valid bus descriptor with GPIO outputs configured.
 *
 * @return None.
 *
 * @note Requires an initialized, running DWT delay counter.
 */
void IIC_Stop(iic_bus_t *p_bus)
{
    /* SDA rises while SCL is high to generate the STOP condition. */
    IIC_WriteScl(p_bus, 0);
    Delay_Us(2);
    IIC_WriteSda(p_bus, 0);
    Delay_Us(1);
    IIC_WriteScl(p_bus, 1);
    Delay_Us(1);
    IIC_WriteSda(p_bus, 1);
    Delay_Us(1);
}

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
unsigned char IIC_WaitAck(iic_bus_t *p_bus)
{
    unsigned short ack_retry_count = 5;

    IIC_SetSdaInputMode(p_bus);
    IIC_WriteScl(p_bus, 1);

    while (IIC_ReadSda(p_bus))
    {
        ack_retry_count--;
        Delay_Us(1);

        if (0 == ack_retry_count)
        {
            /* Restore output mode and terminate the transfer on timeout. */
            IIC_SetSdaOutputMode(p_bus);
            IIC_Stop(p_bus);
            return ERROR;
        }
    }

    IIC_SetSdaOutputMode(p_bus);
    IIC_WriteScl(p_bus, 0);
    Delay_Us(2);
    return SUCCESS;
}

/**
 * @brief Send an ACK clock pulse with SDA low.
 *
 * @param[in]  p_bus : Valid bus descriptor for the active read transfer.
 *
 * @return None.
 *
 * @note Leaves SCL low after the acknowledgment pulse.
 */
void IIC_SendAck(iic_bus_t *p_bus)
{
    IIC_WriteSda(p_bus, 0);
    Delay_Us(1);
    IIC_WriteScl(p_bus, 1);
    Delay_Us(1);
    IIC_WriteScl(p_bus, 0);
    Delay_Us(1);
}

/**
 * @brief Send a NACK clock pulse with SDA high.
 *
 * @param[in]  p_bus : Valid bus descriptor for the active read transfer.
 *
 * @return None.
 *
 * @note Leaves SCL low after the acknowledgment pulse.
 */
void IIC_SendNack(iic_bus_t *p_bus)
{
    IIC_WriteSda(p_bus, 1);
    Delay_Us(1);
    IIC_WriteScl(p_bus, 1);
    Delay_Us(1);
    IIC_WriteScl(p_bus, 0);
    Delay_Us(2);
}

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
void IIC_SendByte(iic_bus_t *p_bus, unsigned char tx_byte)
{
    unsigned char bit_index = 8;

    /* Transmit the most significant bit first. */
    while (bit_index--)
    {
        IIC_WriteScl(p_bus, 0);
        Delay_Us(2);
        IIC_WriteSda(p_bus, tx_byte & 0x80);
        Delay_Us(1);
        tx_byte += tx_byte;
        Delay_Us(1);
        IIC_WriteScl(p_bus, 1);
        Delay_Us(1);
    }

    IIC_WriteScl(p_bus, 0);
    Delay_Us(2);
}

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
unsigned char IIC_ReadByte(iic_bus_t *p_bus)
{
    unsigned char bit_index = 8;
    unsigned char rx_byte   = 0;

    IIC_SetSdaInputMode(p_bus);

    /* Assemble the byte by sampling SDA during each high SCL phase. */
    while (bit_index--)
    {
        rx_byte += rx_byte;
        IIC_WriteScl(p_bus, 0);
        Delay_Us(2);
        IIC_WriteScl(p_bus, 1);
        Delay_Us(1);
        rx_byte |= IIC_ReadSda(p_bus);
    }

    IIC_WriteScl(p_bus, 0);
    IIC_SetSdaOutputMode(p_bus);
    return rx_byte;
}

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
                                                                               )
{
    /*************** 1. Send device address ***************/
    IIC_Start(p_bus);
    IIC_SendByte(p_bus, device_address << 1);

    if (IIC_WaitAck(p_bus))
    {
        IIC_Stop(p_bus);
        return 1;
    }

    /************* 2. Send register and data **************/
    IIC_SendByte(p_bus, register_address);
    IIC_WaitAck(p_bus);
    IIC_SendByte(p_bus, tx_byte);
    IIC_WaitAck(p_bus);

    /****************** 3. Stop transfer ******************/
    IIC_Stop(p_bus);
    Delay_Us(1);
    return 0;
}

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
                                                                               )
{
    unsigned char byte_index;

    /*************** 1. Send device address ***************/
    IIC_Start(p_bus);
    IIC_SendByte(p_bus, device_address << 1);

    if (IIC_WaitAck(p_bus))
    {
        IIC_Stop(p_bus);
        return 1;
    }

    /************* 2. Send register and data **************/
    IIC_SendByte(p_bus, register_address);
    IIC_WaitAck(p_bus);

    for (byte_index = 0; byte_index < size; byte_index++)
    {
        IIC_SendByte(p_bus, p_tx_data[byte_index]);
        IIC_WaitAck(p_bus);
    }

    /****************** 3. Stop transfer ******************/
    IIC_Stop(p_bus);
    Delay_Us(1);
    return 0;
}

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
                                                                               )
{
    unsigned char rx_byte;

    /***************** 1. Select register *****************/
    IIC_Start(p_bus);
    IIC_SendByte(p_bus, device_address << 1);
    IIC_WaitAck(p_bus);
    IIC_SendByte(p_bus, register_address);
    IIC_WaitAck(p_bus);

    /*************** 2. Read register data ****************/
    IIC_Start(p_bus);
    IIC_SendByte(p_bus, (device_address << 1) + 1);
    IIC_WaitAck(p_bus);
    rx_byte = IIC_ReadByte(p_bus);

    /*************** 3. Send NACK and stop ****************/
    IIC_SendNack(p_bus);
    IIC_Stop(p_bus);
    return rx_byte;
}

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
                                                                               )
{
    unsigned char byte_index;

    /***************** 1. Select register *****************/
    IIC_Start(p_bus);
    IIC_SendByte(p_bus, device_address << 1);

    if (IIC_WaitAck(p_bus))
    {
        IIC_Stop(p_bus);
        return 1;
    }

    IIC_SendByte(p_bus, register_address);
    IIC_WaitAck(p_bus);

    /*************** 2. Read register data ****************/
    IIC_Start(p_bus);
    IIC_SendByte(p_bus, (device_address << 1) + 1);
    IIC_WaitAck(p_bus);

    for (byte_index = 0; byte_index < size; byte_index++)
    {
        p_rx_data[byte_index] = IIC_ReadByte(p_bus);

        if (byte_index < size - 1)
        {
            IIC_SendAck(p_bus);
        }
    }

    /*************** 3. Send NACK and stop ****************/
    IIC_SendNack(p_bus);
    IIC_Stop(p_bus);
    return 0;
}

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
void IIC_Init(iic_bus_t *p_bus)
{
    GPIO_InitTypeDef gpio_init = {0};

    /* Configure both pins as push-pull outputs with internal pull-ups. */
    gpio_init.Pin   =       p_bus->sda_pin;
    gpio_init.Mode  =  GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull  =          GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p_bus->p_sda_port, &gpio_init);

    gpio_init.Pin = p_bus->scl_pin;
    HAL_GPIO_Init(p_bus->p_scl_port, &gpio_init);
}

/**************************** Function Definitions ****************************/
