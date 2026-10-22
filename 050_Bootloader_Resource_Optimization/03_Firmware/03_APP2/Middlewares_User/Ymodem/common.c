/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file common.c
 *
 * @par dependencies
 * - common.h
 * - stddef.h
 * - stm32f4xx.h
 * - usart.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides string conversion and UART transmit helpers.
 *
 * Processing flow:
 *
 * Convert file-size strings with Str2Int().
 * Send protocol-control bytes with SerialPutChar().
 * The serial polling interface currently returns no received byte.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************* Includes *********************************/
#include "common.h"

#include <stddef.h>

#include "stm32f4xx.h"
#include "usart.h"
/********************************* Includes *********************************/

/********************************* Defines **********************************/
/* 字符范围判断与进制转换，仅供当前文件使用。 */
#define IS_UPPER_HEX(character) \
    (((character) >= 'A') && ((character) <= 'F'))
#define IS_LOWER_HEX(character) \
    (((character) >= 'a') && ((character) <= 'f'))
#define IS_DECIMAL(character) \
    (((character) >= '0') && ((character) <= '9'))
#define IS_VALID_HEX(character) \
    (IS_UPPER_HEX(character) || IS_LOWER_HEX(character) || \
     IS_DECIMAL(character))
#define CONVERT_DECIMAL(character)    ((character) - '0')
#define CONVERT_HEX_ALPHA(character) \
    (IS_UPPER_HEX(character) ? ((character) - 'A' + 10) : \
                               ((character) - 'a' + 10))
#define CONVERT_HEX(character) \
    (IS_DECIMAL(character) ? CONVERT_DECIMAL(character) : \
                             CONVERT_HEX_ALPHA(character))
/********************************* Defines **********************************/

/******************************** Functions *********************************/
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
uint32_t Str2Int(uint8_t *p_input_string, int32_t *p_integer)
{
    uint32_t index = 0U;  /* 输入字符串的遍历下标。 */
    uint32_t result = 0U;  /* 转换成功标记。 */
    uint32_t value = 0U;  /* 累计转换值。 */

    if ((NULL == p_input_string) || (NULL == p_integer))
    {
        return 0U;
    }

    // 1. 解析以 0x 或 0X 开头的十六进制字符串。
    if (('0' == p_input_string[0]) &&
        (('x' == p_input_string[1]) || ('X' == p_input_string[1])))
    {
        if ('\0' == p_input_string[2])
        {
            return 0U;
        }

        for (index = 2U; index < 11U; index++)
        {
            if ('\0' == p_input_string[index])
            {
                *p_integer = (int32_t)value;
                result = 1U;
                break;
            }

            if (IS_VALID_HEX(p_input_string[index]))
            {
                value = (value << 4U) +
                        CONVERT_HEX(p_input_string[index]);
            }
            else
            {
                result = 0U;
                break;
            }
        }

        if (index >= 11U)
        {
            result = 0U;
        }
    }
    else
    {
        // 2. 解析十进制字符串及其大小后缀。
        for (index = 0U; index < 11U; index++)
        {
            if ('\0' == p_input_string[index])
            {
                *p_integer = (int32_t)value;
                result = 1U;
                break;
            }
            else if ((index > 0U) &&
                     (('k' == p_input_string[index]) ||
                      ('K' == p_input_string[index])))
            {
                value <<= 10U;
                *p_integer = (int32_t)value;
                result = 1U;
                break;
            }
            else if ((index > 0U) &&
                     (('m' == p_input_string[index]) ||
                      ('M' == p_input_string[index])))
            {
                value <<= 20U;
                *p_integer = (int32_t)value;
                result = 1U;
                break;
            }
            else if (IS_DECIMAL(p_input_string[index]))
            {
                value = (value * 10U) +
                        CONVERT_DECIMAL(p_input_string[index]);
            }
            else
            {
                result = 0U;
                break;
            }
        }

        if (index >= 11U)
        {
            result = 0U;
        }
    }

    return result;
}

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
uint32_t SerialKeyPressed(uint8_t *p_key)
{
    if (NULL == p_key)
    {
        return 0U;
    }

    return 0U;
}

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
void SerialPutChar(uint8_t character)
{
    // 协议层通过本接口发送请求、确认和取消等控制字节。
    (void)HAL_UART_Transmit(&huart1, &character, 1U, 1000U);
}
/******************************** Functions *********************************/
