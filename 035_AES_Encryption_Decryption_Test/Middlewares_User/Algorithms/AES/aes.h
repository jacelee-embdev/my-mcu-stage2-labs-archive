/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aes.h
 *
 * @par dependencies
 * - main.h
 * - stdio.h
 * - string.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Provides AES configuration, initialization, encryption, and
 *        decryption APIs.
 *
 * Processing flow:
 *
 * Select the AES key length and operating mode, call aes_init() with the
 * original key, and then call aes_encrypt() or aes_decrypt() as required.
 *
 * @version V1.0 2026-09-22
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __AES_H__
#define __AES_H__

/****************************** Includes ******************************/

/* AES 接口依赖工程主头文件及标准输入输出和字符串接口。 */
#include "main.h"
#include <stdio.h>
#include <string.h>
/****************************** Includes ******************************/

/****************************** Defines ******************************/

/* AES 可选密钥长度。 */
typedef enum
{
    AES_KEY_LENGTH_128 = 128,  /* 128 位密钥。 */
    AES_KEY_LENGTH_192 = 192,  /* 192 位密钥。 */
    AES_KEY_LENGTH_256 = 256   /* 256 位密钥。 */
} aes_key_length_t;

/* 当前选用的密钥长度。 */
#define AES_KEY_LENGTH AES_KEY_LENGTH_128

/* AES 可选工作模式。 */
typedef enum
{
    AES_MODE_ECB = 0,  /* 电子密码本模式。 */
    AES_MODE_CBC = 1   /* 密码分组链接模式。 */
} aes_mode_t;

/* 当前选用的工作模式。 */
#define AES_MODE AES_MODE_CBC

/****************************** Defines ******************************/

/************************ Function Declarations ************************/

/**
 * @brief 初始化 AES，执行密钥扩展操作
 *
 * 此函数基于提供的原始密钥，生成 AES 加解密所需的轮密钥。
 *
 * @param[in] pKey 指向原始密钥的指针，密钥长度为 AES_KEY_LENGTH / 8 字节
 *
 * @return 无返回值
 */
void aes_init(const void *pKey);

/**
 * @brief AES 数据加密
 *
 * 此函数对给定的明文数据进行 AES 加密，生成对应的密文。
 *
 * @param[in]  pPlainText  指向明文数据的指针
 * @param[out] pCipherText 指向用于存储加密后密文数据的缓冲区
 * @param[in]  nDataLen    待加密数据长度，单位为字节，必须为 16 字节的整数倍
 * @param[in]  pIV         指向初始化向量的指针，CBC 模式下使用，ECB 模式下可为 NULL
 *
 * @return 无返回值
 */
void aes_encrypt(const unsigned char *pPlainText,
                 unsigned char *pCipherText,
                 unsigned int nDataLen,
                 const unsigned char *pIV);

/**
 * @brief AES 数据解密
 *
 * 此函数对给定的密文数据进行 AES 解密，生成对应的明文。
 *
 * @param[in]  pCipherText 指向密文数据的指针
 * @param[out] pPlainText  指向用于存储解密后明文数据的缓冲区
 * @param[in]  nDataLen    待解密数据长度，单位为字节，必须为 16 字节的整数倍
 * @param[in]  pIV         指向初始化向量的指针，CBC 模式下使用，ECB 模式下可为 NULL
 *
 * @return 无返回值
 */
void aes_decrypt(const unsigned char *pCipherText,
                 unsigned char *pPlainText,
                 unsigned int nDataLen,
                 const unsigned char *pIV);

/**
 * @brief 运行 AES-128 CBC 加解密示例。
 *
 * 打印密文、解密结果和往返校验结果。
 *
 * @return 无返回值
 */
void test(void);

/************************ Function Declarations ************************/

#endif /* __AES_H__ */
