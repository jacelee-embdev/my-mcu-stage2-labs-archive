/******************************************************************************
 * @file key_programmer.h
 *
 * @par dependencies
 * - key_programmer_config.h
 *
 * @brief Application AES key/IV EEPROM provisioning and chip diagnostics.
 *
 * Processing flow:
 * Initialize, authenticate, program one page, and verify its readback.
 * Page layout: IV[16], key[32], CRC8, inverted CRC8, reserved[14].
 * Requires user EEPROM; CK02AT cannot store this page.
 * Optional diagnostic mode performs checks without EEPROM writes.
 *
 * @version V1.0
 * @date 2026-10-12
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

#ifndef KEY_PROGRAMMER_H
#define KEY_PROGRAMMER_H

/***************************** Includes *****************************/

#include "key_programmer_config.h" /* 用户应用配置层 */

/***************************** Includes *****************************/

/**************************** Declaring *****************************/

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    KEY_PROGRAMMER_OK = 0,             /**< 全部检查通过。 */
    KEY_PROGRAMMER_NOT_INITIALIZED,    /**< 尚未完成初始化。 */
    KEY_PROGRAMMER_POWER_ON_FAILED,    /**< 唤醒失败。 */
    KEY_PROGRAMMER_INIT_FAILED,        /**< 初始化或响应帧无效。 */
    KEY_PROGRAMMER_AUTH_FAILED,        /**< 认证返回失败。 */
    KEY_PROGRAMMER_AUTH_DATA_MISMATCH, /**< 认证响应数据不匹配。 */
    KEY_PROGRAMMER_WRITE_FAILED,       /**< EEPROM 写入失败。 */
    KEY_PROGRAMMER_READ_FAILED,        /**< EEPROM 读取失败。 */
    KEY_PROGRAMMER_DATA_MISMATCH,      /**< 整页读回数据不匹配。 */
    KEY_PROGRAMMER_CRC_MISMATCH,       /**< CRC 或反码不匹配。 */
    KEY_PROGRAMMER_DIAGNOSTIC_FAILED,  /**< 诊断未找到全部通过的时序组。 */
    KEY_PROGRAMMER_I2C_FAILED          /**< 底层 I2C 事务失败。 */
} KeyProgrammer_Status;

/**
 * @brief 初始化软件 I2C 和芯片，并打印 SDK 解析的五字节序列号。
 *
 * @return 成功返回 KEY_PROGRAMMER_OK；失败返回唤醒、初始化或 I2C 错误。
 *
 * @note 在 HAL、时钟及日志初始化后调用；此步骤不执行认证或写入。
 * @note 启用诊断模式时仅准备 GPIO，实际芯片检查由 Run 执行。
 */
KeyProgrammer_Status KeyProgrammer_Init(void);

/**
 * @brief 认证通过后烧录应用 AES 密钥和 IV，并校验整页读回数据。
 *
 * @return 全部通过返回 KEY_PROGRAMMER_OK，否则返回对应阶段的模块错误码。
 *
 * @note 要求 Init 成功；每次调用均尝试写入，宜在启动时调用一次。
 * @note 64 字节页布局：IV 16 字节、密钥 32 字节、CRC 及反码各 1 字节，
 * @note 其余 14 字节清零；页号、内容及密码由配置头文件指定。
 * @note 要求带用户 EEPROM 的型号；CK02AT 无法保存该页。
 * @note 不写 eFuse，也不更改出厂认证密钥。
 * @note 启用诊断模式时改为时序、序列号、认证、bypass 和 AES 回环检查，
 * @note 诊断模式不写 EEPROM/eFuse。
 */
KeyProgrammer_Status KeyProgrammer_Run(void);

/**
 * @brief 持续输出 1 Hz 的 GPIO 诊断脉冲并统计电平不符次数。
 *
 * @return 无。
 *
 * @note HAL、时钟和日志初始化后调用，以此替代 Init/Run；阻塞直到复位。
 * @note 先保持 SDA 释放并翻转 SCL，再保持 SCL 低电平并翻转 SDA。
 * @note 清除模块初始化标志；不调用 SDK，不执行认证或 EEPROM 写入。
 */
void KeyProgrammer_BusProbe(void);

#ifdef __cplusplus
}
#endif

/**************************** Declaring *****************************/

#endif
