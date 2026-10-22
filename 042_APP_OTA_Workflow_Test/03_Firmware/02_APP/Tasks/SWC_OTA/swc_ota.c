/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file swc_ota.c
 *
 * @par dependencies
 * - swc_ota.h
 * - usart.h
 * - gpio.h
 * - stdint.h
 * - stdio.h
 * - FreeRTOS.h
 * - task.h
 * - queue.h
 * - semphr.h
 * - ymodem.h
 * - debug.h
 * - os_init.h
 * - w25qxx_Handler.h
 * - at24cxx.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Runs the application OTA receive and reset workflow.
 *
 * Processing flow:
 *
 * Accept the download command and create packet-processing resources.
 * Receive YMODEM data through two SRAM buffers.
 * Reply after the host-processing delay, then flush external flash.
 * Store EEPROM metadata and read it back for verification.
 * Accept the update command, wait for the key, and reset the MCU.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/********************************* Includes *********************************/
#include "swc_ota.h"

#include "usart.h"
#include "gpio.h"

#include "stdint.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "ymodem.h"
#include "debug.h"

#include "os_init.h"
#include "w25qxx_Handler.h"
#include "at24cxx.h"
/********************************* Includes *********************************/

/********************************* Defines **********************************/
#define TAG "swc_ota"  /* 日志模块标识。 */
#define BUFFER_SIZE  1030  /* 单个最大数据包的接收缓冲区容量，单位为字节。 */
/********************************* Defines **********************************/

/******************************** Variables *********************************/
/* 两个接收缓冲区交替使用；处理完成后才允许接收下一包。 */
static ota_state_t gs_ota_state = OTA_WAIT_DOWNLOAD;  /* 当前升级状态。 */
static uint8_t gs_ota_cmd[4] = {0};  /* 串口命令接收缓冲区。 */
uint8_t g_ymodemRec_bufA[BUFFER_SIZE];  /* 第一个数据包缓冲区。 */
uint8_t g_ymodemRec_bufB[BUFFER_SIZE];  /* 第二个数据包缓冲区。 */
static TaskHandle_t g_update_app_data_task_handle = NULL;  /* 写入线程句柄。 */
QueueHandle_t app_data_position_queue = NULL;  /* 文件大小及数据指针队列。 */
SemaphoreHandle_t exit_falsh_state_semaphore;  /* 处理完成通知信号量。 */
/******************************** Variables *********************************/

/******************************** Functions *********************************/
/**
 * @brief 等待 EEPROM 内部写周期结束。
 *
 * 轮询器件应答，每次失败后延时 1 毫秒，超时窗口为 10 毫秒。
 * 应在写入接口返回后调用，确认器件就绪后再进行下一次访问。
 *
 * @return uint8_t 器件就绪结果。
 * @retval 1U 器件已应答。
 * @retval 0U 等待超时。
 */
static uint8_t EEPROM_WaitWriteComplete(void)
{
    uint32_t start_tick = HAL_GetTick();  /* 就绪轮询起始时间。 */

    do
    {
        if (1U == ee_check_ok())
        {
            return 1U;
        }
        vTaskDelay(pdMS_TO_TICKS(1U));
    }
    while ((uint32_t)(HAL_GetTick() - start_tick) < 10U);

    return 0U;
}

/**
 * @brief 等待低电平有效的按键，并执行消抖确认。
 *
 * 扫描到低电平后延时 10 个系统节拍，再读取一次进行确认。
 * 每轮间隔 50 个系统节拍，最多 400 轮；1 毫秒节拍下约等于 20 秒。
 *
 * @return int8_t 按键检测结果。
 * @retval 1 检测到有效按键。
 * @retval -1 扫描超时。
 */
static int8_t Key_Scan(void)
{
    GPIO_PinState io_ret = GPIO_PIN_SET;  /* 当前按键电平，低电平有效。 */
    uint16_t cnt = 400;  /* 最多扫描 400 次，每次间隔 50 个系统节拍。 */

    for (size_t i = 0; i < cnt; i++)
    {
        io_ret = HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin);
        if (GPIO_PIN_RESET == io_ret)
        {
            vTaskDelay(10);

            io_ret = HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin);
            if (GPIO_PIN_RESET == io_ret)
            {
                return 1;
            }
        }
        vTaskDelay(50);
    }
    return -1;
}

/**
 * @brief 触发单片机软复位。
 *
 * 设置异常屏蔽标记后调用系统复位接口，重新启动当前应用。
 *
 * @return 无；正常情况下系统复位，不再返回。
 */
void SoftReset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

/**
 * @brief 处理文件准备通知和外部闪存数据写入。
 *
 * 1. 从队列取出文件大小指针，并发送文件准备完成通知。
 * 2. 从队列逐包取出数据指针，按共享包长度调用缓冲写入接口。
 * 3. 每包处理后释放信号量，允许接收线程切换缓冲区并应答。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；线程持续运行。
 */
void update_app_data_task(void *p_argument)
{
    (void)p_argument;
    uint8_t *p_data = NULL;  /* 待处理数据区的首地址。 */
    int32_t *p_size_t = NULL;  /* 文件大小变量的地址。 */

    // 1. 第一条队列消息为文件大小指针，先完成文件准备握手。
    xQueueReceive(app_data_position_queue, &p_size_t, portMAX_DELAY);

    // 后续数据缓冲区指针的接收位置示例，当前保持禁用。
    // xQueueReceive(app_data_position_queue, &p_data, portMAX_DELAY);

    // 文件大小指针当前仅接收；按大小预擦除扇区的步骤尚未实现。

    /* 通知接收线程：文件准备处理完成。 */
    if (pdTRUE != xSemaphoreGive(exit_falsh_state_semaphore))
    {
        elog_e(TAG, "file preparation completion signal failed");
    }

    // 2. 持续接收数据指针，并在写入处理后通知接收线程。
    for (;;)
    {
        xQueueReceive(app_data_position_queue, &p_data, portMAX_DELAY);
        if (NULL == p_data)
        {
            elog_e(TAG, "received data pointer is NULL");
            continue;
        }

        // 将本包数据交给外部闪存缓冲写入接口。
        W25Q64_WriteData(p_data, (uint32_t)packet_length);

        /* 本包处理完成后，允许接收线程切换缓冲区并发送确认应答。 */
        if (pdTRUE != xSemaphoreGive(exit_falsh_state_semaphore))
        {
            elog_e(TAG, "data completion signal failed");
        }
    }
}

/**
 * @brief 运行应用内接收、外部闪存写入和按键复位状态机。
 *
 * 1. 收到 0x11、0x22、0x33 后创建资源，开始文件接收。
 * 2. 接收成功后等待 200 毫秒，发送 0x44、0x55、0x66。
 * 3. 写入外部闪存尾部数据，释放资源，并写入及回读 EEPROM 元数据。
 * 4. 收到 0x77、0x88、0x99 后等待按键，有效按键触发系统软复位。
 * 循环末尾延时 10 毫秒；部分资源创建失败时直接返回。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；正常情况下持续运行，系统复位后重新启动。
 */
void swc_ota_task(void *p_argument)
{
    (void)p_argument;
    uint16_t recv_length = 0U;  /* 命令接收长度。 */
    uint8_t ack_cmd[3] = {0x44, 0x55, 0x66};  /* 下载完成应答。 */
    int32_t receive_result = 0;  /* 接收结果；正值为文件字节数。 */
    uint8_t ee_ota_state = 0x00;  /* 启动时写入的升级状态。 */
    HAL_StatusTypeDef rx_status = HAL_OK;  /* 命令接收启动结果。 */

    // 启动时将 EEPROM 升级状态写为 0x00。
    ee_write_bytes(&ee_ota_state, 0x00, 1);
    for (;;)
    {
        switch (gs_ota_state)
        {
            case OTA_WAIT_DOWNLOAD:
                // 1. 启动串口空闲检测 DMA 接收。
                HAL_UARTEx_ReceiveToIdle_DMA(&huart1, gs_ota_cmd, 4);
                // 2. 阻塞等待中断发送的接收长度通知。
                xQueueReceive(
                    g_YmodemReclength_queue,
                    &recv_length,
                    portMAX_DELAY);
                // 3. 校验命令长度，下载命令应为三个字节。
                if (3 == recv_length)
                {
                    // 校验下载命令 0x11、0x22、0x33。
                    if ((0x11 == gs_ota_cmd[0]) &&
                        (0x22 == gs_ota_cmd[1]) &&
                        (0x33 == gs_ota_cmd[2]))
                    {
                        // 4. 切换至下载状态。
                        gs_ota_state = OTA_DOWNLOADING;
                        // 创建完成信号量、指针队列和数据写入线程。
                        exit_falsh_state_semaphore = xSemaphoreCreateBinary();
                        if (NULL == exit_falsh_state_semaphore)
                        {
                            elog_e(TAG, "completion semaphore creation failed");
                            gs_ota_state = OTA_WAIT_DOWNLOAD;
                            break;
                        }
                        app_data_position_queue = xQueueCreate(
                            2U,
                            sizeof(uint8_t *));
                        if (NULL == app_data_position_queue)
                        {
                            elog_e(TAG, "Queue creation failed.\r\n");
                            return;
                        }
                        if (pdPASS != xTaskCreate(
                            update_app_data_task,
                            "update_app_data",
                            512U,
                            NULL,
                            1U,
                            &g_update_app_data_task_handle))
                        {
                            elog_e(
                                TAG,
                                "update app data task creation failed.\r\n");
                            return;
                        }

                        // 将 EEPROM 升级状态写为 0x11。
                        uint8_t ee_ota_state = 0x11;  /* 本次下载的升级状态。 */
                        ee_write_bytes(&ee_ota_state, 0x00, 1);
                    }
                    else
                    {
                        gs_ota_cmd[0] = 0;
                        gs_ota_cmd[1] = 0;
                        gs_ota_cmd[2] = 0;
                        gs_ota_cmd[3] = 0;
                    }
                }
                else
                {
                    gs_ota_cmd[0] = 0;
                    gs_ota_cmd[1] = 0;
                    gs_ota_cmd[2] = 0;
                    gs_ota_cmd[3] = 0;
                }
                break;

            case OTA_DOWNLOADING:
                receive_result = Ymodem_Receive(
                    g_ymodemRec_bufA,
                    g_ymodemRec_bufB);
                if (receive_result > 0)
                {
                    elog_i(
                        TAG,
                        "YMODEM completed: size=%ld",
                        (long)receive_result);
                    gs_ota_state = OTA_WAIT_UPDATE;
                    /* 等待 200 毫秒，让上位机退出协议传输处理，再发送完成应答。 */
                    vTaskDelay(pdMS_TO_TICKS(200U));
                    HAL_StatusTypeDef tx_status = HAL_UART_Transmit(
                        &huart1,
                        &ack_cmd[0],
                        3,
                        1000);
                    if (HAL_OK != tx_status)
                    {
                        elog_e(
                            TAG,
                            "OTA reply 44 55 66 transmit failed: status=%d",
                            (int)tx_status);
                    }

                    W25Q64_WriteData_End();
                    // 删除本次下载创建的线程、队列和信号量。
                    vTaskDelete(g_update_app_data_task_handle);
                    vQueueDelete(app_data_position_queue);
                    vSemaphoreDelete(exit_falsh_state_semaphore);

                    // 将 EEPROM 升级状态写为 0x11。
                    uint8_t ee_ota_state = 0x11;  /* 本次下载的升级状态。 */
                    if (1U != ee_write_bytes(&ee_ota_state, 0x00, 1))
                    {
                        elog_e(TAG, "EEPROM OTA state write failed");
                        break;
                    }
                    if (1U != EEPROM_WaitWriteComplete())
                    {
                        elog_e(
                            TAG,
                            "EEPROM OTA state write completion timeout");
                        break;
                    }

                    if (1U != ee_write_bytes(
                        (uint8_t *)&receive_result,
                        0x01,
                        sizeof(receive_result)))
                    {
                        elog_e(TAG, "EEPROM file size write failed");
                        break;
                    }
                    if (1U != EEPROM_WaitWriteComplete())
                    {
                        elog_e(
                            TAG,
                            "EEPROM file size write completion timeout");
                        break;
                    }

                    // 回读已写入的升级状态和文件大小，输出验证结果。
                    uint8_t ee_read_state = 0;  /* 回读的升级状态。 */
                    if (1U != ee_read_bytes(&ee_read_state, 0x00, 1))
                    {
                        elog_e(TAG, "EEPROM OTA state read failed");
                        break;
                    }
                    elog_i(TAG, "ee_read_state:%x\r\n", ee_read_state);
                    uint8_t data_read_len[4] = {0};  /* 文件大小的四个原始字节。 */
                    if (1U != ee_read_bytes(&data_read_len[0], 0x01, 4))
                    {
                        elog_e(TAG, "EEPROM file size read failed");
                        break;
                    }
                    char data_read_text[12] = {0};  /* 四个字节的十六进制显示字符串。 */
                    for (uint8_t i = 0U; i < 4U; i++)
                    {
                        (void)snprintf(&data_read_text[i * 3U],
                                       sizeof(data_read_text) - i * 3U,
                                       "%02X%s", (unsigned int)data_read_len[i],
                                       (i == 3U) ? "" : " ");
                    }
                    elog_i(TAG, "data_read_len:%s", data_read_text);
                }
                else
                {
                    elog_e(
                        TAG,
                        "YMODEM failed: result=%ld",
                        (long)receive_result);
                    // 接收失败时返回等待下载状态。
                    gs_ota_state = OTA_WAIT_DOWNLOAD;
                    // 删除本次下载创建的线程、队列和信号量。
                    vTaskDelete(g_update_app_data_task_handle);
                    vQueueDelete(app_data_position_queue);
                    vSemaphoreDelete(exit_falsh_state_semaphore);
                }
                break;

            case OTA_WAIT_UPDATE:
                // 1. 启动串口空闲检测 DMA 接收。
                rx_status = HAL_UARTEx_ReceiveToIdle_DMA(
                    &huart1,
                    gs_ota_cmd,
                    4);
                if (HAL_OK != rx_status)
                {
                    elog_e(
                        TAG,
                        "OTA command RX start failed: status=%d, RxState=0x%lX, ErrorCode=0x%lX",
                        (int)rx_status, (unsigned long)huart1.RxState,
                        (unsigned long)huart1.ErrorCode);
                }
                // 2. 阻塞等待中断发送的接收长度通知。
                xQueueReceive(
                    g_YmodemReclength_queue,
                    &recv_length,
                    portMAX_DELAY);
                if (3 == recv_length)
                {
                    if ((0x77 == gs_ota_cmd[0]) &&
                        (0x88 == gs_ota_cmd[1]) &&
                        (0x99 == gs_ota_cmd[2]))
                    {
                        gs_ota_state = OTA_FINISHED;
                    }
                    else
                    {
                        gs_ota_cmd[0] = 0;
                        gs_ota_cmd[1] = 0;
                        gs_ota_cmd[2] = 0;
                        gs_ota_cmd[3] = 0;
                    }
                }
                else
                {
                    gs_ota_cmd[0] = 0;
                    gs_ota_cmd[1] = 0;
                    gs_ota_cmd[2] = 0;
                    gs_ota_cmd[3] = 0;
                }
                break;

            case OTA_FINISHED:
                // 等待有效按键；超时则返回等待下载状态。
                if (1 == Key_Scan())
                {
                    // 发起单片机软复位，重新启动当前应用。
                    SoftReset();
                }
                else
                {
                    elog_w(
                        TAG,
                        "OTA key wait timed out; return to OTA_WAIT_DOWNLOAD");
                    gs_ota_state = OTA_WAIT_DOWNLOAD;
                }
                break;
            default:
                // 未知状态不执行操作。
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10U));
    }
}
/******************************** Functions *********************************/
