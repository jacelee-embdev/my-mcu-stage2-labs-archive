/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file common.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares shared conversion and serial helper interfaces.
 *
 * @version V1.0 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

/****************************** Includes ******************************/
#include <stdint.h>
/****************************** Includes ******************************/

/****************************** Functions ******************************/
/**
 * @brief 将十进制或十六进制字符串转换为整数。
 *
 * @param[in] p_input_string 待转换的字符串。
 * @param[out] p_integer 转换后的整数。
 *
 * @return 转换成功返回 1U，转换失败返回 0U。
 */
uint32_t Str2Int(uint8_t *p_input_string, int32_t *p_integer);

/**
 * @brief 查询串口是否接收到一个字节。
 *
 * @param[out] p_key 用于保存接收到的字节。
 *
 * @return 收到字节返回 1U，否则返回 0U。
 */
uint32_t SerialKeyPressed(uint8_t *p_key);

/**
 * @brief 通过 Bootloader 串口发送一个字节。
 *
 * @param[in] character 待发送的字节。
 *
 * @return 无。
 */
void SerialPutChar(uint8_t character);
/****************************** Functions ******************************/

#endif /* __COMMON_H__ */
