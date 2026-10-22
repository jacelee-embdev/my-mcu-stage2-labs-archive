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
 * - string.h
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
 * @brief Runs the APP download, completion, and reset workflow.
 *
 * Processing flow:
 *
 * Clear startup state, accept a download command, and commit state 0x11.
 * Receive an encrypted file into A through a synchronized write task.
 * Verify the file length and EEPROM metadata before committing state 0x22.
 * Prepare update reception, reply, and wait for an update command and key.
 *
 * @version v1.2 2026-10-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/******************************* Includes *******************************/
#include "swc_ota.h"

#include "usart.h"
#include "gpio.h"

#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "ymodem.h"
#include "debug.h"

#include "os_init.h"
#include "w25qxx_Handler.h"
#include "at24cxx.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
#define TAG "swc_ota"  /* 日志模块标识。 */
#define BUFFER_SIZE  1030  /* 单个最大数据包的接收缓冲区容量，单位为字节。 */
/******************************* Defines *******************************/

/****************************** Variables ******************************/
/* 两个接收缓冲区交替使用；处理完成后才允许接收下一包。 */
static ota_state_t gs_ota_state = OTA_WAIT_DOWNLOAD;  /* 当前升级状态。 */
static uint8_t gs_ota_cmd[4] = {0};  /* 串口命令接收缓冲区。 */
static uint8_t gs_ota_cmd_rx_started = 0U;  /* 命令接收或通知待处理。 */
uint8_t g_ymodemRec_bufA[BUFFER_SIZE];  /* 第一个数据包缓冲区。 */
uint8_t g_ymodemRec_bufB[BUFFER_SIZE];  /* 第二个数据包缓冲区。 */
static TaskHandle_t g_update_app_data_task_handle = NULL;  /* 写入线程句柄。 */
QueueHandle_t app_data_position_queue = NULL;  /* 文件大小及数据指针队列。 */
SemaphoreHandle_t exit_falsh_state_semaphore = NULL;  /* 处理完成通知信号量。 */
volatile uint8_t g_ota_data_write_fault = 0U;  /* 本次写入失败标记。 */
/****************************** Variables ******************************/

/************************* Function Definitions *************************/
/**
 * @brief 轮询 EEPROM 应答并等待内部写周期完成。
 *
 * 应在写入接口返回后调用；未应答时每次延时 1 毫秒。
 * 当前等待窗口为 10 毫秒。
 *
 * @return 器件就绪结果。
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
 * @brief 保存 EEPROM 升级状态并回读比较。
 *
 * 写入地址 0x00，等待内部写周期完成后再读取并确认结果。
 *
 * @param[in] ota_state 与 Bootloader 约定的升级状态字节。
 *
 * @return 状态保存结果。
 * @retval 1 写入和回读比较完成。
 * @retval 0 写入、等待、读取或比较失败。
 */
static uint8_t swc_ota_save_state(uint8_t ota_state)
{
    uint8_t read_state = OTA_EEPROM_NO_UPDATE;  /* EEPROM 回读的升级状态。 */

    if ((1U != ee_write_bytes(&ota_state, 0x00U, 1U)) ||
        (1U != EEPROM_WaitWriteComplete()) ||
        (1U != ee_read_bytes(&read_state, 0x00U, 1U)) ||
        (read_state != ota_state))
    {
        elog_e(TAG, "EEPROM state save/verify failed: 0x%02X",
               (unsigned int)ota_state);
        return 0U;
    }
    return 1U;
}

/**
 * @brief 保存包长度，再提交下载完成状态。
 *
 * 先写 EEPROM 0x01～0x04 并回读比较加密包长度。
 * 长度确认后才将状态提交为 0x22，并回读确认状态。
 *
 * @param[in] firmware_size 已完整接收的加密文件字节数。
 *
 * @return 元数据提交结果。
 * @retval 1 长度和状态已确认。
 * @retval 0 任一写入、等待、读取或比较失败。
 */
static uint8_t swc_ota_save_metadata(uint32_t firmware_size)
{
    uint32_t read_size = 0U;  /* EEPROM 回读的加密包长度。 */

    /* 长度先落盘，0x22 是 Bootloader 可以使用已有包的最后提交标志。 */
    if ((1U != ee_write_bytes((uint8_t *)&firmware_size, 0x01U,
                              sizeof(firmware_size))) ||
        (1U != EEPROM_WaitWriteComplete()) ||
        (1U != ee_read_bytes((uint8_t *)&read_size, 0x01U,
                             sizeof(read_size))) ||
        (read_size != firmware_size))
    {
        elog_e(TAG, "EEPROM firmware size save/verify failed");
        return 0U;
    }
    return swc_ota_save_state(OTA_EEPROM_DOWNLOAD_COMPLETE);
}

/**
 * @brief 删除本次下载资源并清空对应句柄。
 *
 * 按写入线程、数据指针队列、完成信号量的顺序检查并删除资源。
 * 空句柄直接跳过，已删除的句柄重新置空，以便后续下载。
 *
 * @return 无。
 */
static void swc_ota_release_resources(void)
{
    if (NULL != g_update_app_data_task_handle)
    {
        vTaskDelete(g_update_app_data_task_handle);
        g_update_app_data_task_handle = NULL;
    }
    if (NULL != app_data_position_queue)
    {
        vQueueDelete(app_data_position_queue);
        app_data_position_queue = NULL;
    }
    if (NULL != exit_falsh_state_semaphore)
    {
        vSemaphoreDelete(exit_falsh_state_semaphore);
        exit_falsh_state_semaphore = NULL;
    }
}

/**
 * @brief 准备三字节命令的串口空闲 DMA 接收。
 *
 * 已有接收或待处理通知时直接返回，不丢弃提前收到的命令。
 * 重新启动前中止旧接收、取空长度通知并清理缓冲区和溢出标志。
 * 启动接收后关闭 DMA 半传输中断。
 *
 * @return 命令接收准备结果。
 * @retval 1 接收已启动，或已有接收等待处理。
 * @retval 0 DMA 启动失败。
 */
static uint8_t swc_ota_start_command_receive(void)
{
    uint16_t recv_length = 0U;  /* 队列通知的命令接收长度。 */

    if (0U != gs_ota_cmd_rx_started)
    {
        return 1U;
    }
    (void)HAL_UART_AbortReceive(&huart1);
    while (pdTRUE == xQueueReceive(g_YmodemReclength_queue,
                                   &recv_length, 0U))
    {
    }
    memset(gs_ota_cmd, 0, sizeof(gs_ota_cmd));
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart1, gs_ota_cmd,
                                              sizeof(gs_ota_cmd)))
    {
        elog_e(TAG, "OTA command RX start failed");
        return 0U;
    }
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    gs_ota_cmd_rx_started = 1U;
    return 1U;
}

/**
 * @brief 等待长度通知并检查三字节命令是否匹配。
 *
 * 启动或保留已准备的命令接收，再阻塞等待通知。
 * 取到通知后清除接收标记；仅长度为三且字节完全匹配时返回成功。
 *
 * @param[in] p_command 期望收到的三个命令字节。
 *
 * @return 命令匹配结果。
 * @retval 1 完整命令匹配。
 * @retval 0 参数、启动、通知、长度或字节匹配失败。
 */
static uint8_t swc_ota_receive_command(const uint8_t *p_command)
{
    uint16_t recv_length = 0U;  /* 队列通知的命令接收长度。 */

    if ((NULL == p_command) ||
        (0U == swc_ota_start_command_receive()))
    {
        return 0U;
    }
    if ((pdTRUE != xQueueReceive(g_YmodemReclength_queue, &recv_length,
                                 portMAX_DELAY)) ||
        (3U != recv_length))
    {
        gs_ota_cmd_rx_started = 0U;
        return 0U;
    }
    gs_ota_cmd_rx_started = 0U;
    return (0 == memcmp(gs_ota_cmd, p_command, 3U)) ? 1U : 0U;
}

/**
 * @brief 等待低电平有效的升级确认按键。
 *
 * 循环检查按键，每轮等待 50 毫秒；检测到低电平后延时 20 毫秒消抖。
 * 两次采样均有效时返回成功；最多检查 400 轮。
 *
 * @return 按键等待结果。
 * @retval 1 检测到有效按键。
 * @retval -1 约 20 秒内未确认按键。
 */
static int8_t Key_Scan(void)
{
    for (uint16_t count = 0U; count < 400U; count++)
    {
        if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin))
        {
            vTaskDelay(pdMS_TO_TICKS(20U));
            if (GPIO_PIN_RESET ==
                HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin))
            {
                return 1;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50U));
    }
    return -1;
}

/**
 * @brief 屏蔽异常并请求 MCU 系统软复位。
 *
 * 设置异常屏蔽寄存器，然后请求系统复位。
 * 复位后的 Bootloader 根据 EEPROM 完成状态处理已有加密包。
 *
 * @return 无；系统复位后不返回。
 */
void SoftReset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

/**
 * @brief 通过指针队列接收有效数据并写入 A 区。
 *
 * 处理步骤：
 * 1. 接收文件长度指针，检查长度和对齐并通知准备完成。
 * 2. 循环接收数据指针，检查当前有效包长及剩余长度。
 * 3. 没有故障时追加写入 A 区，并扣减剩余长度。
 * 4. 成功或失败均通知接收线程，失败标记用于取消本次下载。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；线程持续等待消息，由 OTA 任务在下载完成或失败后删除。
 */
void update_app_data_task(void *p_argument)
{
    uint8_t *p_data = NULL;  /* 本次待写入的有效数据区首地址。 */
    int32_t *p_size_t = NULL;  /* 首条队列消息中的文件长度地址。 */
    uint32_t remaining_size = 0U;  /* 本文件尚未写入的有效字节数。 */
    uint32_t write_length = 0U;  /* 当前数据包的有效写入长度。 */
    (void)p_argument;

    /* 第一条消息是文件长度；本次下载索引已在创建线程前重置。 */
    if ((pdTRUE != xQueueReceive(app_data_position_queue, &p_size_t,
                                 portMAX_DELAY)) ||
        (NULL == p_size_t) || (*p_size_t <= 16) ||
        ((uint32_t)*p_size_t > OTA_FIRMWARE_MAX_SIZE) ||
        (0U != ((uint32_t)*p_size_t % 16U)))
    {
        g_ota_data_write_fault = 1U;
    }
    else
    {
        remaining_size = (uint32_t)*p_size_t;
    }
    if (pdTRUE != xSemaphoreGive(exit_falsh_state_semaphore))
    {
        elog_e(TAG, "File preparation signal failed");
    }

    for (;;)
    {
        if (pdTRUE != xQueueReceive(app_data_position_queue, &p_data,
                                    portMAX_DELAY))
        {
            continue;
        }
        if ((NULL == p_data) || (packet_length <= 0) ||
            ((uint32_t)packet_length > remaining_size))
        {
            g_ota_data_write_fault = 1U;
        }
        else if (0U == g_ota_data_write_fault)
        {
            write_length = (uint32_t)packet_length;
            if (0U != W25Q64_WriteData(p_data, write_length))
            {
                g_ota_data_write_fault = 1U;
            }
            else
            {
                remaining_size -= write_length;
            }
        }
        /* 失败也通知接收线程，由其取消传输并释放本次资源。 */
        if (pdTRUE != xSemaphoreGive(exit_falsh_state_semaphore))
        {
            elog_e(TAG, "Data completion signal failed");
        }
    }
}

/**
 * @brief 提交下载中状态并创建本次下载资源。
 *
 * 处理步骤：
 * 1. 将 EEPROM 状态写为 0x11 并回读确认。
 * 2. 重置 A 区管理状态并清除写入故障标记。
 * 3. 依次创建完成信号量、指针队列和写入线程。
 * 资源创建失败时释放已创建的资源；状态保存失败时直接返回。
 *
 * @return 下载准备结果。
 * @retval 1 资源准备完成。
 * @retval 0 状态保存或资源创建失败。
 */
static uint8_t swc_ota_prepare_download(void)
{
    /* 先撤销上一包的完成标志，确保断电后不会把半包当完整升级包。 */
    if (0U == swc_ota_save_state(OTA_EEPROM_DOWNLOADING))
    {
        return 0U;
    }
    W25Q64_ResetDownload();
    g_ota_data_write_fault = 0U;
    exit_falsh_state_semaphore = xSemaphoreCreateBinary();
    if (NULL != exit_falsh_state_semaphore)
    {
        app_data_position_queue = xQueueCreate(2U, sizeof(uint8_t *));
    }
    if ((NULL == exit_falsh_state_semaphore) ||
        (NULL == app_data_position_queue) ||
        (pdPASS != xTaskCreate(update_app_data_task, "update_app_data",
                               512U, NULL, 1U,
                               &g_update_app_data_task_handle)))
    {
        elog_e(TAG, "OTA download resource creation failed");
        swc_ota_release_resources();
        return 0U;
    }
    return 1U;
}

/**
 * @brief 接收应用内升级包，并确认更新命令和按键复位。
 *
 * 处理步骤：
 * 1. 启动时将 EEPROM 状态写为 0x00 并回读确认。
 * 2. 收到 11 22 33 后提交下载中状态 0x11，创建写入资源。
 * 3. 双缓冲接收加密包到 A 区，刷新尾部并校验实际写入长度。
 * 4. 保存并校验包长度，再提交完成状态 0x22 并回读确认。
 * 5. 释放下载资源，等待 200 毫秒并先启动更新命令接收。
 * 6. 回复 44 55 66，收到 77 88 99 后等待按键确认。
 * 7. 有效按键触发软复位，由 Bootloader 安装 A 区已有包。
 * 下载或元数据失败时返回等待下载；约 20 秒按键超时后也返回等待下载。
 * 启动状态清除失败会记录错误，任务继续运行。
 *
 * @param[in] p_argument 线程参数，当前未使用。
 *
 * @return 无；线程持续运行，软复位后重新进入 Bootloader。
 */
void swc_ota_task(void *p_argument)
{
    const uint8_t download_cmd[3] = {0x11U, 0x22U, 0x33U};  /* 进入下载流程的三字节命令。 */
    const uint8_t update_cmd[3] = {0x77U, 0x88U, 0x99U};  /* 进入按键确认流程的三字节命令。 */
    uint8_t ack_cmd[3] = {0x44U, 0x55U, 0x66U};  /* 完整下载及元数据确认后的应答。 */
    int32_t receive_result = 0;  /* 本次 YMODEM 文件接收结果。 */
    uint8_t download_valid = 0U;  /* 数据和元信息已经完整确认的标记。 */
    (void)p_argument;

    if (0U == swc_ota_save_state(OTA_EEPROM_NO_UPDATE))
    {
        elog_e(TAG, "Failed to clear startup OTA state");
    }
    for (;;)
    {
        switch (gs_ota_state)
        {
            case OTA_WAIT_DOWNLOAD:
                if ((1U == swc_ota_receive_command(download_cmd)) &&
                    (1U == swc_ota_prepare_download()))
                {
                    gs_ota_state = OTA_DOWNLOADING;
                }
                break;

            case OTA_DOWNLOADING:
                receive_result = Ymodem_Receive(g_ymodemRec_bufA,
                                                g_ymodemRec_bufB);
                download_valid = 0U;
                if ((receive_result > 16) &&
                    ((uint32_t)receive_result <= OTA_FIRMWARE_MAX_SIZE) &&
                    (0U == ((uint32_t)receive_result % 16U)) &&
                    (0U == g_ota_data_write_fault) &&
                    (0U == W25Q64_WriteData_End()) &&
                    ((uint32_t)receive_result == W25Q64_GetWrittenSize()))
                {
                    download_valid = swc_ota_save_metadata(
                        (uint32_t)receive_result);
                }
                swc_ota_release_resources();
                if (0U == download_valid)
                {
                    elog_e(TAG, "OTA download/commit failed: result=%ld",
                           (long)receive_result);
                    if (0U == swc_ota_save_state(OTA_EEPROM_DOWNLOADING))
                    {
                        elog_e(TAG, "Failed to invalidate OTA metadata");
                    }
                    gs_ota_state = OTA_WAIT_DOWNLOAD;
                    break;
                }

                /* 延时后先准备更新命令接收，再发送完成应答。 */
                gs_ota_state = OTA_WAIT_UPDATE;
                elog_i(TAG, "OTA ready: size=%ld, EEPROM state=0x22",
                       (long)receive_result);
                vTaskDelay(pdMS_TO_TICKS(200U));
                /* 先接收再应答，避免上位机立即发送更新命令时丢包。 */
                if (0U == swc_ota_start_command_receive())
                {
                    if (0U == swc_ota_save_state(OTA_EEPROM_DOWNLOADING))
                    {
                        elog_e(TAG, "Failed to invalidate OTA metadata");
                    }
                    gs_ota_state = OTA_WAIT_DOWNLOAD;
                    break;
                }
                if (HAL_OK != HAL_UART_Transmit(&huart1, ack_cmd, 3U, 1000U))
                {
                    elog_e(TAG, "OTA reply 44 55 66 transmit failed");
                }
                break;

            case OTA_WAIT_UPDATE:
                if (1U == swc_ota_receive_command(update_cmd))
                {
                    gs_ota_state = OTA_FINISHED;
                }
                break;

            case OTA_FINISHED:
                if (1 == Key_Scan())
                {
                    SoftReset();
                }
                else
                {
                    elog_w(TAG, "OTA key wait timed out; download again");
                    gs_ota_state = OTA_WAIT_DOWNLOAD;
                }
                break;

            default:
                gs_ota_state = OTA_WAIT_DOWNLOAD;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10U));
    }
}
/************************* Function Definitions *************************/
