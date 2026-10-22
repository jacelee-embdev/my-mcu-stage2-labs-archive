/******************************************************************************
 * @file keros_interface.c
 * @par dependencies
 * - keros_interface.h
 * - stddef.h
 * - string.h
 * - stm32f4xx_hal.h
 * - keros_i2c_interface.h
 * @brief Performs real HAL transfers and preserves the first transport error.
 * @version V0.3 2026-10-13
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

/****************************** Includes ******************************/
#include "keros_interface.h"     /* BSP 层：厂家库平台接口 */

#include <stddef.h>             /* C 标准库 */
#include <string.h>

#include "stm32f4xx_hal.h"       /* Core 层：HAL 时基 */

#include "keros_i2c_interface.h" /* BSP 层：软件 I2C */
/****************************** Includes ******************************/

/************************ Variable Definitions ************************/
static keros_port_error_t g_first_error = {0U, 0U, 0U};
static keros_port_activity_t g_activity = {0U, 0U};
/************************ Variable Definitions ************************/

/************************ Function Definitions ************************/
/**
 * @brief 返回板级型号配置的 EEPROM 容量，不通过通用库猜测型号。
 * @return 配置容量，单位为字节。
 */
uint32_t KerosPort_GetEepromCapacity(void)
{
    return KEROS_EEPROM_CAPACITY_BYTES;
}

/** @brief 清除首次错误和传输计数，在进入一次完整库调用前使用。 */
void KerosPort_ClearError(void)
{
    g_first_error.status = 0U;
    g_first_error.sub_address = 0U;
    g_first_error.is_read = 0U;
    g_activity.read_calls = 0U;
    g_activity.write_calls = 0U;
}

/**
 * @brief 返回首次底层错误快照。
 * @return status 为 0 表示无错误，非零时包含子地址和读写方向。
 */
keros_port_error_t KerosPort_GetError(void)
{
    return g_first_error;
}

/**
 * @brief 返回自上次清除以来，实际提交给 I2C 驱动的读写次数。
 * @return 计数快照，不清除错误或计数。
 */
keros_port_activity_t KerosPort_GetActivity(void)
{
    return g_activity;
}

/**
 * @brief 锁存首次失败，避免厂家库后续操作掩盖错误。
 * @param[in] status 当前平台/I2C 状态。
 * @param[in] sub_address 当前芯片子地址。
 * @param[in] is_read 1 为读操作，0 为写操作。
 */
static void KerosPort_RecordError(uint8_t status, uint16_t sub_address,
                                  uint8_t is_read)
{
    if ((0U != status) && (0U == g_first_error.status))
    {
        g_first_error.status = status;
        g_first_error.sub_address = sub_address;
        g_first_error.is_read = is_read;
    }
}

/**
 * @brief 执行真实读取并锁存底层错误，不按配置屏蔽 EEPROM 地址。
 * @param[in] sub_address 芯片子地址。
 * @param[in] read_length 读取长度，范围 1~255。
 * @param[out] p_data 接收缓冲区。
 * @return 0 为成功，否则为平台/I2C 错误；失败时清零有效缓冲区。
 */
uint8_t keros_read_data(uint16_t sub_address, int read_length,
                        uint8_t *p_data)
{
    uint8_t address[2] = {0U};
    uint8_t status = 0U;

    if ((NULL == p_data) || (read_length <= 0) || (read_length > 255))
    {
        KerosPort_RecordError(ERR_I2C_INVALID_ARGUMENT, sub_address, 1U);
        return ERR_I2C_INVALID_ARGUMENT;
    }
    address[0] = (uint8_t)(sub_address >> 8);
    address[1] = (uint8_t)sub_address;
    g_activity.read_calls++;
    status = I2CRead(KEROS_DEVID_ADDR, address, 2U, p_data,
                      (uint8_t)read_length);
    if (0U != status)
    {
        memset(p_data, 0, (size_t)read_length);
    }
    KerosPort_RecordError(status, sub_address, 1U);
    return status;
}

/**
 * @brief 执行真实写入并锁存底层错误，不按配置屏蔽 EEPROM 地址。
 * @param[in] sub_address 芯片子地址。
 * @param[in] p_data 发送缓冲区。
 * @param[in] write_length 写入长度，范围 1~255。
 * @return 0 为成功，否则为平台/I2C 错误。
 */
uint8_t keros_write_data(uint16_t sub_address, uint8_t *p_data,
                         int write_length)
{
    uint8_t address[2] = {0U};
    uint8_t status = 0U;

    if ((NULL == p_data) || (write_length <= 0) || (write_length > 255))
    {
        KerosPort_RecordError(ERR_I2C_INVALID_ARGUMENT, sub_address, 0U);
        return ERR_I2C_INVALID_ARGUMENT;
    }
    address[0] = (uint8_t)(sub_address >> 8);
    address[1] = (uint8_t)sub_address;
    g_activity.write_calls++;
    status = I2CWrite(KEROS_DEVID_ADDR, address, 2U, p_data,
                       (uint8_t)write_length);
    KerosPort_RecordError(status, sub_address, 0U);
    return status;
}

/**
 * @brief 使用无符号差值等待毫秒时基，兼容 HAL tick 回绕。
 * @param[in] wait_time 等待时长，单位为毫秒。
 * @note 只能在 HAL 时基运行且中断开启的主循环调用。
 */
void keros_delay(uint32_t wait_time)
{
    uint32_t start = HAL_GetTick();

    while ((uint32_t)(HAL_GetTick() - start) < wait_time)
    {
        /* 保留 ECS100 的毫秒等待语义。 */
    }
}

/**
 * @brief 发送厂家唤醒序列，然后发送 Power ON 命令。
 * @return 最终 Power ON 命令的 I2C 状态。
 */
uint8_t keros_power_on(void)
{
    uint8_t data = 0xF0U;
    uint8_t status = 0U;

    /* 唤醒阶段的地址 ACK 不作成功判据，检查最终命令状态。 */
    I2CMasterStart();
    (void)I2CMasterWrite(KEROS_DEVID_ADDR);
    I2CMasterStart();
    (void)I2CMasterWrite(KEROS_DEVID_ADDR);
    status = I2CWrite(KEROS_DEVID_ADDR, NULL, 0U, &data, 1U);
    KerosPort_RecordError(status, 0U, 0U);
    return status;
}
/************************ Function Definitions ************************/
