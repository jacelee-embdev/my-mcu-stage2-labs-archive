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
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Receives YMODEM packets into SRAM using UART DMA and a queue.
 *
 * Processing flow:
 *
 * Prepare UART DMA before sending receive-control responses.
 * Wait for receive-length notifications and check packet framing.
 * Acknowledge packets and stop reception when the session ends.
 *
 * @version v1.0 2026-10-02
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
/******************************* Includes *******************************/

/******************************* Defines *******************************/
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
#define FLASH_SIZE                  (0x40000U)
/******************************* Defines *******************************/

/************************** External Variables **************************/
/* 由系统初始化创建，由串口中断发送接收长度通知。 */
extern QueueHandle_t g_YmodemReclength;
/************************** External Variables **************************/

/************************** Private Variables **************************/
static uint8_t *s_p_rx_buffer = NULL;  /* 当前会话的共享接收缓冲区。 */
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

/****************************** Functions ******************************/
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
    if (NULL != g_YmodemReclength)
    {
        while (pdTRUE == xQueueReceive(g_YmodemReclength,
                                       &discarded_length,
                                       0U))
        {
        }
    }
}

/**
 * @brief 为下一包启动普通 DMA 和串口空闲检测。
 *
 * 已有接收或尚未消费的通知时直接返回，不重复启动或清空队列。
 * 首次启动时清理旧状态，将数据写入共享缓冲区，关闭半传输中断。
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
        (NULL == g_YmodemReclength))
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
 * 保留原函数名；当前通过 DMA 和队列接收整块数据，不逐字节轮询。
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
    if (pdTRUE != xQueueReceive(g_YmodemReclength,
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
    if (p_data[PACKET_SEQNO_INDEX] !=
        (uint8_t)(p_data[PACKET_SEQNO_COMP_INDEX] ^ 0xFFU))
    {
        return -1;
    }

    *p_length = packet_size;
    return 0;
}

/**
 * @brief 通过 YMODEM 接收一个文件会话并完成应答。
 *
 * 1. 初始化共享缓冲区和接收状态，发送请求字符。
 * 2. 解析文件信息并检查大小，逐包检查序号并应答。
 * 3. 处理结束标志和空文件信息包，停止接收后返回。
 * 缓冲区逐包复用，当前不保存完整文件，也不写入 Flash。
 *
 * @param[in,out] p_buffer 接收缓冲区，至少可容纳 1030 字节。
 *
 * @return int32_t 会话结果；正值为发送端声明的文件大小。
 * @retval 0 发送端取消、连续错误超过阈值，或声明大小为零。
 * @retval -1 参数、文件信息或文件大小无效。
 * @retval -3 收到用户终止字符。
 */
int32_t Ymodem_Receive(uint8_t *p_buffer)
{
    uint8_t *packet_data = p_buffer;  /* 直接复用传入的包缓冲区。 */
    uint8_t file_size[FILE_SIZE_LENGTH] = {0U};  /* 文件大小字符串。 */
    uint8_t *p_file = NULL;  /* 文件信息包内的解析位置。 */

    int32_t index = 0;  /* 文件信息字符串的遍历下标。 */
    int32_t packet_length = 0;  /* 数据区长度或控制包标记。 */
    int32_t session_done = 0;  /* 会话结束标记。 */
    int32_t file_done = 0;  /* 当前文件结束标记。 */
    int32_t packets_received = 0;  /* 下一个期望的包序号计数。 */
    int32_t errors = 0;  /* 会话开始后的连续接收错误计数。 */
    int32_t session_begin = 0;  /* 已接收有效包的标记。 */
    int32_t size = 0;  /* 发送端声明的文件大小，单位为字节。 */

    if ((NULL == p_buffer) || (NULL == g_YmodemReclength))
    {
        return -1;
    }

    // 1. 初始化会话并准备接收，再发送请求字符。
    Stop_Receive();
    s_p_rx_buffer = p_buffer;
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
                            Send_Byte(ACK);
                            Send_Byte(CRC16);
                            file_done = 1;
                            break;

                        default:
                            if ((packet_data[PACKET_SEQNO_INDEX] & 0xFFU) !=
                                (packets_received & 0xFFU))
                            {
                                // 包序号错误时不递增计数。
                                Send_Byte(NAK);
                                break;
                            }
                            else if (0 == packets_received)
                            {

                                // 第零包携带文件名和声明的文件大小。
                                if (0U != packet_data[PACKET_HEADER])
                                {
                                    p_file = &packet_data[PACKET_HEADER];

                                    // 跳过文件名，定位大小字符串。
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
                                        Stop_Receive();
                                        return -1;
                                    }

                                    p_file++;

                                    // 复制大小字段，并保证字符串结束。
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
                                        Stop_Receive();
                                        return -1;
                                    }

                                    // 保留原大小上限；当前没有 Flash 写入。
                                    if (size > (FLASH_SIZE - 1U))
                                    {
                                        Send_Byte(CA);
                                        Send_Byte(CA);
                                        Stop_Receive();
                                        return -1;
                                    }

                                    Send_Byte(ACK);
                                    Send_Byte(CRC16);
                                }
                                else
                                {

                                    Send_Byte(ACK);
                                    file_done = 1;
                                    session_done = 1;  /* 空文件名结束会话。 */
                                    break;
                                }
                            }
                            else
                            {
                                // 数据已在 SRAM 中，仅应答后复用缓冲区。
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
/****************************** Functions ******************************/
