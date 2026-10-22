/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file common.c
 *
 * @par dependencies
 * - stddef.h
 * - common.h
 * - stm32f4xx.h
 * - usart.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements conversion and serial helpers used by YMODEM.
 *
 * @version V1.0 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "common.h"

#include <stddef.h>

#include "stm32f4xx.h"
#include "usart.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
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
/****************************** Defines ******************************/

/**
 * @brief 将十进制或十六进制字符串转换为整数。
 *
 * 支持普通十进制、以 0x 开头的十六进制，以及 K、M 数量级后缀。
 *
 * @param[in] p_input_string 待转换的字符串。
 * @param[out] p_integer 转换后的整数。
 *
 * @return 转换成功返回 1U，转换失败返回 0U。
 */
uint32_t Str2Int(uint8_t *p_input_string, int32_t *p_integer)
{
    uint32_t index = 0U;
    uint32_t result = 0U;
    uint32_t value = 0U;

    if ((NULL == p_input_string) || (NULL == p_integer))
    {
        return 0U;
    }

    // 解析以 0x 或 0X 开头的十六进制字符串。
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
        // 解析十进制字符串及其 K、M 数量级后缀。
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
 * @brief 查询串口是否接收到一个字节。
 *
 * @param[out] p_key 用于保存接收到的字节。
 *
 * @return 收到字节返回 1U，否则返回 0U。
 */
uint32_t SerialKeyPressed(uint8_t *p_key)
{
    if (NULL == p_key)
    {
        return 0U;
    }

    if (RESET != USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
    {
        *p_key = (uint8_t)USART1->DR;
        return 1U;
    }

    return 0U;
}

/**
 * @brief 通过 Bootloader 串口发送一个字节。
 *
 * @param[in] character 待发送的字节。
 *
 * @return 无。
 */
void SerialPutChar(uint8_t character)
{
    USART_SendChar(USART1, character);
}
