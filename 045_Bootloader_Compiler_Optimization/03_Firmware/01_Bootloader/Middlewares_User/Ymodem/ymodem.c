/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file ymodem.c
 *
 * @par dependencies
 * - ymodem.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Receives a YMODEM firmware file into the external A region.
 *
 * Processing flow:
 *
 * Reset A-region metadata and validate the advertised file size. Write
 * only valid file bytes, acknowledge retransmissions without duplicate
 * writes, flush at end of data, and verify the final region length.
 *
 * @version v1.3 2026-10-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "ymodem.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
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
#define FLASH_SIZE                 (APP_BACKUP_SIZE)
/******************************* Defines *******************************/

/******************** Private Function Declarations ********************/
static int32_t Receive_Byte(uint8_t *p_character, uint32_t timeout);
static void Send_Byte(uint8_t character);
static int32_t Receive_Packet(uint8_t *p_data,
                              int32_t *p_length,
                              uint32_t timeout);
/******************** Private Function Declarations ********************/

/**
 * @brief 在指定轮询次数内接收一个字节。
 *
 * 持续查询串口接收状态，直至成功取得一个字节或轮询次数耗尽。
 *
 * @param[out] p_character 用于保存接收到的字节。
 * @param[in]  timeout     最大轮询次数。
 *
 * @return int32_t 字节接收结果。
 * @retval  0 接收成功。
 * @retval -1 超时或参数无效。
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
 * 本函数检查包类型、包序号及其反码。包尾 CRC16 字段被接收，
 * 当前不计算或验证其值。
 *
 * @param[out] p_data   用于保存完整数据包的缓冲区。
 * @param[out] p_length 数据区长度；0 表示结束，-1 表示发送端取消。
 * @param[in]  timeout  每个字节的最大轮询次数。
 *
 * @return int32_t 数据包接收结果。
 * @retval  0 正常接收。
 * @retval  1 用户终止。
 * @retval -1 接收失败。
 */
static int32_t Receive_Packet(uint8_t *p_data,
                              int32_t *p_length,
                              uint32_t timeout)
{
    uint16_t index = 0U;  /* 当前数据包或文件信息字段的遍历位置。 */
    uint16_t packet_size = 0U;  /* 当前数据包的数据区长度。 */
    uint8_t character = 0U;  /* 当前接收的字节或控制字符。 */

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
 * @brief 接收一个 YMODEM 固件文件并写入外部 A 区。
 *
 * 处理步骤：
 * 1. 重置 A 区管理状态，请求并解析文件信息。
 * 2. 检查文件大小是否为正且不超过 A 区容量。
 * 3. 逐包写入实际文件字节，重传只确认，末包不写入协议填充。
 * 4. 收到数据结束标志时检查接收长度并刷新尾部数据。
 * 5. 空文件名结束会话后，再确认 A 区已写入长度。
 * 当前不计算或验证 CRC16；接收阶段不修改内部 APP。
 *
 * @param[in,out] p_buffer 文件数据暂存缓冲区，至少 1024 字节。
 *
 * @return 会话结果；正值为完整接收的文件字节数。
 * @retval 0 发送端取消，或会话开始后连续接收错误超过阈值。
 * @retval -1 参数、文件信息、大小、有效长度或闪存写入失败。
 * @retval -3 收到用户终止字符。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer)
{
    /* 工程启动栈仅 1 KB，单接收会话的 1K 数据包保存在静态区。 */
    /* 完整数据包暂存区，不占用启动栈。 */
    static uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    uint8_t file_size[FILE_SIZE_LENGTH] = {0U};  /* 文件信息包中的长度字符串。 */
    uint8_t *p_file = NULL;  /* 文件信息包内的解析位置。 */
    uint8_t *p_buffer_write = p_buffer;  /* 当前文件数据的暂存缓冲区。 */
    int32_t index = 0;  /* 当前数据包或文件信息字段的遍历位置。 */
    int32_t packet_length = 0;  /* 当前数据区长度或结束、取消标记。 */
    int32_t session_done = 0;  /* 文件会话已经结束的标记。 */
    int32_t file_done = 0;  /* 当前文件的数据阶段已经结束的标记。 */
    int32_t packets_received = 0;  /* 下一个期望的包序号计数。 */
    int32_t errors = 0;  /* 会话开始后的连续接收错误计数。 */
    int32_t session_begin = 0;  /* 已经接收有效包的标记。 */
    int32_t size = 0;  /* 发送端声明的有效文件长度。 */
    uint32_t bytes_received = 0U;  /* 已写入缓冲区的有效文件字节数。 */

    if (NULL == p_buffer)
    {
        return -1;
    }

    Erase_Flash_Block(BLOCK_1);

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
                            if ((size <= 0) ||
                                (bytes_received != (uint32_t)size) ||
                                (0U != W25Q64_WriteData_End(BLOCK_1)))
                            {
                                Send_Byte(CA);
                                Send_Byte(CA);
                                return -1;
                            }
                            Send_Byte(ACK);
                            Send_Byte(CRC16);
                            file_done = 1;
                            break;

                        default:
                            if ((packets_received > 0) &&
                                (packet_data[PACKET_SEQNO_INDEX] ==
                                 ((packets_received - 1) & 0xFFU)))
                            {
                                /* ACK 丢失后重发的包只确认，不再次写入。 */
                                Send_Byte(ACK);
                                if (1 == packets_received)
                                {
                                    Send_Byte(CRC16);
                                }
                            }
                            else if ((packet_data[PACKET_SEQNO_INDEX] &
                                      0xFFU) !=
                                     (packets_received & 0xFFU))
                            {
                                Send_Byte(NAK);
                            }
                            else if (0 == packets_received)
                            {
                                // 第 0 包保存文件名和文件大小。
                                if (0U != packet_data[PACKET_HEADER])
                                {
                                    /* 本接口每次只安装一个文件。 */
                                    if (size > 0)
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        return -1;
                                    }
                                    p_file = &packet_data[PACKET_HEADER];

                                    // 跳过文件名，定位其后的文件大小字段。
                                    for (index = 0;
                                         (0U != *p_file) &&
                                         (index < FILE_NAME_LENGTH) &&
                                         (index < packet_length - 1);
                                         index++)
                                    {
                                        p_file++;
                                    }

                                    if ((0U != *p_file) ||
                                        (index >= packet_length - 1))
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
                                         (p_file <
                                          &packet_data[PACKET_HEADER +
                                                       packet_length - 1]) &&
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

                                    // 固件大小不得超过当前接收上限。
                                    if ((size <= 0) ||
                                        ((uint32_t)size > FLASH_SIZE))
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
                                uint32_t write_length;  /* 当前数据包实际需要保存的字节数。 */

                                if ((size <= 0) ||
                                    (bytes_received >= (uint32_t)size))
                                {
                                    Send_Byte(CA);
                                    Send_Byte(CA);
                                    return -1;
                                }
                                /* 最后一包只保存文件字节，忽略 YMODEM 填充。 */
                                write_length = (uint32_t)size - bytes_received;
                                if (write_length > (uint32_t)packet_length)
                                {
                                    write_length = (uint32_t)packet_length;
                                }
                                memcpy(p_buffer_write,
                                       &packet_data[PACKET_HEADER],
                                       write_length);
                                if (0U != W25Q64_WriteData(BLOCK_1,
                                                          p_buffer_write,
                                                          write_length))
                                {
                                    Send_Byte(CA);
                                    Send_Byte(CA);
                                    return -1;
                                }
                                bytes_received += write_length;
                                Send_Byte(ACK);
                            }

                            if (packet_data[PACKET_SEQNO_INDEX] ==
                                (packets_received & 0xFFU))
                            {
                                packets_received++;
                            }
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

    if ((size <= 0) || (Read_BlockSize(BLOCK_1) != (uint32_t)size))
    {
        return -1;
    }
    return size;
}
