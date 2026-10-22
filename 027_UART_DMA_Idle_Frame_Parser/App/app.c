/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file app.c
 *
 * @par dependencies
 * - app.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements UART frame parsing and application-task delivery.
 *
 * Processing flow:
 *
 * app_init() creates the validated-packet queue and starts the packet-parser
 * and application tasks. The parser owns one UART circular-DMA buffer, starts
 * Receive-to-Idle before entering its loop, and is woken when the ISR publishes
 * new bytes. Valid packet contexts are forwarded to the application task.
 *
 * @version V1.1 2026-09-12
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "app.h"
/****************************** Includes ******************************/

/****************************** Defines *******************************/
#define TAG               "app"
#define FRAME_HEADER_1    (0xFFU)  /* First frame-header byte. */
#define FRAME_HEADER_2    (0xFEU)  /* Second frame-header byte. */
#define FRAME_TAIL        (0xAAU)  /* Frame-tail byte. */
/****************************** Defines *******************************/

/************************ Queue Handles ************************/
static QueueHandle_t g_packet_ok_queue = NULL;

/*************************** Global Resources ************************/
circular_buf_t g_uart_rx_buffer = {0};
/*************************** Global Resources ************************/

/**
 * @brief Feeds one received byte into the UART packet state machine.
 *
 * The parser recognizes FF FE, validates the payload length, collects payload
 * bytes, verifies the LEN + DATA checksum, and checks the AA frame tail.
 * Parser context is preserved by the caller between calls.
 *
 * @param[in,out] p_parser Pointer to the persistent parser context.
 * @param[in] data Byte read from the UART receive buffer.
 *
 * @return packet_parse_result_t Result after consuming the byte.
 * @retval PACKET_PARSE_IN_PROGRESS The current frame is incomplete.
 * @retval PACKET_PARSE_COMPLETE    A complete valid frame was received.
 * @retval PACKET_PARSE_ERROR       The parser or current frame is invalid.
 */
static packet_parse_result_t packet_parser_feed_byte(
    packet_parser_t *p_parser,
    uint8_t data)
{
    packet_parse_result_t parse_result = PACKET_PARSE_IN_PROGRESS;

    /* Step 1: Reject an invalid parser context. */
    if (NULL == p_parser)
    {
        return PACKET_PARSE_ERROR;
    }

    /* Step 2: Consume the byte according to the current parser state. */
    switch (p_parser->frame_state)
    {
        case RX_WAIT_HEADER_1:
        {
            if (FRAME_HEADER_1 == data)
            {
                p_parser->frame_state = RX_WAIT_HEADER_2;
            }
            break;
        }

        case RX_WAIT_HEADER_2:
        {
            if (FRAME_HEADER_2 == data)
            {
                p_parser->frame_state = RX_WAIT_LENGTH;
            }
            else if (FRAME_HEADER_1 != data)
            {
                p_parser->frame_state = RX_WAIT_HEADER_1;
            }
            break;
        }

        case RX_WAIT_LENGTH:
        {
            if ((0U < data) && (MAX_PAYLOAD_LEN >= data))
            {
                p_parser->parsed_packet.len   = data;
                p_parser->payload_index       = 0U;
                p_parser->calculated_checksum =
                    p_parser->parsed_packet.len;
                p_parser->frame_state         = RX_READ_PAYLOAD;
            }
            else
            {
                p_parser->frame_state = RX_WAIT_HEADER_1;
                parse_result          = PACKET_PARSE_ERROR;
            }
            break;
        }

        case RX_READ_PAYLOAD:
        {
            p_parser->parsed_packet.data[p_parser->payload_index] = data;
            p_parser->payload_index++;
            p_parser->calculated_checksum += data;

            if (p_parser->payload_index >= p_parser->parsed_packet.len)
            {
                p_parser->frame_state = RX_READ_CHECK;
            }
            break;
        }

        case RX_READ_CHECK:
        {
            if (p_parser->calculated_checksum == data)
            {
                p_parser->frame_state = RX_WAIT_TAIL;
            }
            else
            {
                p_parser->frame_state = RX_WAIT_HEADER_1;
                parse_result          = PACKET_PARSE_ERROR;
            }
            break;
        }

        case RX_WAIT_TAIL:
        {
            if (FRAME_TAIL == data)
            {
                parse_result = PACKET_PARSE_COMPLETE;
            }
            else
            {
                parse_result = PACKET_PARSE_ERROR;
            }
            p_parser->frame_state = RX_WAIT_HEADER_1;
            break;
        }

        default:
        {
            p_parser->frame_state = RX_WAIT_HEADER_1;
            parse_result          = PACKET_PARSE_ERROR;
            break;
        }
    }

    return parse_result;
}

/**
 * @brief Entry function for the packet-parser task.
 *
 * Starts USART1 Receive-to-Idle circular DMA, blocks for direct task
 * notifications from the UART receive-event callback, and consumes every byte
 * published up to a stable head_total snapshot. Valid packet contexts are
 * forwarded to g_packet_ok_queue.
 *
 * @param[in] p_argument Pointer to the task argument. This parameter is unused.
 *
 * @return None. This task runs indefinitely.
 */
static void packet_parser_task(void *p_argument)
{
    packet_parser_t parser =
    {
        .frame_state         = RX_WAIT_HEADER_1,
        .payload_index       = 0U,
        .calculated_checksum = 0U,
        .parsed_packet       = {0}
    };
    packet_parser_t *p_parser = &parser;
    uint32_t head_snapshot = 0U;

    (void)p_argument;

    /* Step 1: Start DMA with the application-owned receive buffer. */
    if (HAL_OK != bsp_uart_rx_start(&huart1, &g_uart_rx_buffer))
    {
        elog_e(TAG, "Failed to start UART Receive-to-Idle DMA.\r\n");
        Error_Handler();
    }

    for (;;)
    {
        /* Step 2: Block until the ISR publishes one or more new bytes. */
        if (0U == ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
        {
            continue;
        }

        /* Step 3: Capture the fixed endpoint for this processing pass. */
        head_snapshot = g_uart_rx_buffer.head_total;

        /* Step 4: Consume and parse each byte up to the endpoint. */
        while (g_uart_rx_buffer.tail_total != head_snapshot)
        {
            uint8_t data = 0U;
            packet_parse_result_t parse_result =
                PACKET_PARSE_IN_PROGRESS;

            if (BUFFER_WR_OK != buf_read_data(&g_uart_rx_buffer, &data))
            {
                elog_e(TAG, "Buffer read failed.\r\n");
                break;
            }

            parse_result = packet_parser_feed_byte(p_parser, data);

            if (PACKET_PARSE_COMPLETE == parse_result)
            {
                if (pdPASS != xQueueSend(g_packet_ok_queue,
                                         &p_parser,
                                         0U))
                {
                    elog_e(TAG, "Queue send failed.\r\n");
                }
            }
            else if (PACKET_PARSE_ERROR == parse_result)
            {
                elog_e(TAG, "Data parsing failed.\r\n");
            }
            else
            {
                /* The current packet is not complete yet. */
            }
        }
    }
}

/**
 * @brief Entry function for the application task.
 *
 * Waits for a validated packet-parser context and logs every valid payload
 * byte received from the packet-parser task.
 *
 * @param[in] p_argument Pointer to the task argument. This parameter is unused.
 *
 * @return None. This task runs indefinitely.
 */
static void app_task(void *p_argument)
{
    packet_parser_t *p_parser = NULL;

    (void)p_argument;

    for (;;)
    {
        /* Step 1: Block until the parser publishes a valid packet. */
        p_parser = NULL;
        if (pdPASS != xQueueReceive(g_packet_ok_queue,
                                    &p_parser,
                                    portMAX_DELAY))
        {
            elog_e(TAG, "Failed to receive packet.\r\n");
            continue;
        }

        /* Step 2: Reject an invalid packet context. */
        if (NULL == p_parser)
        {
            elog_e(TAG, "Packet parser pointer is NULL.\r\n");
            continue;
        }

        /* Step 3: Report all payload bytes in the valid packet. */
        for (uint8_t index = 0U;
             index < p_parser->parsed_packet.len;
             index++)
        {
            elog_i(TAG,
                   "Payload%u:[%x]\r\n",
                   index,
                   p_parser->parsed_packet.data[index]);
        }
    }
}

/**
 * @brief Initializes UART parsing and application delivery.
 *
 * Steps:
 * 1. Creates the validated-packet queue.
 * 2. Creates the packet-parser task.
 * 3. Creates the application task.
 *
 * @return None. Resource or task creation failures are logged and stop the
 *         remaining initialization.
 *
 * @note Call once after EasyLogger initialization and before the scheduler
 *       starts.
 */
void app_init(void)
{
    /* Step 1: Create the parser-to-application packet queue. */
    g_packet_ok_queue = xQueueCreate(1U, sizeof(packet_parser_t *));
    if (NULL == g_packet_ok_queue)
    {
        elog_e(TAG, "Packet queue creation failed.\r\n");
        return;
    }

    /* Step 2: Create the packet-parser task. */
    if (pdPASS != xTaskCreate(packet_parser_task,
                              "packet_parser",
                              128U,
                              NULL,
                              1U,
                              NULL))
    {
        elog_e(TAG, "Packet parser task creation failed.\r\n");
        return;
    }

    /* Step 3: Create the application task. */
    if (pdPASS != xTaskCreate(app_task,
                              "app",
                              128U,
                              NULL,
                              1U,
                              NULL))
    {
        elog_e(TAG, "Application task creation failed.\r\n");
        return;
    }

    elog_i(TAG, "App initialized successfully.\r\n");
}
