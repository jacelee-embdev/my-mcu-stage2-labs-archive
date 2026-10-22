/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file swc_ota.h
 *
 * @par dependencies
 * - FreeRTOS.h
 * - queue.h
 * - semphr.h
 * - w25qxx_Handler.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares OTA states, shared resources, and the receive task.
 *
 * Processing flow:
 *
 * Create the OTA task during RTOS initialization. The task receives an
 * encrypted package into A, confirms state 0x22, and resets after the update
 * command and a valid key press. Bootloader installs the pending package.
 *
 * @version v1.2 2026-10-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SWC_OTA_H
#define __SWC_OTA_H

/*************************** Type Definitions ***************************/
typedef enum
{
    OTA_WAIT_DOWNLOAD = 0,   /* 等待下载命令。 */
    OTA_DOWNLOADING,         /* 正在接收并处理数据包。 */
    OTA_WAIT_UPDATE,         /* 等待更新命令。 */
    OTA_FINISHED             /* 等待按键触发系统软复位。 */
} ota_state_t;
/*************************** Type Definitions ***************************/

/******************************* Includes *******************************/
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "w25qxx_Handler.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
/* 与 Bootloader 共用的 EEPROM 状态及 A 区加密包容量。 */
#define OTA_EEPROM_NO_UPDATE          (0x00U)
#define OTA_EEPROM_DOWNLOADING        (0x11U)
#define OTA_EEPROM_DOWNLOAD_COMPLETE  (0x22U)
#define OTA_FIRMWARE_MAX_SIZE         W25Q64_DOWNLOAD_MAX_SIZE
/******************************* Defines *******************************/

/************************** External Variables **************************/
extern QueueHandle_t app_data_position_queue;  /* 文件大小及数据指针队列。 */
extern SemaphoreHandle_t exit_falsh_state_semaphore;  /* 处理完成信号量。 */
extern volatile uint8_t g_ota_data_write_fault;  /* 本次写入失败标记。 */
/************************** External Variables **************************/

/************************ Function Declarations ************************/
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
void swc_ota_task(void *p_argument);
/************************ Function Declarations ************************/

#endif /* 结束头文件保护。 */
