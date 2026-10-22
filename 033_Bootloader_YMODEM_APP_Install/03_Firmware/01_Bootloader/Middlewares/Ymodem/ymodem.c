/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file ymodem.c
 *
 * @par dependencies
 * - string.h
 * - ymodem.h
 * - common.h
 * - flash.h
 * - bootloader.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements YMODEM reception for Bootloader firmware updates.
 *
 * Processing flow:
 *
 * Receive the metadata packet, validate the image size, erase the APP Flash
 * region, receive each data packet, program it into Flash, and complete the
 * YMODEM session through ACK, NAK, CA, and CRC16 request control bytes.
 *
 * @version V1.0 2026-09-20
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "ymodem.h"

#include <string.h>

#include "common.h"
#include "flash.h"
#include "bootloader.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define PACKET_SEQNO_INDEX         (1U)
#define PACKET_SEQNO_COMP_INDEX    (2U)
#define PACKET_HEADER              (3U)
#define PACKET_TRAILER             (2U)
#define PACKET_OVERHEAD            (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE                (128U)
#define PACKET_1K_SIZE             (1024U)
#define FILE_NAME_LENGTH           (256U)
#define FILE_SIZE_LENGTH           (16U)

#define SOH                        (0x01U)
#define STX                        (0x02U)
#define EOT                        (0x04U)
#define ACK                        (0x06U)
#define NAK                        (0x15U)
#define CA                         (0x18U)
#define CRC16                      (0x43U)
#define ABORT1                     (0x41U)
#define ABORT2                     (0x61U)

#define NAK_TIMEOUT                (0x100000U)
#define MAX_ERRORS                 (3)
#define FLASH_SIZE                 (0x40000U)
/****************************** Defines ******************************/

/********************** Private Function Declarations **********************/
static int32_t Receive_Byte(uint8_t *p_character, uint32_t timeout);
static void Send_Byte(uint8_t character);
static int32_t Receive_Packet(uint8_t *p_data,
                              int32_t *p_length,
                              uint32_t timeout);
/********************** Private Function Declarations **********************/

/**
 * @brief 在指定轮询次数内接收一个字节。
 *
 * @param[out] p_character 用于保存接收到的字节。
 * @param[in] timeout 最大轮询次数。
 *
 * @return 成功返回 0，超时或参数无效返回 -1。
 */
static int32_t Receive_Byte(uint8_t *p_character, uint32_t timeout)
{
    if (NULL == p_character)
    {
        return -1;
    }

    while (timeout-- > 0U)
    {
        if (1U == SerialKeyPressed(p_character))
        {
            return 0;
        }
    }

    return -1;
}

/**
 * @brief 发送一个 YMODEM 控制字节。
 *
 * @param[in] character 待发送的控制字节。
 *
 * @return 无。
 */
static void Send_Byte(uint8_t character)
{
    SerialPutChar(character);
}

/**
 * @brief 接收并检查一个 YMODEM 数据包。
 *
 * 本函数检查包类型、包序号及其反码。当前版本保持原有行为，
 * 不在此处校验数据区末尾的 CRC16。
 *
 * @param[out] p_data 用于保存完整数据包的缓冲区。
 * @param[out] p_length 数据区长度；0 表示结束，-1 表示发送端取消。
 * @param[in] timeout 每个字节的最大轮询次数。
 *
 * @return 正常接收返回 0，用户终止返回 1，接收失败返回 -1。
 */
static int32_t Receive_Packet(uint8_t *p_data,
                              int32_t *p_length,
                              uint32_t timeout)
{
    uint16_t index = 0U;
    uint16_t packet_size = 0U;
    uint8_t character = 0U;

    if ((NULL == p_data) || (NULL == p_length))
    {
        return -1;
    }

    *p_length = 0;
    if (0 != Receive_Byte(&character, timeout))
    {
        return -1;
    }

    // 根据首字节确定数据包类型和有效数据长度。
    switch (character)
    {
        case SOH:
            packet_size = PACKET_SIZE;
            break;

        case STX:
            packet_size = PACKET_1K_SIZE;
            break;

        case EOT:
            return 0;

        case CA:
            if ((0 == Receive_Byte(&character, timeout)) &&
                (CA == character))
            {
                *p_length = -1;
                return 0;
            }

            return -1;

        case ABORT1:
        case ABORT2:
            return 1;

        default:
            return -1;
    }

    p_data[0] = character;
    for (index = 1U;
         index < (packet_size + PACKET_OVERHEAD);
         index++)
    {
        if (0 != Receive_Byte(&p_data[index], timeout))
        {
            return -1;
        }
    }

    // 包序号与其反码不匹配时，判定该数据包无效。
    if (p_data[PACKET_SEQNO_INDEX] !=
        ((p_data[PACKET_SEQNO_COMP_INDEX] ^ 0xFFU) & 0xFFU))
    {
        return -1;
    }

    *p_length = packet_size;
    return 0;
}

/**
 * @brief 通过 YMODEM 协议接收并写入一个 APP 固件。
 *
 * 处理步骤：
 * 1. 请求并解析文件信息包。
 * 2. 检查固件大小并擦除对应的 APP Flash 区域。
 * 3. 逐包接收数据、写入 Flash，并回读校验。
 * 4. 响应传输结束包和空文件名包，结束当前会话。
 *
 * @param[in] p_buffer 接收数据时使用的临时缓冲区。
 *
 * @return 成功时返回固件大小；失败时返回对应的负值或 0。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer)
{
    uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD] = {0U};
    uint8_t file_size[FILE_SIZE_LENGTH] = {0U};
    uint8_t *p_file = NULL;
    uint8_t *p_buffer_write = p_buffer;
    int32_t index = 0;
    int32_t packet_length = 0;
    int32_t session_done = 0;
    int32_t file_done = 0;
    int32_t packets_received = 0;
    int32_t errors = 0;
    int32_t session_begin = 0;
    int32_t size = 0;
    uint32_t flash_destination = APP_BASE_ADDR;
    uint32_t ram_source = 0U;

    if (NULL == p_buffer)
    {
        return -1;
    }

    // 持续接收文件，直到收到空文件信息包并结束会话。
    for (session_done = 0, errors = 0, session_begin = 0; ;)
    {
        for (packets_received = 0, file_done = 0,
             p_buffer_write = p_buffer; ;)
        {
            switch (Receive_Packet(packet_data,
                                   &packet_length,
                                   NAK_TIMEOUT))
            {
                case 0:
                    errors = 0;

                    switch (packet_length)
                    {
                        // 发送端主动取消传输。
                        case -1:
                            Send_Byte(ACK);
                            return 0;

                        // 收到文件数据结束标志。
                        case 0:
                            Send_Byte(ACK);
                            file_done = 1;
                            break;

                        default:
                            if ((packet_data[PACKET_SEQNO_INDEX] & 0xFFU) !=
                                (packets_received & 0xFFU))
                            {
                                Send_Byte(NAK);
                            }
                            else if (0 == packets_received)
                            {
                                // 第 0 包保存文件名和文件大小。
                                if (0U != packet_data[PACKET_HEADER])
                                {
                                    p_file = &packet_data[PACKET_HEADER];

                                    // 跳过文件名，定位其后的文件大小字段。
                                    for (index = 0;
                                         (0U != *p_file) &&
                                         (index < FILE_NAME_LENGTH);
                                         index++)
                                    {
                                        p_file++;
                                    }

                                    if (0U != *p_file)
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        return -1;
                                    }

                                    p_file++;

                                    // 复制文件大小字符串，并保证以空字符结尾。
                                    for (index = 0;
                                         (' ' != *p_file) &&
                                         ('\0' != *p_file) &&
                                         (index <
                                          (FILE_SIZE_LENGTH - 1U));
                                         index++)
                                    {
                                        file_size[index] = *p_file++;
                                    }
                                    file_size[index] = '\0';

                                    if (1U != Str2Int(file_size, &size))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        return -1;
                                    }

                                    // 固件不得超过当前允许的 Flash 容量。
                                    if (size > (FLASH_SIZE - 1U))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        return -1;
                                    }

                                    // 擦除即将写入固件的 APP Flash 区域。
                                    if (1U == Flash_erase(APP_BASE_ADDR,
                                                          (uint32_t)size))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        return -1;
                                    }

                                    Send_Byte(ACK);
                                    Send_Byte(CRC16);
                                }
                                else
                                {
                                    // 空文件信息包表示整个会话结束。
                                    Send_Byte(ACK);
                                    file_done = 1;
                                    session_done = 1;
                                    break;
                                }
                            }
                            else
                            {
                                // 将当前数据包写入 APP Flash 并回读校验。
                                memcpy(p_buffer_write,
                                       &packet_data[PACKET_HEADER],
                                       (uint32_t)packet_length);
                                ram_source = (uint32_t)p_buffer;

                                for (index = 0;
                                     (index < packet_length) &&
                                     (flash_destination <
                                      (APP_BASE_ADDR + (uint32_t)size));
                                     index += 4)
                                {
                                    Flash_Write(
                                        flash_destination,
                                        *(uint32_t *)ram_source);

                                    if (*(uint32_t *)flash_destination !=
                                        *(uint32_t *)ram_source)
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        return -2;
                                    }

                                    flash_destination += 4U;
                                    ram_source += 4U;
                                }

                                Send_Byte(ACK);
                            }

                            packets_received++;
                            session_begin = 1;
                            break;
                    }
                    break;

                case 1:
                    Send_Byte(CA);
                    Send_Byte(CA);
                    return -3;

                default:
                    if (session_begin > 0)
                    {
                        errors++;
                    }

                    if (errors > MAX_ERRORS)
                    {
                        Send_Byte(CA);
                        Send_Byte(CA);
                        return 0;
                    }

                    Send_Byte(CRC16);
                    break;
            }

            if (0 != file_done)
            {
                break;
            }
        }

        if (0 != session_done)
        {
            break;
        }
    }

    return size;
}
