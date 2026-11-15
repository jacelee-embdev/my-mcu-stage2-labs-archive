/*******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * Licensed under the MIT License. See LICENSE in the project root.
 *
 * @file sw_bsp_aht21_driver.c
 *
 * @par dependencies
 * - sw_bsp_aht21_driver.h
 * - stddef.h
 * - elog.h
 * - sw_bsp_aht21_register.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Initializes the AHT21 and reads temperature and relative humidity.
 *
 * Processing flow:
 *
 * Bind the platform interfaces and check the sensor's power-on status.
 * Trigger a measurement, wait for conversion, and read one data frame.
 * Verify the CRC before converting and publishing both measurements.
 *
 * @version v1.0 2026-11-09
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************** Includes **********************************/

#include "sw_bsp_aht21_driver.h"    /* Module header                          */

#include <stddef.h>                 /* C standard library                     */

#include "elog.h"                   /* Middleware                             */

#include "sw_bsp_aht21_register.h"  /* AHT21 register definitions.            */

/********************************** Includes **********************************/

/********************************** Defines ***********************************/

#define TAG   "aht21.driver"      /* EasyLogger module tag.                   */
#define DEBUG                     /* Debug log switch.                        */

#define AHT21_NOT_INITIALIZED (0U)  /* Driver not initialized.                */
#define AHT21_INITIALIZED     (1U)  /* Driver initialized.                    */

#define AHT21_POWER_ON_STATUS_MASK (0x18U)  /* Power-on status bits.          */
#define AHT21_POWER_ON_WAIT_MS     (300U)   /* Power-on wait in ms.           */
#define AHT21_MEASUREMENT_WAIT_MS  (80U)    /* Conversion wait in ms.         */

#define AHT21_MEASUREMENT_BUSY_MASK   (0x80U)  /* Busy status bit.            */
#define AHT21_MEASUREMENT_DATA_BYTES  (6U)     /* Status and readings.        */
#define AHT21_MEASUREMENT_FRAME_BYTES (7U)     /* Data and CRC.               */

#define AHT21_CRC8_POLYNOMIAL (0x31U)  /* CRC-8 polynomial.                   */
#define AHT21_CRC8_INITIAL    (0xFFU)  /* CRC-8 initial value.                */

/********************************** Defines ***********************************/

/********************************* Declaring **********************************/

static uint8_t sg_is_initialized       = AHT21_NOT_INITIALIZED;  /* State.    */
static uint8_t sg_power_on_status_bits =                    0U;  /* Bits.     */

/********************************* Declaring **********************************/

/**************************** Function Definitions ****************************/

/**
 * @brief Calculate the AHT21 CRC-8 checksum of a byte buffer.
 *
 * Processing steps:
 * 1. Check the input and output pointers.
 * 2. XOR each byte into the CRC and process its eight bits.
 * 3. Store the calculated CRC.
 *
 * @param[in]  p_data     : Non-NULL buffer with at least byte_count bytes.
 * @param[in]  byte_count : Number of bytes to process. Zero is allowed.
 * @param[out] p_crc      : Receives the CRC on success.
 *
 * @return aht21_status_t : Calculation result.
 * @retval AHT21_OK              The CRC was written to p_crc.
 * @retval AHT21_ERROR_PARAMETER A required pointer is NULL.
 *
 * @note A zero byte count produces the initial CRC value.
 */
static aht21_status_t AHT21_CalculateCrc8(
                                               const uint8_t * const     p_data,
                                               const uint8_t         byte_count,
                                               uint8_t       * const      p_crc
                                                                               )
{
    uint8_t crc = AHT21_CRC8_INITIAL;

    /***************** 1. Check arguments *****************/
    if ((NULL == p_data) || (NULL == p_crc))
    {
#ifdef DEBUG
        elog_e(TAG, "CRC-8 input or output pointer is NULL.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    /****************** 2. Calculate CRC ******************/
    for (uint8_t byte_index = 0U; byte_index < byte_count; byte_index++)
    {
        crc ^= p_data[byte_index];

        for (uint8_t bit_index = 0U; bit_index < 8U; bit_index++)
        {
            if (0U != (crc & 0x80U))
            {
                crc = (uint8_t)((crc << 1U) ^ AHT21_CRC8_POLYNOMIAL);
            }
            else
            {
                crc = (uint8_t)(crc << 1U);
            }
        }
    }

    /****************** 3. Store result *******************/
    *p_crc = crc;
    return AHT21_OK;
}

/**
 * @brief Read one raw status byte from the AHT21.
 *
 * Processing steps:
 * 1. Check the driver and output pointers.
 * 2. Read the status byte, using a critical section for software IIC.
 * 3. Copy the status byte to the caller.
 *
 * @param[in]  p_self   : Driver object with its IIC interface bound.
 * @param[out] p_status : Receives the raw status byte on success.
 *
 * @return aht21_status_t : Read result or an IIC callback error.
 * @retval AHT21_OK              The status byte was written to p_status.
 * @retval AHT21_ERROR_PARAMETER A required pointer is NULL.
 * @retval AHT21_ERROR_TIMEOUT   The software IIC address was not acknowledged.
 *
 * @note Call after the bus is ready and the sensor power-on delay has elapsed.
 */
static aht21_status_t AHT21_ReadStatus(
                                      const bsp_aht21_driver_t * const   p_self,
                                      uint8_t                  * const p_status
                                                                               )
{

    /***************** 1. Check arguments *****************/
    if ((NULL == p_self) || (NULL == p_status))
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 status read requires a driver and output pointer.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    const aht21_iic_interface_t * const p_iic       = p_self->p_iic_interface;
    aht21_status_t                      result      =             AHT21_ERROR;
    uint8_t                             status_byte =                      0U;

    /************** 2. Read the status byte ***************/
#ifdef HARDWARE_IIC
    result = p_iic->pf_iic_start(NULL);
    if (AHT21_OK == result)
    {
        /* A failed step ends the read and falls through to STOP. */
        do
        {
            result = p_iic->pf_iic_send_byte(NULL, AHT21_REG_READ_ADDR);
            if (AHT21_OK != result)
            {
                break;
            }

            result = p_iic->pf_iic_wait_ack(NULL);
            if (AHT21_OK != result)
            {
                break;
            }

            result = p_iic->pf_iic_receive_byte(NULL, &status_byte);
            if (AHT21_OK != result)
            {
                break;
            }

            result = p_iic->pf_iic_send_nack(NULL);
        } while (0);

        if (AHT21_OK == result)
        {
            result = p_iic->pf_iic_stop(NULL);
        }
        else
        {
            (void)p_iic->pf_iic_stop(NULL);
        }
    }
#else
    p_iic->pf_critical_enter();

    iic_bus_t * const p_bus = p_iic->p_bus;

    IIC_Start(p_bus);
    IIC_SendByte(p_bus, AHT21_REG_READ_ADDR);
    if (SUCCESS == IIC_WaitAck(p_bus))
    {
        status_byte = IIC_ReadByte(p_bus);
        IIC_SendNack(p_bus);
        IIC_Stop(p_bus);
        result = AHT21_OK;
    }
    else
    {
        /* IIC_WaitAck already issued STOP on timeout. */
        result = AHT21_ERROR_TIMEOUT;
    }

    p_iic->pf_critical_exit();
#endif  /* HARDWARE_IIC */

    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 status read failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }

    /************** 3. Store the status byte **************/
    *p_status = status_byte;

    return AHT21_OK;
}

/**
 * @brief Check the required AHT21 power-on status bits.
 *
 * Processing steps:
 * 1. Read the raw status byte.
 * 2. Cache the masked bits and check the required power-on pattern.
 *
 * @param[in] p_self : Driver object with its IIC interface bound.
 *
 * @return aht21_status_t : Status check result or a status read error.
 * @retval AHT21_OK              The required status bits are set.
 * @retval AHT21_ERROR_PARAMETER The driver object is NULL.
 * @retval AHT21_ERROR_RESOURCE  The required status bits are missing.
 *
 * @note The cache holds the masked bits of the last completed status read.
 */
static aht21_status_t AHT21_CheckPowerOnStatus(
                                        const bsp_aht21_driver_t * const p_self
                                                                               )
{
    aht21_status_t result      = AHT21_ERROR;
    uint8_t        status_byte =          0U;

    /************** 1. Read the status byte ***************/
    result = AHT21_ReadStatus(p_self, &status_byte);
    if (AHT21_OK != result)
    {
        return result;
    }

    /************** 2. Check the status bits **************/
    sg_power_on_status_bits =
        (uint8_t)(status_byte & AHT21_POWER_ON_STATUS_MASK);
    if (AHT21_POWER_ON_STATUS_MASK != sg_power_on_status_bits)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 power-on status check failed (0x%02X).",
               (unsigned int)status_byte);
#endif  /* DEBUG */

        return AHT21_ERROR_RESOURCE;
    }

    return AHT21_OK;
}

/**
 * @brief Initialize the bus and check that the AHT21 is ready.
 *
 * Processing steps:
 * 1. Check the driver argument and clear the initialization flag.
 * 2. Initialize the IIC bus.
 * 3. Wait for power-on stabilization and check the sensor status.
 * 4. Mark the driver initialized and log its IIC address.
 *
 * @param[in] p_self : Driver object with the platform callbacks bound.
 *
 * @return aht21_status_t : Initialization result or a callback error.
 * @retval AHT21_OK              The sensor passed the power-on check.
 * @retval AHT21_ERROR_PARAMETER The driver object is NULL.
 * @retval AHT21_ERROR_RESOURCE  The sensor status bits are missing.
 *
 * @note The caller supplies a powered sensor and serializes initialization.
 */
static aht21_status_t AHT21_Init(bsp_aht21_driver_t * const p_self)
{

    /**************** 1. Check the driver *****************/
    if (NULL == p_self)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 initialization requires a driver object.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    aht21_status_t                      result =             AHT21_ERROR;
    const aht21_iic_interface_t * const p_iic  = p_self->p_iic_interface;
    sg_is_initialized = AHT21_NOT_INITIALIZED;

    /************* 2. Initialize the IIC bus **************/
#ifdef HARDWARE_IIC
    result = p_iic->pf_iic_init(NULL);
    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 IIC initialization failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }
#else
    p_iic->pf_critical_enter();

    IIC_Init(p_iic->p_bus);
    p_iic->pf_critical_exit();
#endif  /* HARDWARE_IIC */

    /**************** 3. Check the sensor *****************/
#ifdef OS_SUPPORTING
    p_self->p_rtos_delay->pf_rtos_delay(pdMS_TO_TICKS(AHT21_POWER_ON_WAIT_MS));
#else
    const uint32_t start_tick = p_self->p_timebase->pf_get_tick_count();

    while ((p_self->p_timebase->pf_get_tick_count() - start_tick) <
           AHT21_POWER_ON_WAIT_MS)
    {
        /* Wait for the powered sensor to stabilize. */
    }
#endif  /* OS_SUPPORTING */

    result = AHT21_CheckPowerOnStatus(p_self);
    if (AHT21_OK != result)
    {
        return result;
    }

    /*********** 4. Mark the driver initialized ***********/
    sg_is_initialized = AHT21_INITIALIZED;

#ifdef DEBUG
    elog_d(TAG, "AHT21 initialized at IIC address 0x%02X.",
           (unsigned int)AHT21_IIC_ADDRESS);
#endif  /* DEBUG */

    return result;
}

/**
 * @brief Report that bus deinitialization is not implemented.
 *
 * @param[in] p_self : Driver object; must not be NULL.
 *
 * @return aht21_status_t : Deinitialization request result.
 * @retval AHT21_ERROR_PARAMETER The driver object is NULL.
 * @retval AHT21_ERROR_RESOURCE  The operation is not implemented.
 *
 * @note The global initialization flag is left unchanged.
 *       A valid driver still receives AHT21_ERROR_RESOURCE.
 */
static aht21_status_t AHT21_Deinit(bsp_aht21_driver_t * const p_self)
{

    /**************** 1. Check the driver *****************/
    if (NULL == p_self)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 deinitialization requires a driver object.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    /********** 2. Report unsupported operation ***********/
    /* Bus deinitialization is not implemented. */
    aht21_status_t result = AHT21_ERROR_RESOURCE;

    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 IIC bus deinitialization failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }

    /************ 3. Clear state after success ************/
    sg_is_initialized = AHT21_NOT_INITIALIZED;

    return AHT21_OK;
}

/**
 * @brief Read temperature and relative humidity from one AHT21 measurement.
 *
 * @param[in]  p_self                  : Initialized driver object.
 * @param[out] p_temperature_c         : Temperature output in degrees Celsius.
 * @param[out] p_relative_humidity_pct : Relative humidity output in percent.
 *
 * @return aht21_status_t : Measurement result or a callback error.
 * @retval AHT21_OK              Both outputs were written.
 * @retval AHT21_ERROR_PARAMETER An argument is NULL or both output pointers are
 *                               identical.
 * @retval AHT21_ERROR_RESOURCE  The driver is not initialized.
 * @retval AHT21_ERROR_TIMEOUT   Busy polling or a software IIC ACK timed out.
 * @retval AHT21_ERROR           The frame CRC did not match.
 *
 * @note Outputs are written only on success. Serialize measurements on the
 *       same sensor from trigger through frame read.
 */
static aht21_status_t AHT21_ReadTemperatureHumidity(
                             bsp_aht21_driver_t * const                  p_self,
                             float              * const         p_temperature_c,
                             float              * const p_relative_humidity_pct
                                                                               )
{

    /************ 1. Check arguments and state ************/
    if ((NULL == p_self) || (NULL == p_temperature_c) ||
        (NULL == p_relative_humidity_pct) ||
        (p_temperature_c == p_relative_humidity_pct))
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 read requires a driver and distinct outputs.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    if (AHT21_INITIALIZED != sg_is_initialized)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 measurement requires an initialized driver.");
#endif  /* DEBUG */

        return AHT21_ERROR_RESOURCE;
    }

    const aht21_iic_interface_t * const p_iic = p_self->p_iic_interface;
    const uint8_t command[] =
    {
        AHT21_REG_WRITE_ADDR,
        AHT21_REG_MEASURE_CMD,
        AHT21_REG_MEASURE_CMD_ARGS1,
        AHT21_REG_MEASURE_CMD_ARGS2
    };
    aht21_status_t result                               = AHT21_ERROR;
    uint8_t        frame[AHT21_MEASUREMENT_FRAME_BYTES] =        {0U};

    /*************** 2. Trigger measurement ***************/
#ifdef HARDWARE_IIC
    result = p_iic->pf_iic_start(NULL);
    if (AHT21_OK == result)
    {
        for (uint8_t byte_index = 0U;
             (AHT21_OK == result) && (byte_index < sizeof(command));
             byte_index++)
        {
            result = p_iic->pf_iic_send_byte(NULL, command[byte_index]);
            if (AHT21_OK == result)
            {
                result = p_iic->pf_iic_wait_ack(NULL);
            }
        }

        const aht21_status_t stop_result = p_iic->pf_iic_stop(NULL);
        if (AHT21_OK == result)
        {
            result = stop_result;
        }
    }
#else
    p_iic->pf_critical_enter();

    iic_bus_t * const p_bus = p_iic->p_bus;

    result = AHT21_OK;
    IIC_Start(p_bus);
    for (uint8_t byte_index = 0U; byte_index < sizeof(command); byte_index++)
    {
        IIC_SendByte(p_bus, command[byte_index]);
        if (SUCCESS != IIC_WaitAck(p_bus))
        {
            result = AHT21_ERROR_TIMEOUT;
            break;
        }
    }

    if (AHT21_OK == result)
    {
        IIC_Stop(p_bus);
    }

    p_iic->pf_critical_exit();
#endif  /* HARDWARE_IIC */

    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 measurement trigger failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }

    /*************** 3. Wait for conversion ***************/
    uint32_t wait_ms     = AHT21_MEASUREMENT_WAIT_MS;
    uint8_t  status_byte =                        0U;

    /* Check once after conversion, then retry up to five times. */
    for (uint8_t poll = 0U; poll < 6U; poll++)
    {
#ifdef OS_SUPPORTING
        p_self->p_rtos_delay->pf_rtos_delay(pdMS_TO_TICKS(wait_ms));
#else
        const uint32_t start_tick = p_self->p_timebase->pf_get_tick_count();

        while ((p_self->p_timebase->pf_get_tick_count() - start_tick) < wait_ms)
        {
            /* Wait outside the IIC critical section. */
        }
#endif  /* OS_SUPPORTING */

        result = AHT21_ReadStatus(p_self, &status_byte);
        if (AHT21_OK != result)
        {
            return result;
        }

        if (0U == (status_byte & AHT21_MEASUREMENT_BUSY_MASK))
        {
            break;
        }

        wait_ms = 5U;
    }

    if (0U != (status_byte & AHT21_MEASUREMENT_BUSY_MASK))
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 measurement timed out.");
#endif  /* DEBUG */

        return AHT21_ERROR_TIMEOUT;
    }

    /****** 4. Read and verify the measurement frame ******/
#ifdef HARDWARE_IIC
    result = p_iic->pf_iic_start(NULL);
    if (AHT21_OK == result)
    {
        result = p_iic->pf_iic_send_byte(NULL, AHT21_REG_READ_ADDR);
        if (AHT21_OK == result)
        {
            result = p_iic->pf_iic_wait_ack(NULL);
        }

        for (uint8_t byte_index = 0U;
             (AHT21_OK == result) &&
             (byte_index < AHT21_MEASUREMENT_FRAME_BYTES);
             byte_index++)
        {
            result = p_iic->pf_iic_receive_byte(NULL, &frame[byte_index]);
            if (AHT21_OK != result)
            {
                break;
            }

            if (AHT21_MEASUREMENT_DATA_BYTES == byte_index)
            {
                result = p_iic->pf_iic_send_nack(NULL);
            }
            else
            {
                result = p_iic->pf_iic_send_ack(NULL);
            }
        }

        const aht21_status_t stop_result = p_iic->pf_iic_stop(NULL);
        if (AHT21_OK == result)
        {
            result = stop_result;
        }
    }
#else
    p_iic->pf_critical_enter();

    IIC_Start(p_bus);
    IIC_SendByte(p_bus, AHT21_REG_READ_ADDR);
    if (SUCCESS == IIC_WaitAck(p_bus))
    {
        for (uint8_t byte_index = 0U;
             byte_index < AHT21_MEASUREMENT_DATA_BYTES;
             byte_index++)
        {
            frame[byte_index] = IIC_ReadByte(p_bus);
            IIC_SendAck(p_bus);
        }

        frame[AHT21_MEASUREMENT_DATA_BYTES] = IIC_ReadByte(p_bus);
        IIC_SendNack(p_bus);
        IIC_Stop(p_bus);
        result = AHT21_OK;
    }
    else
    {
        result = AHT21_ERROR_TIMEOUT;
    }

    p_iic->pf_critical_exit();
#endif  /* HARDWARE_IIC */

    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 measurement frame read failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }

    const uint8_t received_crc   = frame[AHT21_MEASUREMENT_DATA_BYTES];
    uint8_t       calculated_crc =                                  0U;

    result = AHT21_CalculateCrc8(frame, AHT21_MEASUREMENT_DATA_BYTES,
                                &calculated_crc);
    if (AHT21_OK != result)
    {
        return result;
    }

    if (received_crc != calculated_crc)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 measurement frame CRC mismatch.");
#endif  /* DEBUG */

        return AHT21_ERROR;
    }

    /******* 5. Decode and store both measurements ********/
    /* frame[3] shares the humidity and temperature fields. */
    const uint32_t humidity_raw    = ((uint32_t)frame[1U] << 12U) |
                                     ((uint32_t)frame[2U] <<  4U) |
                                     ((uint32_t)frame[3U] >>  4U);
    const uint32_t temperature_raw = ((uint32_t)(frame[3U] & 0x0FU) << 16U) |
                                     ((uint32_t)frame[4U] << 8U) |
                                      (uint32_t)frame[5U];

    *p_relative_humidity_pct = ((float)humidity_raw    / 1048576.0f) * 100.0f;
    *p_temperature_c         = ((float)temperature_raw / 1048576.0f) * 200.0f -
                               50.0f;

    return AHT21_OK;
}

/**
 * @brief Report that sensor sleep is not implemented.
 *
 * @param[in] p_self : Driver object; must not be NULL.
 *
 * @return aht21_status_t : Sleep request result.
 * @retval AHT21_ERROR_PARAMETER The driver object is NULL.
 * @retval AHT21_ERROR_RESOURCE  The operation is not implemented.
 *
 * @note No sensor command is sent.
 *       A valid driver still receives AHT21_ERROR_RESOURCE.
 */
static aht21_status_t AHT21_Sleep(bsp_aht21_driver_t * const p_self)
{

    /*********** 1. Check the driver and state ************/
    if (NULL == p_self)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 sleep requires a driver object.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    if (AHT21_INITIALIZED != sg_is_initialized)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 sleep requires an initialized driver.");
#endif  /* DEBUG */

        return AHT21_ERROR_RESOURCE;
    }

    /************ 2. Report unsupported sleep *************/
    /* The sleep operation is not implemented. */
    aht21_status_t result = AHT21_ERROR_RESOURCE;

    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 sleep failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }

    /***************** 3. Report success ******************/
    return AHT21_OK;
}

/**
 * @brief Report that sensor wakeup is not implemented.
 *
 * @param[in] p_self : Driver object; must not be NULL.
 *
 * @return aht21_status_t : Wakeup request result.
 * @retval AHT21_ERROR_PARAMETER The driver object is NULL.
 * @retval AHT21_ERROR_RESOURCE  The operation is not implemented.
 *
 * @note No sensor command is sent.
 *       A valid driver still receives AHT21_ERROR_RESOURCE.
 */
static aht21_status_t AHT21_Wakeup(bsp_aht21_driver_t * const p_self)
{

    /*********** 1. Check the driver and state ************/
    if (NULL == p_self)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 wakeup requires a driver object.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    if (AHT21_INITIALIZED != sg_is_initialized)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 wakeup requires an initialized driver.");
#endif  /* DEBUG */

        return AHT21_ERROR_RESOURCE;
    }

    /************ 2. Report unsupported wakeup ************/
    /* The wakeup operation is not implemented. */
    aht21_status_t result = AHT21_ERROR_RESOURCE;

    if (AHT21_OK != result)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 wakeup failed (%d).", (int)result);
#endif  /* DEBUG */

        return result;
    }

    /***************** 3. Report success ******************/
    return AHT21_OK;
}

/**
 * @brief Bind platform interfaces and initialize an AHT21 driver object.
 *
 * @param[in]  p_iic_interface : Software bus or hardware callbacks.
 * @param[out] p_self          : Receives the initialized driver on success.
 * @param[in]  p_timebase      : Millisecond time-base interface.
 * @param[in]  p_rtos_delay    : RTOS delay interface when enabled.
 *
 * @return aht21_status_t : Instantiation result or an initialization error.
 * @retval AHT21_OK              The driver object is ready for use.
 * @retval AHT21_ERROR_PARAMETER A required pointer or callback is NULL.
 * @retval AHT21_ERROR_RESOURCE  The driver is already initialized or the sensor
 *                               failed the power-on status check.
 *
 * @note Serialize instantiation and keep the platform tables valid while in
 *       use, along with the software bus descriptor. A failed call leaves
 *       the caller's driver object untouched.
 */
aht21_status_t AHT21_Instantiate(
                     const aht21_iic_interface_t        * const p_iic_interface,
                     bsp_aht21_driver_t                 * const          p_self,
                     const aht21_timebase_interface_t   * const      p_timebase
#ifdef OS_SUPPORTING
                   , const aht21_rtos_delay_interface_t * const    p_rtos_delay
#endif  /* OS_SUPPORTING */
                                                                               )
{

    /********** 1. Check the arguments and state **********/
    if ((NULL == p_self) || (NULL == p_iic_interface) ||
        (NULL == p_timebase) || (NULL == p_timebase->pf_get_tick_count))
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 requires a driver, IIC and time base.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }

    if (AHT21_INITIALIZED == sg_is_initialized)
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 driver is already initialized.");
#endif  /* DEBUG */

        return AHT21_ERROR_RESOURCE;
    }

    /********** 2. Check the platform callbacks ***********/
#ifdef HARDWARE_IIC
    if ((NULL == p_iic_interface->pf_iic_init)         ||
        (NULL == p_iic_interface->pf_iic_start)        ||
        (NULL == p_iic_interface->pf_iic_stop)         ||
        (NULL == p_iic_interface->pf_iic_wait_ack)     ||
        (NULL == p_iic_interface->pf_iic_send_ack)     ||
        (NULL == p_iic_interface->pf_iic_send_nack)    ||
        (NULL == p_iic_interface->pf_iic_send_byte)    ||
        (NULL == p_iic_interface->pf_iic_receive_byte))
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 hardware IIC callback is missing.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }
#else
    const iic_bus_t * const p_bus = p_iic_interface->p_bus;

    if ((NULL == p_bus) || (NULL == p_bus->p_sda_port) ||
        (NULL == p_bus->p_scl_port) || (0U == p_bus->sda_pin) ||
        (0U == p_bus->scl_pin) ||
        (NULL == p_iic_interface->pf_critical_enter) ||
        (NULL == p_iic_interface->pf_critical_exit))
    {
#ifdef DEBUG
        elog_e(TAG,
               "AHT21 software IIC bus or critical callbacks are invalid.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }
#endif  /* HARDWARE_IIC */

#ifdef OS_SUPPORTING
    if ((NULL == p_rtos_delay) || (NULL == p_rtos_delay->pf_rtos_delay))
    {
#ifdef DEBUG
        elog_e(TAG, "AHT21 RTOS delay callback is missing.");
#endif  /* DEBUG */

        return AHT21_ERROR_PARAMETER;
    }
#endif  /* OS_SUPPORTING */

    /********* 3. Bind and initialize the driver **********/
    bsp_aht21_driver_t driver =
    {
        .p_iic_interface = p_iic_interface,
        .p_timebase      =      p_timebase,
#ifdef OS_SUPPORTING
        .p_rtos_delay = p_rtos_delay,
#endif  /* OS_SUPPORTING */

        .pf_aht21_inst                      =                          NULL,
        .pf_aht21_init                      =                    AHT21_Init,
        .pf_aht21_deinit                    =                  AHT21_Deinit,
        .pf_aht21_read_status               =              AHT21_ReadStatus,
        .pf_aht21_read_temperature_humidity = AHT21_ReadTemperatureHumidity,
        .pf_aht21_sleep                     =                   AHT21_Sleep,
        .pf_aht21_wakeup                    =                  AHT21_Wakeup,
    };

    const aht21_status_t result = AHT21_Init(&driver);
    if (AHT21_OK == result)
    {
        *p_self = driver;
    }

    return result;
}

/**************************** Function Definitions ****************************/
