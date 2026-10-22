/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file circular_buffer.h
 *
 * @par dependencies
 * - stdlib.h
 * - stdint.h
 * - stdio.h
 * - string.h
 * - elog.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares circular-buffer data types and operation interfaces.
 *
 * Processing flow:
 *
 * Call create_empty_circular_buf() to allocate an empty buffer. Use
 * buf_write_data() and buf_read_data() to store and retrieve bytes, and call
 * check_buf_status() when the current state is required. head_total and
 * tail_total are cumulative counters; array positions are calculated with
 * modulo BUFFER_SIZE when data is accessed.
 *
 * @version V1.1 2026-09-12
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

/****************************** Includes ******************************/
#include <stdlib.h>             /* Compiler library. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "elog.h"               /* Middleware layer. */
/****************************** Includes ******************************/

/****************************** Defines *******************************/
#define BUFFER_SIZE    (128U)  /* Number of bytes in the data array. */

#if ((0U == BUFFER_SIZE) || (0U != (BUFFER_SIZE & (BUFFER_SIZE - 1U))))
#error "BUFFER_SIZE must be a non-zero power of two."
#endif

typedef uint8_t data_type_t; /* Data type stored in the circular buffer. */

typedef struct
{
    uint8_t data[BUFFER_SIZE];       /* Circular data storage. */
    volatile uint32_t head_total;    /* Total published or written bytes. */
    volatile uint32_t tail_total;    /* Total consumed or read bytes. */
} circular_buf_t; /* Circular buffer with cumulative positions. */

typedef enum
{
    BUFFER_NULL   = 0,  /* The buffer pointer is NULL. */
    BUFFER_NORMAL = 1,  /* The buffer is neither empty nor full. */
    BUFFER_EMPTY  = 2,  /* The buffer contains no unread data. */
    BUFFER_FULL   = 3,  /* No software write position is available. */
} buf_status_t; /* Current circular-buffer state. */

typedef enum
{
    BUFFER_WR_OK    = 0,  /* The operation completed successfully. */
    BUFFER_WR_ERROR = 1,  /* The operation failed. */
} buf_wr_status_t; /* Circular-buffer read or write result. */
/****************************** Defines *******************************/

/****************************** Declaring ******************************/
/************************ API Declaration ************************/
/**
 * @brief Creates an empty circular buffer.
 *
 * Allocates one circular_buf_t object and initializes all fields to zero.
 *
 * @return circular_buf_t * Pointer to the created circular buffer.
 * @retval NULL Memory allocation failed.
 */
circular_buf_t *create_empty_circular_buf(void);

/**
 * @brief Checks the current state of a circular buffer.
 *
 * @param[in] p_buffer Pointer to the circular buffer to be checked.
 *
 * @return buf_status_t Current state of the circular buffer.
 * @retval BUFFER_NULL   The buffer pointer is NULL.
 * @retval BUFFER_NORMAL The buffer is neither empty nor full.
 * @retval BUFFER_EMPTY  The buffer contains no unread data.
 * @retval BUFFER_FULL   No software write position is available.
 */
buf_status_t check_buf_status(circular_buf_t *p_buffer);

/**
 * @brief Writes one data item to a circular buffer.
 *
 * @param[in,out] p_buffer Pointer to the destination circular buffer.
 * @param[in] data Data item to be written.
 *
 * @return buf_wr_status_t Result of the write operation.
 * @retval BUFFER_WR_OK    The data item was written successfully.
 * @retval BUFFER_WR_ERROR The buffer is NULL or full.
 */
buf_wr_status_t buf_write_data(circular_buf_t *p_buffer,
                               data_type_t data);

/**
 * @brief Reads one data item from a circular buffer.
 *
 * @param[in,out] p_buffer Pointer to the source circular buffer.
 * @param[out] p_data Pointer to the variable that receives the data item.
 *
 * @return buf_wr_status_t Result of the read operation.
 * @retval BUFFER_WR_OK    The data item was read successfully.
 * @retval BUFFER_WR_ERROR An input pointer is NULL or the buffer is empty.
 */
buf_wr_status_t buf_read_data(circular_buf_t *p_buffer,
                              data_type_t *p_data);
/****************************** Declaring ******************************/

#endif /* End of __CIRCULAR_BUFFER_H__ */
