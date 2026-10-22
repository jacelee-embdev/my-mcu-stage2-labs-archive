/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file bootloader.h
 *
 * @par dependencies
 * - main.h
 * - aes.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares APP firmware installation and startup interfaces.
 *
 * Processing flow:
 *
 * Select the startup path from EEPROM state. Receive into A or use a
 * completed package, decrypt into B, back up the old APP, install the
 * new APP, and launch it or restore the valid backup after a failure.
 *
 * @version v1.4 2026-10-06
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

/******************************* Includes *******************************/
#include "main.h"
#include "aes.h"
/******************************* Includes *******************************/

/******************************* Defines *******************************/
/* APP 运行区基址及内部备用地址常量；当前旧 APP 备份写入外部 A 区。 */
#define APP_RUN_BASE_ADDR       ((uint32_t)0x08008000U)
#define APP_BACKUP_BASE_ADDR    ((uint32_t)0x08020000U)

/* APP 运行区容量及外部 A/B 分区所允许的加密包上限。 */
#define APP_RUN_SIZE            ((uint32_t)0x00018000U)
#define APP_BACKUP_SIZE         ((uint32_t)BLOCK_SIZE)
#define APP_SRAM_SIZE           ((uint32_t)0x00020000U)

/* SRAM 地址掩码常量；当前启动入口校验使用地址范围和对齐检查。 */
#define SRAM_ADDR_MASK          ((uint32_t)0x2FFE0000U)

#define NO_APP_UPDATE           (0X00)
#define APP_DOWNLOADING_FAULT    (0X11)
#define APP_DOWNLOAD_COMPLETE   (0X22)
/******************************* Defines *******************************/

/************************ Function Declarations ************************/

/**
 * @brief 校验内部 APP 启动入口并交接控制权。
 *
 * 处理步骤：
 * 1. 检查初始栈顶、复位入口地址和 Thumb 状态。
 * 2. 关闭中断并将向量表基址切换到 APP 运行区。
 * 3. 复位时钟配置，设置主堆栈指针并调用 APP 入口。
 * APP 启动代码负责初始化运行环境并重新使能中断。
 *
 * @return 无；入口无效时返回，正常启动 APP 后通常不会返回。
 */
void bootloader_jump_to_app(void);

/**
 * @brief 将 A 区加密固件解密为 B 区的有效 APP 明文。
 *
 * 处理步骤：
 * 1. 检查加密包长度和 A 区管理长度。
 * 2. 用初始向量解密文件头，获取并校验明文长度。
 * 3. 连续解密数据块，只将有效明文字节写入 B 区。
 * 4. 刷新尾部数据，校验明文长度及 APP 启动入口。
 * 每次更新重新设置初始向量；本阶段不修改内部 APP。
 *
 * @param[in] firmware_size A 区加密文件的实际字节数。
 *
 * @return 解密结果。
 * @retval 0 解密、写入和启动入口检查完成。
 * @retval -1 长度记录、包格式或 A 区读取无效。
 * @retval -2 B 区写入、刷新或长度检查失败。
 * @retval -3 解密后的 APP 启动入口无效。
 */
int8_t exitA_to_extiB(int32_t firmware_size);

/**
 * @brief 按有效长度将内部旧 APP 备份到外部 A 区。
 *
 * 检查旧 APP 的长度和启动入口，再重置 A 区管理状态。
 * 将旧 APP 明文完整写入 A 区并刷新尾部；失败时作废 A 区长度。
 *
 * @param[in] app_size EEPROM 记录的旧 APP 明文字节数。
 *
 * @return 旧 APP 备份结果。
 * @retval 0 备份完成。
 * @retval -1 旧 APP 长度或启动入口无效。
 * @retval -2 A 区写入或刷新失败。
 */
int8_t internal_APP_to_exti_A(int32_t app_size);

/**
 * @brief 将 B 区新 APP 安装到内部运行区。
 *
 * 调用公共搬运接口，从 B 区读取已解密的有效明文。
 *
 * @return 安装结果，沿用公共搬运接口的状态值。
 * @retval 0 安装完成。
 * @retval -1 长度或后续读取无效。
 * @retval -2 擦除或写入回读失败。
 * @retval -3 首段读取或启动入口检查失败。
 */
int8_t exti_B_to_internal_APP(void);

/**
 * @brief 将 A 区旧 APP 备份恢复到内部运行区。
 *
 * 调用公共搬运接口，从 A 区读取本次已经完成的有效旧 APP 备份。
 *
 * @return 恢复结果，沿用公共搬运接口的状态值。
 * @retval 0 恢复完成。
 * @retval -1 长度或后续读取无效。
 * @retval -2 擦除或写入回读失败。
 * @retval -3 首段读取或启动入口检查失败。
 */
int8_t exti_A_to_internal_APP(void);

/**
 * @brief 接收新加密包并执行完整安装流程。
 *
 * 通过静态缓冲区接收 YMODEM 文件到 A 区，然后解密、备份及安装。
 * 下载或升级返回后，主循环可等待按键并重新发起下载。
 *
 * @return 无；成功进入 APP 后通常不会返回。
 */
void bootloader_download_app(void);

/**
 * @brief 按 EEPROM 状态选择 APP 启动或固件升级路径。
 *
 * 处理步骤：
 * 1. 从 EEPROM 0x00 读取升级状态。
 * 2. 状态 0x00：按键有效时下载，否则尝试启动当前 APP。
 * 3. 状态 0x11：尝试启动旧 APP；返回后重新下载并完整安装。
 * 4. 状态 0x22：读取已有包长度，恢复 A 区管理参数并直接升级。
 * 读取失败或包长度无效时尝试启动当前 APP。
 * 状态处理返回后，主循环等待按键并提供重新下载。
 *
 * @return 无；成功进入 APP 后通常不会返回。
 */
void OTA_state_mangner(void);
/************************ Function Declarations ************************/

#endif /* __BOOTLOADER_H__ */
