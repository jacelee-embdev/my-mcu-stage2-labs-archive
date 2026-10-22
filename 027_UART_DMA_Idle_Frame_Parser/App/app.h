/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file app.h
 *
 * @par dependencies
 * - stdint.h
 * - usart.h
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - circular_buffer.h
 * - elog.h
 * - main.h
 * - bsp_uart_driver.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares UART packet types and application initialization.
 *
 * Processing flow:
 *
 * Call app_init() after EasyLogger initialization and before the scheduler
 * starts. The parser task owns one UART DMA circular buffer, parses complete
 * protocol frames, and forwards valid packet contexts to the application
 * task.
 *
 * @version V1.1 2026-09-12
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __APP_H__
#define __APP_H__

/****************************** Includes ******************************/
#include <stdint.h>

#include "usart.h"              /* Core layer. */

#include "FreeRTOS.h"           /* OS layer. */
#include "task.h"
#include "queue.h"

#include "circular_buffer.h"    /* Middleware layer. */

#include "elog.h"               /* BSP and application layer. */
#include "main.h"
#include "bsp_uart_driver.h"
/****************************** Includes ******************************/

/****************************** Defines *******************************/
#define MAX_PAYLOAD_LEN    (256U)  /* Maximum accepted payload length. */

typedef enum
{
    RX_WAIT_HEADER_1,  /* Wait for the first frame-header byte. */
    RX_WAIT_HEADER_2,  /* Wait for the second frame-header byte. */
    RX_WAIT_LENGTH,    /* Wait for the payload-length byte. */
    RX_READ_PAYLOAD,   /* Collect the configured payload bytes. */
    RX_READ_CHECK,     /* Wait for and verify the checksum byte. */
    RX_WAIT_TAIL       /* Wait for the frame-tail byte. */
} packet_parser_state_t; /* Packet-parser state. */

typedef enum
{
    PACKET_PARSE_IN_PROGRESS,  /* The current frame is incomplete. */
    PACKET_PARSE_COMPLETE,     /* A complete valid frame was received. */
    PACKET_PARSE_ERROR         /* The current frame is invalid. */
} packet_parse_result_t; /* Result produced after one input byte. */

typedef struct
{
    uint8_t len;                    /* Number of valid payload bytes. */
    uint8_t data[MAX_PAYLOAD_LEN];  /* Parsed payload storage. */
} parsed_packet_t; /* Payload delivered after frame validation. */

typedef struct
{
    packet_parser_state_t frame_state;  /* Current parser state. */
    uint8_t payload_index;              /* Next payload write index. */
    uint8_t calculated_checksum;        /* Running LEN + DATA sum. */
    parsed_packet_t parsed_packet;      /* Frame under construction. */
} packet_parser_t; /* Persistent packet-parser context. */
/****************************** Defines *******************************/

/****************************** Declaring ******************************/
/********************** Global Variable Declaration ******************/
extern circular_buf_t g_uart_rx_buffer;
/********************** Global Variable Declaration ******************/

/************************ API Declaration ************************/
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
void app_init(void);
/****************************** Declaring ******************************/

#endif /* End of __APP_H__ */
