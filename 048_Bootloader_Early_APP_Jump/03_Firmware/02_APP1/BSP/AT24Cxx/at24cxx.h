/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file at24cxx.h
 *
 * @par dependencies
 * - main.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares EEPROM device settings and data access operations.
 *
 * Processing flow:
 *
 * Detect the device with ee_check_ok(), then call the data access functions.
 * Call ee_test() repeatedly to verify the last page with changing test data.
 *
 * @version V1.0 2026-10-01
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __ATC24CXX_DRIVER_H__
#define __ATC24CXX_DRIVER_H__

/****************************** Includes ******************************/
#include "main.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/*
 * AT24C02 容量为 2048 位，即 256 字节，共 32 页，每页 8 个字节。
 *
 * 设备地址字节依次包含固定设备标识、三个地址选择位和读写方向位。
 * 三个地址选择位均为零时，写地址为 0xA0，读地址为 0xA1。
 *
 * AT24C01/02 每页 8 个字节，AT24C04/08A/16A 每页 16 个字节。
 */
#define EEPROM_DEV_ADDR  0xA0 /* 设备地址字节，最低位为写方向。 */
#define EEPROM_PAGE_SIZE 8    /* 单页容量，单位为字节。 */
#define EEPROM_SIZE      256  /* 总容量，单位为字节。 */

/* 调试日志由 at24cxx.c 中的 debug 宏控制：1 开启，0 关闭。 */
// #define debug
/****************************** Defines ******************************/

/************************ Function Declarations ************************/

/**
 * @brief 检测串行存储器是否应答。
 *
 * @return 设备检测结果。
 * @retval 1 设备已应答。
 * @retval 0 设备未应答。
 */
uint8_t ee_check_ok(void);

/**
 * @brief 从指定地址开始连续读取多个字节。
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
                      uint16_t byte_count);

/**
 * @brief 从指定地址开始按页边界写入多个字节。
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
                       uint16_t byte_count);

/**
 * @brief 将全部存储区域写为擦除值。
 *
 * @return 无。
 */
void ee_erase(void);

/**
 * @brief 执行串行存储器最后一页的读写测试。
 *
 * 每次调用执行一轮测试，仅覆盖 0xF8 至 0xFF 的八个字节。
 * 测试值跨调用递增，字节值溢出后回到零。
 * 写入后轮询应答，等待内部写入完成，超时上限为十毫秒。
 * 读取后打印每个地址的写入值、读取值及整页比较结果。
 * AT24C02 不支持读取芯片标识，测试仅打印已应答的设备地址。
 *
 * @note 等待过程依赖持续运行的毫秒计数，不应在中断中调用。
 *
 * @return 读写测试结果。
 * @retval 1 读写成功且数据一致。
 * @retval 0 设备未应答、写入超时、读写失败或数据不一致。
 */
uint8_t ee_test(void);
/************************ Function Declarations ************************/

#endif /* __ATC24CXX_DRIVER_H__ */
