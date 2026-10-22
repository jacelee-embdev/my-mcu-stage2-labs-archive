/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file at24cxx.c
 *
 * @par dependencies
 * - at24cxx.h
 * - iic.h
 * - elog.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides AT24C02 operations through the SPL software I2C driver.
 *
 * Processing flow:
 *
 * Detect the device with ee_check_ok(), then call the data access functions.
 * Initialize the millisecond time base before calling the read/write test.
 * Call ee_test() repeatedly to verify the last page with changing test data.
 *
 * @version V1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "at24cxx.h"
#include "iic.h"
#include "elog.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define TAG   "at24cxx" /* 模块日志标签。 */
#define debug 1         /* 调试日志开关：一开启，零关闭。 */
/****************************** Defines ******************************/

/************************ Function Definitions ************************/

/**
 * @brief 初始化总线引脚并检测串行存储器是否应答。
 *
 * 调用底层接口配置引脚、发送设备地址并检查应答。
 * 检测失败时再次发送总线停止信号。
 *
 * @return 设备检测结果。
 * @retval 1 设备已应答。
 * @retval 0 设备未应答。
 */
uint8_t ee_check_ok(void)
{
    if (i2c_check_device(EEPROM_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        /* 检测失败后发送总线停止信号。 */
        i2c_stop();
        return 0;
    }
}

/**
 * @brief 从指定地址开始连续读取多个字节。
 *
 * 处理步骤：
 * 1. 发送写方向的设备地址和读取起始地址。
 * 2. 重新发送起始信号，切换到读方向。
 * 3. 连续读取数据，最后一个字节后发送非应答信号。
 * 4. 发送停止信号并返回读取结果。
 *
 * @param[out] p_read_buf 接收读取数据的缓冲区指针。
 * @param[in] start_address 读取起始地址。
 * @param[in] byte_count 读取字节数。
 *
 * @return 数据读取结果。
 * @retval 1 读取成功。
 * @retval 0 设备未应答，读取失败。
 */
uint8_t ee_read_bytes(uint8_t *p_read_buf,
                      uint16_t start_address,
                      uint16_t byte_count)
{
    uint16_t byte_index; /* 当前读取字节的索引。 */

    /* 1. 发送起始信号。 */
    i2c_start();

    /* 2. 发送写方向的设备地址，准备传送读取起始地址。 */
    i2c_send_byte(EEPROM_DEV_ADDR | EEPROM_I2C_WR);

    /* 3. 等待设备应答。 */
    if (i2c_wait_ack() != 0)
    {
        goto cmd_fail; /* 设备未应答。 */
    }

    /* 4. 发送字节地址，当前器件使用一个地址字节。 */
    i2c_send_byte((uint8_t)start_address);

    /* 5. 等待设备应答。 */
    if (i2c_wait_ack() != 0)
    {
        goto cmd_fail; /* 设备未应答。 */
    }

    /* 6. 重新发送起始信号，准备读取数据。 */
    i2c_start();

    /* 7. 发送读方向的设备地址。 */
    i2c_send_byte(EEPROM_DEV_ADDR | EEPROM_I2C_RD);

    /* 8. 等待设备应答。 */
    if (i2c_wait_ack() != 0)
    {
        goto cmd_fail; /* 设备未应答。 */
    }

    /* 9. 连续读取数据。 */
    for (byte_index = 0; byte_index < byte_count; byte_index++)
    {
        p_read_buf[byte_index] = i2c_read_byte(); /* 读取一个字节。 */

        /* 中间字节后发送应答，最后一个字节后发送非应答。 */
        if (byte_index != byte_count - 1)
        {
            i2c_send_ack(); /* 数据线为低电平。 */
        }
        else
        {
            i2c_send_nack(); /* 数据线为高电平。 */
        }
    }

    /* 读取成功后发送停止信号。 */
    i2c_stop();
    return 1;

cmd_fail: /* 命令失败后发送停止信号，释放总线。 */

    i2c_stop();
    return 0;
}

/**
 * @brief 从指定地址开始按页边界写入多个字节。
 *
 * 处理步骤：
 * 1. 在首个字节或页边界处发送停止信号。
 * 2. 轮询设备应答，再发送当前字节地址。
 * 3. 发送数据并检查应答，递增当前地址。
 * 4. 发送停止信号并返回结果。
 *
 * @note 返回成功表示数据已发送，最后一页的内部写周期可能仍在进行。
 *       后续访问前应轮询设备应答，确认内部写入完成。
 *
 * @param[in] p_write_buf 待写入数据的缓冲区指针。
 * @param[in] start_address 写入起始地址。
 * @param[in] byte_count 写入字节数。
 *
 * @return 数据发送结果。
 * @retval 1 写入数据已发送。
 * @retval 0 设备未应答或应答重试次数已用尽。
 */
uint8_t ee_write_bytes(uint8_t *p_write_buf,
                       uint16_t start_address,
                       uint16_t byte_count)
{
    uint16_t byte_index, ack_retry_count; /* 字节索引及应答重试计数。 */
    uint16_t current_address;            /* 当前写入地址。 */

    /* 当前器件每页八个字节，连续写入时按页边界分段。 */
    current_address = start_address;
    for (byte_index = 0; byte_index < byte_count; byte_index++)
    {
        /* 首个字节或页首地址处重新发送起始信号和地址。 */
        if ((byte_index == 0) ||
            (current_address & (EEPROM_PAGE_SIZE - 1)) == 0)
        {
            /* 0. 发送停止信号，触发器件内部写操作。 */
            i2c_stop();

            /* 通过轮询设备应答，等待内部写操作结束。 */
            for (ack_retry_count = 0;
                 ack_retry_count < 1000;
                 ack_retry_count++)
            {
                /* 1. 发送起始信号。 */
                i2c_start();

                /* 2. 发送写方向的设备地址。 */
                i2c_send_byte(EEPROM_DEV_ADDR | EEPROM_I2C_WR);

                /* 3. 读取设备应答，收到应答后结束轮询。 */
                if (i2c_wait_ack() == 0)
                {
                    break;
                }
            }
            if (ack_retry_count == 1000)
            {
                goto cmd_fail; /* 应答重试次数已用尽。 */
            }

            /* 4. 发送字节地址，当前器件使用一个地址字节。 */
            i2c_send_byte((uint8_t)current_address);

            /* 5. 等待设备应答。 */
            if (i2c_wait_ack() != 0)
            {
                goto cmd_fail; /* 设备未应答。 */
            }
        }

        /* 6. 发送当前数据字节。 */
        i2c_send_byte(p_write_buf[byte_index]);

        /* 7. 等待设备应答。 */
        if (i2c_wait_ack() != 0)
        {
            goto cmd_fail; /* 设备未应答。 */
        }
        current_address++; /* 递增写入地址。 */
    }

    /* 数据发送成功后发送停止信号。 */
    i2c_stop();
    return 1;

cmd_fail: /* 命令失败后发送停止信号，释放总线。 */

    i2c_stop();
    return 0;
}

/**
 * @brief 将全部存储区域写为擦除值。
 *
 * 将缓冲区填充为全高电平字节，从地址零开始写入全部容量的数据。
 *
 * @return 无。
 */
void ee_erase(void)
{
    uint16_t byte_index;            /* 擦除缓冲区的字节索引。 */
    uint8_t erase_buf[EEPROM_SIZE]; /* 保存擦除值的缓冲区。 */

    /* 1. 将缓冲区填充为擦除值。 */
    for (byte_index = 0; byte_index < EEPROM_SIZE; byte_index++)
    {
        erase_buf[byte_index] = 0xFF;
    }

    /* 2. 从地址零开始写入整个缓冲区。 */
    if (ee_write_bytes(erase_buf, 0, EEPROM_SIZE) == 0)
    {
#if debug
        elog_e(TAG, "EEPROM erase failed.");
#endif
        return;
    }
    else
    {
#if debug
        elog_i(TAG, "EEPROM erase successful.");
#endif
    }
}

/**
 * @brief 执行读写测试应答轮询之间的软件延时。
 *
 * 在空循环中递减计数，直到计数为零。
 *
 * @param[in] delay_count 延时循环计数。
 *
 * @return 无。
 */
static void ee_delay(__IO uint32_t delay_count)
{
    for (; delay_count != 0; delay_count--)
        ;
}

/**
 * @brief 执行串行存储器最后一页的读写测试。
 *
 * 处理步骤：
 * 1. 检测设备应答，打印设备地址及不支持读取芯片标识的说明。
 * 2. 生成跨调用递增的数据，仅写入最后一页的八个字节。
 * 3. 轮询设备应答，等待内部写入完成，超时上限为十毫秒。
 * 4. 读取同一页，打印每个地址的写入值和读取值，并比较结果。
 *
 * AT24C02 不提供读取芯片标识的命令，设备地址不能作为芯片标识。
 * 当前测试范围为 0xF8 至 0xFF；递增值溢出后回到零。
 *
 * @note 系统滴答毫秒计数须持续递增；本测试会覆盖最后一页原有数据。
 *       完成等待期间不能关闭系统滴答中断，也不应在中断中调用。
 *
 * @return 读写测试结果。
 * @retval 1 读写成功且数据一致。
 * @retval 0 设备未应答、写入超时、读写失败或数据不一致。
 */
uint8_t ee_test(void)
{
    static uint8_t next_value = 0; /* 跨调用保留的递增测试值。 */
    /* 最后一页的页首地址。 */
    const uint16_t test_address = EEPROM_SIZE - EEPROM_PAGE_SIZE;
    uint8_t write_buf[EEPROM_PAGE_SIZE] = {0}; /* 一页待写入数据。 */
    uint8_t read_buf[EEPROM_PAGE_SIZE] = {0};  /* 一页读回数据。 */
    uint8_t compare_ok = 1;                   /* 本轮数据比较结果。 */
    uint8_t write_ready = 0;                  /* 内部写入完成标志。 */
    uint16_t byte_index = 0;                  /* 测试数据的字节索引。 */
    uint32_t write_start_tick = 0;            /* 写入等待的毫秒起点。 */

    /* 1. 检测设备，并由已有接口完成总线引脚配置。 */
    if (0 == ee_check_ok())
    {
#if debug
        elog_w(TAG, "Serial EEPROM not detected.");
#endif
        return 0;
    }

#if debug
    /* 器件没有可读的芯片标识，仅输出已收到应答的设备地址。 */
    elog_w(TAG, "AT24C02 does not support chip ID reads.");
    elog_i(TAG, "EEPROM ACK: addr7=0x%02X, write_addr=0x%02X",
           (unsigned int)(EEPROM_DEV_ADDR >> 1),
           (unsigned int)EEPROM_DEV_ADDR);
    elog_i(TAG, "Test page: 0x%02X-0x%02X",
           (unsigned int)test_address,
           (unsigned int)(test_address + EEPROM_PAGE_SIZE - 1));
#endif

    /* 2. 每轮使用下一组递增数据，字节值溢出后自然回到零。 */
    for (byte_index = 0; byte_index < EEPROM_PAGE_SIZE; byte_index++)
    {
        write_buf[byte_index] = next_value;
        next_value++;
    }

    /* 3. 仅覆盖最后一页，不跨越页边界。 */
    if (0 == ee_write_bytes(write_buf, test_address, EEPROM_PAGE_SIZE))
    {
#if debug
        elog_e(TAG, "EEPROM write failed.");
#endif
        return 0;
    }

    /* 4. 写入停止信号已触发内部写周期，以毫秒计数限制应答等待时间。 */
    write_start_tick = SysTickUpdateTime;
    do
    {
        if (1 == ee_check_ok())
        {
            write_ready = 1;
            break;
        }

        /* 两次应答检测之间保留短暂的软件延时。 */
        ee_delay(100);
    }
    while ((SysTickUpdateTime - write_start_tick) < 10);

    if (0 == write_ready)
    {
#if debug
        elog_e(TAG, "EEPROM write completion timeout.");
#endif
        return 0;
    }

    /* 5. 从最后一页的相同起始地址读取八个字节。 */
    if (0 == ee_read_bytes(read_buf, test_address, EEPROM_PAGE_SIZE))
    {
#if debug
        elog_e(TAG, "EEPROM read failed.");
#endif
        return 0;
    }

    /* 6. 打印所有字节的读写值，并记录整页比较结果。 */
    for (byte_index = 0; byte_index < EEPROM_PAGE_SIZE; byte_index++)
    {
#if debug
        elog_i(TAG, "addr=0x%02X, W=0x%02X, R=0x%02X",
               (unsigned int)(test_address + byte_index),
               (unsigned int)write_buf[byte_index],
               (unsigned int)read_buf[byte_index]);
#endif
        if (read_buf[byte_index] != write_buf[byte_index])
        {
            compare_ok = 0;
        }
    }

    /* 7. 输出本轮测试结果。 */
    if (0 == compare_ok)
    {
#if debug
        elog_e(TAG, "EEPROM read/write data mismatch.");
#endif
        return 0;
    }
#if debug
    elog_i(TAG, "EEPROM read/write test successful.");
#endif
    return 1;
}

/************************ Function Definitions ************************/
