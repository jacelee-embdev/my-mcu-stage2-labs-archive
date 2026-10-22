/******************************************************************************
 * @file keros_i2c_interface.c
 * @par dependencies
 * - keros_i2c_interface.h
 * - stddef.h
 * - stm32f4xx_hal.h
 * @brief Migrates the ECS100 software I2C transactions to a bare-metal HAL app.
 * Processing flow: initialize PB6/PB7, then exchange KEROS commands.
 * @version V0.3 2026-10-12
 * @note 1 tab == 4 spaces! External pull-ups are required on SCL and SDA.
 ******************************************************************************/

/****************************** Includes ******************************/
#include "keros_i2c_interface.h" /* BSP 层：软件 I2C 接口 */

#include <stddef.h>             /* C 标准库 */

#include "stm32f4xx_hal.h"       /* Core 层：HAL 和 CMSIS */
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define KEROS_I2C_PORT       GPIOB
#define KEROS_SCL_PIN        GPIO_PIN_6
#define KEROS_SDA_PIN        GPIO_PIN_7
#define KEROS_I2C_DELAY_US   5U
/****************************** Defines ******************************/

/************************ Variable Definitions ************************/
static uint32_t g_delay_cycles = 1U; /* 每个软件 I2C 延时间隔的周期数 */
/************************ Variable Definitions ************************/

/************************ Function Definitions ************************/
/** @brief 使用 DWT 周期计数延时，避免空循环被优化和主频依赖。 */
static void KerosI2C_Delay(void)
{
    uint32_t start = DWT->CYCCNT;

    while ((uint32_t)(DWT->CYCCNT - start) < g_delay_cycles)
    {
        /* volatile 周期计数器保证每轮重新读取。 */
    }
}

/** @brief 初始化 GPIOB 和 DWT；须在系统时钟配置完成后调用。 */
void I2CSetInitial(void)
{
    GPIO_InitTypeDef gpio_config = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN | KEROS_SDA_PIN,
                      GPIO_PIN_SET);
    gpio_config.Pin = KEROS_SCL_PIN | KEROS_SDA_PIN;
    gpio_config.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEROS_I2C_PORT, &gpio_config);

    /* 不清零计数器，允许其他模块同时使用 DWT。 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    g_delay_cycles = (HAL_RCC_GetHCLKFreq() / 1000000U) *
                     KEROS_I2C_DELAY_US;
    if (0U == g_delay_cycles)
    {
        g_delay_cycles = 1U;
    }
    I2CMasterStop();
}

/** @brief 按 ECS100 的命令流程生成 START 或 repeated START。 */
void I2CMasterStart(void)
{
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_RESET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_RESET);
}

/** @brief 生成 STOP，并保留总线空闲延时。 */
void I2CMasterStop(void)
{
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_RESET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
}

/**
 * @brief 高位先发，发送八位数据并读取第九个时钟的 ACK。
 * @param[in] data_byte 发送数据。
 * @return 0 为 ACK，1 为 NACK。
 */
uint8_t I2CMasterWrite(uint8_t data_byte)
{
    uint8_t bit_index = 0U;
    uint8_t nack = 1U;

    for (bit_index = 0U; bit_index < 8U; bit_index++)
    {
        HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN,
            (0U != (data_byte & 0x80U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        KerosI2C_Delay();
        HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_SET);
        KerosI2C_Delay();
        HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_RESET);
        data_byte <<= 1;
    }

    /* 开漏输出写 1 即释放 SDA，可直接采样输入寄存器。 */
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
    nack = (GPIO_PIN_SET == HAL_GPIO_ReadPin(KEROS_I2C_PORT,
                                            KEROS_SDA_PIN)) ? 1U : 0U;
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_RESET);
    KerosI2C_Delay();
    return nack;
}

/**
 * @brief 读取八位数据，然后发送 ACK 或 NACK。
 * @param[in] ack 非零发送 ACK，零发送 NACK。
 * @return 接收字节。
 */
uint8_t I2CMasterRead(uint8_t ack)
{
    uint8_t bit_index = 0U;
    uint8_t received = 0U;

    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_SET);
    for (bit_index = 0U; bit_index < 8U; bit_index++)
    {
        KerosI2C_Delay();
        HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_SET);
        KerosI2C_Delay();
        received <<= 1;
        if (GPIO_PIN_SET == HAL_GPIO_ReadPin(KEROS_I2C_PORT,
                                             KEROS_SDA_PIN))
        {
            received |= 1U;
        }
        HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_RESET);
    }

    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN,
                      (0U != ack) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_SET);
    KerosI2C_Delay();
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEROS_I2C_PORT, KEROS_SDA_PIN, GPIO_PIN_SET);
    return received;
}

/**
 * @brief 最多三次发送写地址，保持原驱动的 repeated START 重试。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @return 0 为成功，否则为地址 NACK。
 */
uint8_t I2CSendDeviceAddress(uint8_t device_address)
{
    uint8_t attempt = 0U;

    for (attempt = 0U; attempt < MAXATTEMPTS; attempt++)
    {
        I2CMasterStart();
        if (0U == I2CMasterWrite(device_address))
        {
            return 0U;
        }
    }
    return ERR_I2C_NO_ADDRESS_ACK;
}

/**
 * @brief 发出 repeated START，并发送读地址。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @return 0 为 ACK，1 为 NACK。
 */
uint8_t I2CMasterReceiver(uint8_t device_address)
{
    I2CMasterStart();
    return I2CMasterWrite(device_address | 1U);
}

/**
 * @brief 写入子地址和数据，并传播 ACK 错误。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @param[in] p_address 子地址；长度为零时允许为空。
 * @param[in] address_length 子地址长度。
 * @param[in] p_data 数据；长度为零时允许为空。
 * @param[in] data_length 数据长度。
 * @return 0 为成功，否则为 ERR_I2C_*。
 */
uint8_t I2CWrite(uint8_t device_address, uint8_t *p_address,
                 uint8_t address_length, uint8_t *p_data,
                 uint8_t data_length)
{
    uint8_t status = 0U;
    uint8_t index = 0U;

    if (((0U != address_length) && (NULL == p_address)) ||
        ((0U != data_length) && (NULL == p_data)))
    {
        I2CMasterStop();
        return ERR_I2C_INVALID_ARGUMENT;
    }
    status = I2CSendDeviceAddress(device_address);
    for (index = 0U; (index < address_length) && (0U == status); index++)
    {
        if (0U != I2CMasterWrite(p_address[index]))
        {
            status = ERR_I2C_NO_DATA_ACK;
        }
    }
    for (index = 0U; (index < data_length) && (0U == status); index++)
    {
        if (0U != I2CMasterWrite(p_data[index]))
        {
            status = ERR_I2C_NO_DATA_ACK;
        }
    }
    I2CMasterStop();
    return status;
}

/**
 * @brief 写子地址后读数据，并在最后一个字节发送 NACK。
 * @param[in] device_address 含 R/W 位的设备地址。
 * @param[in] p_address 子地址；长度为零时允许为空。
 * @param[in] address_length 子地址长度。
 * @param[out] p_data 接收缓冲区。
 * @param[in] data_length 非零读取长度。
 * @return 0 为成功，否则为 ERR_I2C_*。
 */
uint8_t I2CRead(uint8_t device_address, uint8_t *p_address,
                uint8_t address_length, uint8_t *p_data,
                uint8_t data_length)
{
    uint8_t status = 0U;
    uint8_t index = 0U;

    if ((NULL == p_data) || (0U == data_length) ||
        ((0U != address_length) && (NULL == p_address)))
    {
        I2CMasterStop();
        return ERR_I2C_INVALID_ARGUMENT;
    }
    status = I2CSendDeviceAddress(device_address);
    for (index = 0U; (index < address_length) && (0U == status); index++)
    {
        if (0U != I2CMasterWrite(p_address[index]))
        {
            status = ERR_I2C_NO_DATA_ACK;
        }
    }
    if ((0U == status) &&
        (0U != I2CMasterReceiver(device_address)))
    {
        status = ERR_I2C_NO_ADDRESS_ACK;
    }
    if (0U == status)
    {
        for (index = 0U; index < data_length; index++)
        {
            p_data[index] = I2CMasterRead(
                ((uint16_t)index + 1U < data_length) ? 1U : 0U);
        }
    }
    I2CMasterStop();
    return status;
}
/************************ Function Definitions ************************/
