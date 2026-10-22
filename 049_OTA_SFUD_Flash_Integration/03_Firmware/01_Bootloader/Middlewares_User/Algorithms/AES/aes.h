/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aes.h
 *
 * @par dependencies
 * - None.
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Declares AES encryption and decryption interfaces supporting
 * 128/192/256-bit keys with optional CBC-style IV chaining.
 *
 * Processing flow:
 *
 * Pass a 16-byte data block, a key and an IV buffer to process one block.
 * When the IV pointer is NULL, the block is processed without chaining;
 * otherwise the IV buffer is updated for chaining the next block (CBC).
 *
 * @version V1.0 2026-09-27
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

#ifndef __AES_H__
#define __AES_H__

/****************************** Functions ******************************/
/**
 * @brief AES 加密单个 16 字节数据块（128bit 秘钥，可选 CBC 链接）。
 *
 * @param[in,out] IV_IN_OUT     链接向量输入、密文输出缓存（16 字节）；
 *                              传 NULL 表示不使用链接。
 * @param[in,out] State_IN_OUT  明文输入、密文输出（16 字节）。
 * @param[in]     key128bit     128bit 秘钥（16 字节）。
 *
 * @return None.
 */
void Aes_IV_key128bit_Encrypt(unsigned char *IV_IN_OUT,
                              unsigned char *State_IN_OUT,
                              unsigned char *key128bit);

/**
 * @brief AES 解密单个 16 字节数据块（128bit 秘钥，可选 CBC 链接）。
 *
 * @param[in,out] IV_IN_OUT     链接向量输入、原密文输出缓存（16 字节）；
 *                              传 NULL 表示不使用链接。
 * @param[in,out] State_IN_OUT  密文输入、明文输出（16 字节）。
 * @param[in]     key128bit     128bit 秘钥（16 字节）。
 *
 * @return None.
 */
void Aes_IV_key128bit_Decode(unsigned char *IV_IN_OUT,
                             unsigned char *State_IN_OUT,
                             unsigned char *key128bit);

/**
 * @brief AES 加密单个 16 字节数据块（192bit 秘钥，可选 CBC 链接）。
 *
 * @param[in,out] IV_IN_OUT     链接向量输入、密文输出缓存（16 字节）；
 *                              传 NULL 表示不使用链接。
 * @param[in,out] State_IN_OUT  明文输入、密文输出（16 字节）。
 * @param[in]     key192bit     192bit 秘钥（24 字节）。
 *
 * @return None.
 */
void Aes_IV_key192bit_Encrypt(unsigned char *IV_IN_OUT,
                              unsigned char *State_IN_OUT,
                              unsigned char *key192bit);

/**
 * @brief AES 解密单个 16 字节数据块（192bit 秘钥，可选 CBC 链接）。
 *
 * @param[in,out] IV_IN_OUT     链接向量输入、原密文输出缓存（16 字节）；
 *                              传 NULL 表示不使用链接。
 * @param[in,out] State_IN_OUT  密文输入、明文输出（16 字节）。
 * @param[in]     key192bit     192bit 秘钥（24 字节）。
 *
 * @return None.
 */
void Aes_IV_key192bit_Decode(unsigned char *IV_IN_OUT,
                             unsigned char *State_IN_OUT,
                             unsigned char *key192bit);

/**
 * @brief AES 加密单个 16 字节数据块（256bit 秘钥，可选 CBC 链接）。
 *
 * @param[in,out] IV_IN_OUT     链接向量输入、密文输出缓存（16 字节）；
 *                              传 NULL 表示不使用链接。
 * @param[in,out] State_IN_OUT  明文输入、密文输出（16 字节）。
 * @param[in]     key256bit     256bit 秘钥（32 字节）。
 *
 * @return None.
 */
void Aes_IV_key256bit_Encrypt(unsigned char *IV_IN_OUT,
                              unsigned char *State_IN_OUT,
                              unsigned char *key256bit);

/**
 * @brief AES 解密单个 16 字节数据块（256bit 秘钥，可选 CBC 链接）。
 *
 * @param[in,out] IV_IN_OUT     链接向量输入、原密文输出缓存（16 字节）；
 *                              传 NULL 表示不使用链接。
 * @param[in,out] State_IN_OUT  密文输入、明文输出（16 字节）。
 * @param[in]     key256bit     256bit 秘钥（32 字节）。
 *
 * @return None.
 */
void Aes_IV_key256bit_Decode(unsigned char *IV_IN_OUT,
                             unsigned char *State_IN_OUT,
                             unsigned char *key256bit);
/****************************** Functions ******************************/

#endif /* __AES_H__ */
