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
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares OTA states, shared resources, and the OTA task.
 *
 * Processing flow:
 *
 * Create swc_ota_task() during RTOS initialization.
 * Use the shared queue and semaphore for packet-processing handshakes.
 *
 * @version v1.1 2026-10-04
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __SWC_OTA_H
#define __SWC_OTA_H

/***************************** Type Definitions *****************************/
typedef enum
{
    OTA_WAIT_DOWNLOAD = 0,   /* 等待下载命令。 */
    OTA_DOWNLOADING,         /* 正在接收并处理数据包。 */
    OTA_WAIT_UPDATE,         /* 等待更新命令。 */
    OTA_FINISHED             /* 等待按键触发系统软复位。 */
} ota_state_t;
/***************************** Type Definitions *****************************/

/********************************* Includes *********************************/
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
/********************************* Includes *********************************/

/**************************** External Variables ****************************/
extern QueueHandle_t app_data_position_queue;  /* 文件大小及数据指针队列。 */
extern SemaphoreHandle_t exit_falsh_state_semaphore;  /* 处理完成信号量。 */
/**************************** External Variables ****************************/

/************************** Function Declarations ***************************/
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
void swc_ota_task(void *p_argument);
/************************** Function Declarations ***************************/

#endif /* 结束头文件保护。 */
