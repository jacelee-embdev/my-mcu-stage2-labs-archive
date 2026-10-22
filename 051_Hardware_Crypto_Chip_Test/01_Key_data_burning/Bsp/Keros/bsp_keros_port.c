/******************************************************************************
 * @file bsp_keros_port.c
 *
 * @par dependencies
 * - bsp_keros_port.h
 * - stm32f4xx_hal.h
 * - keros_lib.h
 * - bsp_keros_soft_i2c.h
 *
 * @brief STM32 HAL and software I2C callbacks for the Keros SDK.
 *
 * Processing flow:
 * Encode 16-bit subaddresses, transfer data, and provide delays.
 * Wake the chip through two address attempts and command 0xF0.
 *
 * @version V0.3
 * @date 2026-10-12
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

/***************************** Includes *****************************/

#include "bsp_keros_port.h" /* 本模块接口 */

#include "stm32f4xx_hal.h" /* Core/HAL 层 */

#include "keros_lib.h" /* 芯片 SDK 层 */

#include "bsp_keros_soft_i2c.h" /* 用户 BSP 层 */

/***************************** Includes *****************************/

/*********************** Variable Definitions ***********************/

/* 首次地址唤醒的 ACK 电平，0 为 ACK。 */
static uint8_t s_wake_ack_first;
/* 第二次地址唤醒的 ACK 电平，0 为 ACK。 */
static uint8_t s_wake_ack_second;

/*********************** Variable Definitions ***********************/

/*********************** Function Definitions ***********************/

/**
 * @brief 取得最近两次地址唤醒操作采样到的应答电平。
 *
 * @param[out] first 第一次应答；可为 NULL，0 为 ACK，1 为 NACK。
 * @param[out] second 第二次应答；可为 NULL，0 为 ACK，1 为 NACK。
 *
 * @return 无。
 */
void KerosPort_GetWakeAcks(uint8_t *first, uint8_t *second)
{
    if (first != NULL)
    {
        *first = s_wake_ack_first;
    }
    if (second != NULL)
    {
        *second = s_wake_ack_second;
    }
}

/**
 * @brief 将 16 位子地址按高字节在前编码，并调用软件 I2C 读取。
 *
 * @param[in] sub_addr 芯片寄存器或协议子地址。
 * @param[in] read_len 读取字节数，调用方应保证在 0 至 255 之间。
 * @param[out] r_data 接收缓冲区，正长度时必须有效且容量足够。
 *
 * @return 底层 I2C 状态：0 成功，非 0 为通信错误。
 *
 * @note 此适配层不检查指针和长度。
 */
uint8_t keros_read_data(uint16_t sub_addr, int read_len, uint8_t *r_data)
{
    /* 按高字节在前编码的 16 位子地址。 */
    uint8_t bSubAddress[2];

    bSubAddress[0] = sub_addr >> 8;
    bSubAddress[1] = sub_addr & 0xff;

    return I2CRead(KEROS_DEVID_ADDR, bSubAddress, 2, r_data, read_len);
}

/**
 * @brief 将 16 位子地址按高字节在前编码，并调用软件 I2C 写入。
 *
 * @param[in] sub_addr 芯片寄存器或协议子地址。
 * @param[in] w_data 发送缓冲区，正长度时必须有效且容量足够。
 * @param[in] write_len 写入字节数，调用方应保证在 0 至 255 之间。
 *
 * @return 底层 I2C 状态：0 成功，非 0 为通信错误。
 *
 * @note 此适配层不检查指针和长度。
 */
uint8_t keros_write_data(uint16_t sub_addr, uint8_t *w_data, int write_len)
{
    /* 按高字节在前编码的 16 位子地址。 */
    uint8_t bSubAddress[2];

    bSubAddress[0] = sub_addr >> 8;
    bSubAddress[1] = sub_addr & 0xff;

    return I2CWrite(KEROS_DEVID_ADDR, bSubAddress, 2, w_data, write_len);
}

/**
 * @brief 使用 HAL 毫秒计数实现阻塞等待。
 *
 * @param[in] wait_time 等待时间，单位 ms。
 *
 * @return 无。
 *
 * @note 依赖 HAL_GetTick() 持续递增；不可在系统节拍停止的上下文调用。
 */
void keros_delay(uint32_t wait_time)
{
    uint32_t tick;
    tick = HAL_GetTick();
    while (HAL_GetTick() - tick < wait_time)
        ;
}

/**
 * @brief 发送两次地址唤醒，再发送 0xF0 唤醒命令。
 *
 * @return 最后一次 I2C 写事务的状态：0 成功，非 0 失败。
 *
 * @note 不控制物理电源；地址唤醒的 ACK 单独保存，供上层查询。
 * @note 返回后由调用方等待芯片就绪。
 */
uint8_t keros_power_on(void)
{
    /* 0xF0 为协议唤醒命令，不代表切换物理供电。 */
    uint8_t Data = 0xF0;

    I2CMasterStart();
    s_wake_ack_first = I2CMasterWrite(0x38);
    I2CMasterStart();
    s_wake_ack_second = I2CMasterWrite(0x38);

    return I2CWrite(KEROS_DEVID_ADDR, NULL, 0, &Data, 1);
}

/*********************** Function Definitions ***********************/
