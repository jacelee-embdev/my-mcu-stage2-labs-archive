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
 * @brief Declares string conversion and serial helper interfaces.
 *
 * Processing flow:
 *
 * Use Str2Int() for file-size parsing and SerialPutChar() for replies.
 * SerialKeyPressed() currently reports no received byte.
 *
 * @version v1.0 2026-10-02
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

/******************************* Includes *******************************/
#include <stdint.h>
/******************************* Includes *******************************/

/************************ Function Declarations ************************/
/**
 * @brief 将十进制或十六进制字符串转换为整数。
 *
 * 支持普通十进制、以 0x 或 0X 开头的十六进制，以及 K、M 大小后缀。
 * K、M 后缀大小写均可，分别按 1024 和 1048576 倍转换。
 *
 * @param[in] p_input_string 待转换的字符串。
 * @param[out] p_integer 转换成功后写入的整数。
 *
 * @return uint32_t 转换结果。
 * @retval 1U 转换成功。
 * @retval 0U 参数无效或字符串格式不符合当前转换规则。
 */
uint32_t Str2Int(uint8_t *p_input_string, int32_t *p_integer);

/**
 * @brief 提供当前不执行实际接收的串口查询接口。
 *
 * 当前接收链路由 DMA 和长度队列负责。
 * 本接口无论参数是否为空均返回 0，不修改输出字节。
 *
 * @param[out] p_key 字节输出参数，当前不会写入。
 *
 * @return 固定返回 0U。
 */
uint32_t SerialKeyPressed(uint8_t *p_key);

/**
 * @brief 通过 USART1 阻塞发送一个字节。
 *
 * 使用 HAL 串口发送接口，超时参数为 1000 毫秒。
 * 当前接口不向调用者反馈 HAL 的发送结果。
 *
 * @param[in] character 待发送的字节。
 *
 * @return 无。
 */
void SerialPutChar(uint8_t character);
/************************ Function Declarations ************************/

#endif /* __COMMON_H__ */
