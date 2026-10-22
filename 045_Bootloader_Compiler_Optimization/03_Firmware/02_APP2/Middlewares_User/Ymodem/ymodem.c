/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file ymodem.c
 *
 * @par dependencies
 * - ymodem.h
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - usart.h
 * - os_init.h
 * - swc_ota.h
 * - debug.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Receives YMODEM data and synchronizes dual-buffer payload writes.
 *
 * Processing flow:
 *
 * Prepare UART DMA before receive-control replies. Validate file metadata,
 * hand off valid payload pointers, and wait for processing completion.
 * Count actual file bytes, ignore final packet padding, and reject incomplete
 * or failed downloads. Packet CRC16 is not calculated or validated.
 *
 * @version v1.2 2026-10-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "ymodem.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"
#include "os_init.h"
#include "swc_ota.h"

#include "debug.h"

/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define TAG "ymodem"  /* 日志模块标识。 */
/* 包结构、控制字节及接收限制；超时时间单位为毫秒。 */
#define YMODEM_RX_BUFFER_SIZE       (1030U)
#define PACKET_SEQNO_INDEX          (1U)
#define PACKET_SEQNO_COMP_INDEX     (2U)
#define PACKET_HEADER               (3U)
#define PACKET_TRAILER              (2U)
#define PACKET_OVERHEAD             (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE                 (128U)
#define PACKET_1K_SIZE              (1024U)
#define FILE_NAME_LENGTH            (256U)
#define FILE_SIZE_LENGTH            (16U)
#define SOH                         (0x01U)
#define STX                         (0x02U)
#define EOT                         (0x04U)
#define ACK                         (0x06U)
#define NAK                         (0x15U)
#define CA                          (0x18U)
#define CRC16                       (0x43U)
#define ABORT1                      (0x41U)
#define ABORT2                      (0x61U)
#define NAK_TIMEOUT                 (2000U)
#define MAX_ERRORS                  (3)
/******************************* Defines *******************************/

/************************** Private Variables **************************/
static uint8_t *s_p_rx_buffer = NULL;  /* 当前数据包的接收缓冲区。 */
static uint8_t s_rx_started = 0U;  /* 已启动或通知尚未消费的接收标记。 */
/************************** Private Variables **************************/

/******************** Private Function Declarations ********************/
static int32_t Receive_Byte(uint8_t *p_data,
                            uint16_t *p_received_length,
                            uint32_t timeout_ms);
static void Send_Byte(uint8_t character);
static int32_t Receive_Packet(uint8_t *p_data,
                              int32_t *p_length,
                              uint32_t timeout);
/******************** Private Function Declarations ********************/

/************************* Function Definitions *************************/
/**
 * @brief 停止串口接收并清空旧的接收长度通知。
 *
 * 同步中止接收，清除接收标记，再以零等待方式取空队列。
 * 用于会话初始化、超时恢复和有效会话的退出路径。
 *
 * @return 无。
 */
static void Stop_Receive(void)
{
    uint16_t discarded_length;  /* 取出并丢弃的旧通知。 */

    // 1. 同步停止接收，恢复串口接收状态。
    (void)HAL_UART_AbortReceive(&huart1);
    s_rx_started = 0U;

    // 2. 清空已完成或超时边界上残留的长度通知。
    if (NULL != g_YmodemReclength_queue)
    {
        while (pdTRUE == xQueueReceive(g_YmodemReclength_queue,
                                       &discarded_length,
                                       0U))
        {
        }
    }
}

/**
 * @brief 为下一包启动普通模式直接内存访问接收和串口空闲检测。
 *
 * 已有接收或尚未消费的通知时直接返回，不重复启动或清空队列。
 * 每次重新启动时清理旧状态，配置当前缓冲区并关闭半传输中断。
 *
 * @return int32_t 接收启动结果。
 * @retval 0 已启动，或已有接收等待处理。
 * @retval -1 缓冲区、队列不可用，或 HAL 启动失败。
 */
static int32_t Start_Receive(void)
{
    // 1. 保留已启动的接收及其尚未消费的通知。
    if (0U != s_rx_started)
    {
        return 0;
    }

    if ((NULL == s_p_rx_buffer) ||
        (NULL == g_YmodemReclength_queue))
    {
        return -1;
    }

    // 2. 清理旧状态，再为下一包启动 DMA。
    Stop_Receive();
    __HAL_UART_CLEAR_OREFLAG(&huart1);

    if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(
        &huart1,
        s_p_rx_buffer,
        YMODEM_RX_BUFFER_SIZE))
    {
        Stop_Receive();
        return -1;
    }

    // 3. 不使用半传输通知，避免将半包交给解析流程。
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);

    s_rx_started = 1U;
    return 0;
}

/**
 * @brief 等待一块接收数据的长度通知。
 *
 * 通过 DMA 和队列接收整块数据，不逐字节轮询。
 * 超时后停止接收并清空旧通知；成功时数据已在共享缓冲区中。
 *
 * @param[in,out] p_data 本次会话的共享接收缓冲区。
 * @param[out] p_received_length 本次收到的字节数。
 * @param[in] timeout_ms 等待通知的超时时间，单位为毫秒。
 *
 * @return int32_t 接收结果。
 * @retval 0 收到长度有效的接收通知。
 * @retval -1 参数无效、启动失败、等待超时或长度无效。
 */
static int32_t Receive_Byte(uint8_t *p_data,
                            uint16_t *p_received_length,
                            uint32_t timeout_ms)
{
    if ((NULL == p_data) ||
        (NULL == p_received_length) ||
        (p_data != s_p_rx_buffer))
    {
        return -1;
    }

    *p_received_length = 0U;

    if (0 != Start_Receive())
    {
        return -1;
    }

    // 等待长度通知；超时则停止接收并清空旧通知。
    if (pdTRUE != xQueueReceive(g_YmodemReclength_queue,
                                p_received_length,
                                pdMS_TO_TICKS(timeout_ms)))
    {
        Stop_Receive();
        return -1;
    }

    // 普通 DMA 已由 HAL 结束接收，检查通知长度。
    s_rx_started = 0U;

    if ((0U == *p_received_length) ||
        (*p_received_length > YMODEM_RX_BUFFER_SIZE))
    {
        return -1;
    }

    return 0;
}

/**
 * @brief 发送一个 YMODEM 控制字节。
 *
 * 发送确认、否定应答或请求字符前，先准备下一包接收。
 * 若准备接收失败则直接返回；其他控制字节直接通过串口发送。
 *
 * @param[in] character 待发送的控制字节。
 *
 * @return 无。
 */
static void Send_Byte(uint8_t character)
{
    // 先准备下一包，再发送允许发送端继续传输的应答。
    if ((character == ACK) ||
        (character == NAK) ||
        (character == CRC16))
    {
        if (0 != Start_Receive())
        {
            return;
        }
    }

    SerialPutChar(character);
}

/**
 * @brief 检查已接收数据的包类型、长度和包序号反码。
 *
 * 一次空闲或传输完成事件按一块完整数据处理，当前不累计拆分包。
 * 包尾字段计入长度检查，但当前不计算或验证 CRC16。
 *
 * @param[in,out] p_data 本次会话的共享接收缓冲区。
 * @param[out] p_length 数据区长度；结束为 0，发送端取消为 -1。
 * @param[in] timeout 等待接收通知的超时时间，单位为毫秒。
 *
 * @return int32_t 数据包检查结果。
 * @retval 0 收到有效数据包、结束标志或取消标志。
 * @retval 1 收到用户终止字符。
 * @retval -1 接收失败、参数无效或数据包格式错误。
 */
static int32_t Receive_Packet(uint8_t *p_data,
                              int32_t *p_length,
                              uint32_t timeout)
{
    uint16_t packet_size = 0U;  /* 根据包头确定的数据区长度。 */
    uint16_t received_length = 0U;  /* 中断通知的实际接收长度。 */

    if ((NULL == p_data) || (NULL == p_length))
    {
        return -1;
    }

    *p_length = 0;

    // 1. 等待数据块，再从缓冲区首字节识别包类型。
    if (0 != Receive_Byte(p_data, &received_length, timeout))
    {
        return -1;
    }

    switch (p_data[0])
    {
        case SOH:
            packet_size = PACKET_SIZE;
            break;

        case STX:
            packet_size = PACKET_1K_SIZE;
            break;

        case EOT:
            return (1U == received_length) ? 0 : -1;

        case CA:
            if ((2U == received_length) && (CA == p_data[1]))
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

    // 2. 检查数据区和包头、包尾的总长度。
    if (received_length != (packet_size + PACKET_OVERHEAD))
    {
        return -1;
    }

    // 3. 检查包序号反码；期望的序号由会话层检查。
    if (0xFFU != ((uint32_t)p_data[PACKET_SEQNO_INDEX] +
                  (uint32_t)p_data[PACKET_SEQNO_COMP_INDEX]))
    {
        return -1;
    }

    *p_length = packet_size;
    return 0;
}

/************************* Function Definitions *************************/

/*************************** Shared Variables ***************************/
int32_t packet_length = 0;  /* 数据区长度或控制包标记，供处理线程使用。 */
/*************************** Shared Variables ***************************/

/************************* Function Definitions *************************/
/**
 * @brief 使用双缓冲接收一个完整加密文件并交给写入线程。
 *
 * 处理步骤：
 * 1. 准备 DMA 接收，发送请求字符并解析文件信息。
 * 2. 校验包长度，发送长度指针并等待写入线程准备完成。
 * 3. 逐包检查类型和序号，将有效数据指针交给写入线程。
 * 4. 最多等待 2 秒确认处理结果，再累计有效长度、切换缓冲区并应答。
 * 5. 数据结束和空文件名结束时检查完整长度，再停止接收。
 * 加密文件须大于 16 字节、按 16 字节对齐且不超过 A 区容量。
 * 末包填充不写入；上一数据包的重传只确认，不重复写入。
 * 当前不计算或验证 CRC16，写入由独立线程执行。
 *
 * @param[in,out] p_buffer_1 第一个接收缓冲区，至少 1030 字节。
 * @param[in,out] p_buffer_2 第二个接收缓冲区，至少 1030 字节且与第一个不同。
 *
 * @return 会话结果；正值为完整接收的加密文件字节数。
 * @retval 0 发送端取消，或会话开始后连续接收错误超过阈值。
 * @retval -1 参数、格式、大小、长度、队列交接、处理等待或写入失败。
 * @retval -3 收到用户终止字符。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer_1, uint8_t *p_buffer_2)
{
    uint8_t *packet_data = p_buffer_1;  /* 当前用于接收和解析的数据包缓冲区。 */
    uint8_t file_size[FILE_SIZE_LENGTH] = {0U};  /* 文件大小字符串。 */
    uint8_t *p_file = NULL;  /* 文件信息包内的解析位置。 */

    uint32_t index = 0U;  /* 文件信息字符串的遍历下标。 */
    // int32_t packet_length = 0;  /* 数据区长度或控制包标记，供处理线程使用。 */
    int32_t session_done = 0;  /* 会话结束标记。 */
    int32_t file_done = 0;  /* 当前文件结束标记。 */
    int32_t packets_received = 0;  /* 下一个期望的包序号计数。 */
    int32_t errors = 0;  /* 会话开始后的连续接收错误计数。 */
    int32_t session_begin = 0;  /* 已接收有效包的标记。 */
    int32_t size = 0;  /* 发送端声明的文件大小，单位为字节。 */
    uint32_t bytes_received = 0U;  /* 已交给写入线程的有效文件长度。 */

    if ((NULL == p_buffer_1) || (NULL == p_buffer_2) ||
        (p_buffer_1 == p_buffer_2) ||
        (NULL == g_YmodemReclength_queue) ||
        (NULL == app_data_position_queue) ||
        (NULL == exit_falsh_state_semaphore))
    {
        return -1;
    }

    // 1. 初始化会话并准备接收，再发送请求字符。
    Stop_Receive();
    s_p_rx_buffer = p_buffer_1;
    Send_Byte(CRC16);

    // 2. 逐文件、逐包处理，直到收到空文件信息包。
    for (session_done = 0, errors = 0, session_begin = 0; ;)
    {
        for (packets_received = 0, file_done = 0; ;)
        {
            switch (Receive_Packet(packet_data,
                                   &packet_length,
                                   NAK_TIMEOUT))
            {
                case 0:
                    errors = 0;

                    switch (packet_length)
                    {
                        // 发送端取消传输，确认后清理接收状态。
                        case -1:
                            Send_Byte(ACK);
                            Stop_Receive();
                            return 0;

                        // 文件数据结束，确认并请求后续文件信息包。
                        case 0:
                            if ((size <= 16) ||
                                (bytes_received != (uint32_t)size) ||
                                (0U != g_ota_data_write_fault))
                            {
                                Send_Byte(CA);
                                Send_Byte(CA);
                                Stop_Receive();
                                return -1;
                            }
                            Send_Byte(ACK);
                            Send_Byte(CRC16);
                            file_done = 1;
                            break;

                        default:
                            if ((packet_data[PACKET_SEQNO_INDEX] & 0xFFU) !=
                                (packets_received & 0xFFU))
                            {
                                /* ACK 丢失导致的重传只应答，不重复写入。 */
                                if ((packets_received > 1) &&
                                    (packet_data[PACKET_SEQNO_INDEX] ==
                                     (uint8_t)(packets_received - 1)))
                                {
                                    Send_Byte(ACK);
                                }
                                else
                                {
                                    Send_Byte(NAK);
                                }
                                break;
                            }
                            else if (0 == packets_received)
                            {
                                // 第零包携带文件名和声明的文件大小。
                                if (0U != packet_data[PACKET_HEADER])
                                {
                                    /* 一次 OTA 只接收一个文件。 */
                                    if (0U != bytes_received)
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }
                                    p_file = &packet_data[PACKET_HEADER];

                                    // 跳过文件名，定位大小字符串。
                                    for (index = 0;
                                         (p_file < packet_data + PACKET_HEADER +
                                                   packet_length) &&
                                         (0U != *p_file) &&
                                         (index < FILE_NAME_LENGTH);
                                         index++)
                                    {
                                        p_file++;
                                    }

                                    if ((p_file >= packet_data + PACKET_HEADER +
                                                   packet_length) ||
                                        (0U != *p_file))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }

                                    p_file++;

                                    // 复制大小字段，并保证字符串结束。
                                    for (index = 0;
                                         (p_file < packet_data + PACKET_HEADER +
                                                   packet_length) &&
                                         (' ' != *p_file) &&
                                         ('\0' != *p_file) &&
                                         (index <
                                          (FILE_SIZE_LENGTH - 1U));
                                         index++)
                                    {
                                        file_size[index] = *p_file++;
                                    }
                                    file_size[index] = '\0';

                                    if ((p_file >= packet_data + PACKET_HEADER +
                                                   packet_length) ||
                                        (1U != Str2Int(file_size, &size)))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }

                                    // 检查声明的文件大小是否超过当前接收上限。
                                    if ((size <= 16) ||
                                        ((uint32_t)size >
                                         OTA_FIRMWARE_MAX_SIZE) ||
                                        (0U != ((uint32_t)size % 16U)))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }

                                    /* 交给处理线程的文件大小指针。 */
                                    int32_t *p_size = &size;
                                    if (pdPASS != xQueueSend(
                                        app_data_position_queue,
                                        &p_size,
                                        0U))
                                    {
                                        elog_e(
                                            TAG,
                                            "file size queue send failed: "
                                            "size=%ld",
                                            (long)size);
                                        Stop_Receive();
                                        return -1;
                                    }
                                    /* 取走文件准备完成通知，避免被第一包数据误用。 */
                                    if (pdTRUE != xSemaphoreTake(
                                        exit_falsh_state_semaphore,
                                        pdMS_TO_TICKS(2000U)))
                                    {
                                        elog_e(
                                            TAG,
                                            "file preparation timeout: "
                                            "size=%ld",
                                            (long)size);
                                        Stop_Receive();
                                        return -1;
                                    }
                                    if (0U != g_ota_data_write_fault)
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }
                                    if (packet_data == p_buffer_1)
                                    {
                                        packet_data = p_buffer_2;
                                    }
                                    else
                                    {
                                        packet_data = p_buffer_1;
                                    }
                                    s_p_rx_buffer = packet_data;

                                    Send_Byte(ACK);
                                    Send_Byte(CRC16);
                                }
                                else
                                {
                                    if ((size <= 16) ||
                                        (bytes_received != (uint32_t)size) ||
                                        (0U != g_ota_data_write_fault))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }
                                    Send_Byte(ACK);
                                    file_done = 1;
                                    session_done = 1;  /* 空文件名结束会话。 */
                                    break;
                                }
                            }
                            else
                            {
                                uint32_t write_length = 0U;
                                if (bytes_received >= (uint32_t)size)
                                {
                                    Send_Byte(CA);
                                    Send_Byte(CA);
                                    Stop_Receive();
                                    return -1;
                                }
                                /* 末包只保存有效文件字节，不保存协议填充。 */
                                write_length = (uint32_t)size - bytes_received;
                                if (write_length > (uint32_t)packet_length)
                                {
                                    write_length = (uint32_t)packet_length;
                                }
                                packet_length = (int32_t)write_length;
                                // 将数据区首地址交给写入线程，等待处理完成。
                                uint8_t *p_payload =
                                    packet_data + PACKET_HEADER;
                                if (pdPASS != xQueueSend(
                                    app_data_position_queue,
                                    &p_payload,
                                    0U))
                                {
                                    elog_e(TAG,
                                           "data queue send failed: "
                                           "packet=%ld, data=%p",
                                           (long)packets_received,
                                           (void *)p_payload);

                                    Stop_Receive();
                                    return -1;
                                }

                                /* 最多等待处理线程 2 秒；完成后才切换缓冲区并应答。 */
                                if (pdTRUE != xSemaphoreTake(
                                    exit_falsh_state_semaphore,
                                    pdMS_TO_TICKS(2000U)))
                                {
                                    elog_e(TAG,
                                           "data processing timeout: "
                                           "packet=%ld",
                                           (long)packets_received);

                                    Stop_Receive();
                                    return -1;
                                }

                                if (0U != g_ota_data_write_fault)
                                {
                                    Send_Byte(CA);
                                    Send_Byte(CA);
                                    Stop_Receive();
                                    return -1;
                                }
                                bytes_received += write_length;

                                if (packet_data == p_buffer_1)
                                {
                                    packet_data = p_buffer_2;
                                }
                                else
                                {
                                    packet_data = p_buffer_1;
                                }
                                s_p_rx_buffer = packet_data;
                                Send_Byte(ACK);
                            }

                            packets_received++;
                            session_begin = 1;
                            break;
                    }
                    break;

                // 用户终止时发送取消字节并结束会话。
                case 1:
                    Send_Byte(CA);
                    Send_Byte(CA);
                    Stop_Receive();
                    return -3;

                default:
                    // 初始等待持续请求；会话开始后才累计错误。
                    if (session_begin > 0)
                    {
                        errors++;
                    }

                    if (errors > MAX_ERRORS)
                    {
                        Send_Byte(CA);
                        Send_Byte(CA);
                        Stop_Receive();
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

    // 3. 会话结束时停止接收，返回声明的文件大小。
    Stop_Receive();
    return size;
}
/************************* Function Definitions *************************/
