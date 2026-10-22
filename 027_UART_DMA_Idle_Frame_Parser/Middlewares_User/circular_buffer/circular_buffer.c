/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file circular_buffer.c
 *
 * @par dependencies
 * - circular_buffer.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements circular-buffer creation, status, write, and read APIs.
 *
 * Processing flow:
 *
 * Create a circular-buffer object with create_empty_circular_buf(). Use
 * buf_write_data() to write at head_total and buf_read_data() to read at
 * tail_total. Both fields are cumulative counters; data-array positions wrap
 * with modulo BUFFER_SIZE. One array position remains unused so empty and full
 * states can be distinguished for software writes.
 *
 * @version V1.1 2026-09-12
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "circular_buffer.h"
/****************************** Includes ******************************/

/****************************** Defines *******************************/
#define TAG    "circular_buffer"
/****************************** Defines *******************************/

/**
 * @brief Creates an empty circular buffer.
 *
 * Steps:
 * 1. Allocates memory for one circular_buf_t object.
 * 2. Initializes all object fields to zero.
 *
 * @return circular_buf_t * Pointer to the created circular buffer.
 * @retval NULL Memory allocation failed.
 */
circular_buf_t *create_empty_circular_buf(void)
{
    circular_buf_t *p_circular_buf = NULL;

    /* Step 1: Allocate one circular-buffer object. */
    p_circular_buf = (circular_buf_t *)malloc(sizeof(circular_buf_t));
    if (NULL == p_circular_buf)
    {
        elog_e(TAG, "Memory allocation failed.\r\n");
        return NULL;
    }

    /* Step 2: Initialize all buffer fields. */
    (void)memset(p_circular_buf, 0, sizeof(circular_buf_t));

    return p_circular_buf;
}

/**
 * @brief Checks the current state of a circular buffer.
 *
 * Calculates the unread-data count from the cumulative head and tail values.
 * One array position remains unused for software writes.
 *
 * @param[in] p_buffer Pointer to the circular buffer to be checked.
 *
 * @return buf_status_t Current state of the circular buffer.
 * @retval BUFFER_NULL   The buffer pointer is NULL.
 * @retval BUFFER_NORMAL The buffer is neither empty nor full.
 * @retval BUFFER_EMPTY  The buffer contains no unread data.
 * @retval BUFFER_FULL   No software write position is available.
 */
buf_status_t check_buf_status(circular_buf_t *p_buffer)
{
    uint32_t data_count = 0U;

    /* Step 1: Reject an invalid buffer pointer. */
    if (NULL == p_buffer)
    {
        elog_e(TAG, "Buffer pointer is NULL.\r\n");
        return BUFFER_NULL;
    }

    /* Step 2: Calculate the number of unread data items. */
    data_count = p_buffer->head_total - p_buffer->tail_total;

    /* Step 3: Classify the current buffer state. */
    if (0U == data_count)
    {
        return BUFFER_EMPTY;
    }

    if ((BUFFER_SIZE - 1U) <= data_count)
    {
        return BUFFER_FULL;
    }

    return BUFFER_NORMAL;
}

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
                               data_type_t data)
{
    buf_status_t buffer_status = BUFFER_NULL;

    /* Step 1: Check that the buffer has a software write position. */
    buffer_status = check_buf_status(p_buffer);
    if ((BUFFER_NULL == buffer_status) ||
        (BUFFER_FULL == buffer_status))
    {
        elog_e(TAG, "Buffer write failed.\r\n");
        return BUFFER_WR_ERROR;
    }

    /* Step 2: Write the item and advance the cumulative head. */
    p_buffer->data[p_buffer->head_total % BUFFER_SIZE] = data;
    p_buffer->head_total++;

    return BUFFER_WR_OK;
}

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
                              data_type_t *p_data)
{
    buf_status_t buffer_status = BUFFER_NULL;

    /* Step 1: Reject an invalid output pointer. */
    if (NULL == p_data)
    {
        elog_e(TAG, "Data pointer is NULL.\r\n");
        return BUFFER_WR_ERROR;
    }

    /* Step 2: Check that the buffer contains unread data. */
    buffer_status = check_buf_status(p_buffer);
    if ((BUFFER_NULL == buffer_status) ||
        (BUFFER_EMPTY == buffer_status))
    {
        elog_e(TAG, "Buffer read failed.\r\n");
        return BUFFER_WR_ERROR;
    }

    /* Step 3: Read the item and advance the cumulative tail. */
    *p_data = p_buffer->data[p_buffer->tail_total % BUFFER_SIZE];
    p_buffer->tail_total++;

    return BUFFER_WR_OK;
}
