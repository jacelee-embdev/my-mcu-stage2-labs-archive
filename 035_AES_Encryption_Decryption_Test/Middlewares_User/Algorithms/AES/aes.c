/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aes.c
 *
 * @par dependencies
 * - aes.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements the AES configuration, initialization, encryption, and
 *        decryption APIs.
 *
 * Processing flow:
 *
 * Select the AES key length and operating mode, call aes_init() with the
 * original key, and then call aes_encrypt() or aes_decrypt() as required.
 *
 * @version V1.0 2026-09-26
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "aes.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
/* AES 模块的日志标签。 */
#define TAG "aes"

/* 兼容 C51 与非 C51 编译环境。 */
#ifndef __C51__

#define code /* 非 C51 环境不使用存储区限定符。 */
#define data
#define idata
#define xdata
#define pdata
typedef unsigned char BOOL;
#else
    typedef bit bool;
#endif

/* 根据所选密钥长度计算密钥字数。 */
#define Nk (AES_KEY_LENGTH / 32)

/* AES 数据块固定为四个字。 */
#define Nb 4

/* 根据密钥长度选择轮数。 */
#if AES_KEY_LENGTH == AES_KEY_LENGTH_128
    #define Nr  10
#elif AES_KEY_LENGTH == AES_KEY_LENGTH_192
    #define Nr  12
#elif AES_KEY_LENGTH == AES_KEY_LENGTH_256
    #define Nr  14
#endif

/* 有限域约减所用的多项式常量。 */
#define BPOLY 0x1B
/****************************** Defines ******************************/

/*************************** Global Variables ***************************/
/* 保存密钥扩展生成的全部轮密钥。 */
static xdata unsigned char g_roundKeyTable[4 * Nb * (Nr + 1)];
/*************************** Global Variables ***************************/

/************************ S-Box Generation ************************/
/**
 * @brief 在 GF(2^8) 中计算两个字节的乘积。
 *
 * 逐位检查乘数，并按 AES 多项式对中间结果约减。
 *
 * @param[in] a 被乘数。
 * @param[in] b 乘数。
 *
 * @return 有限域乘积。
 */
unsigned char gmul(unsigned char a, unsigned char b)
{
    /* 保存有限域乘积。 */
    unsigned char p = 0;
    while (b)
    {
        if (b & 1)
        {
            p ^= a;
        }
        a = (a << 1) ^ ((a & 0x80) ? BPOLY : 0);
        b >>= 1;
    }
    return p;
}

/**
 * @brief 查找字节在 GF(2^8) 中的乘法逆元。
 *
 * 遍历候选值；零元素返回零。
 *
 * @param[in] x 待求逆的字节。
 *
 * @return 乘法逆元。
 */
unsigned char ginv(unsigned char x)
{
    if (0 == x)
    {
        return 0;
    }

    /* 逐一查找使乘积为一的候选元素。 */
    for (unsigned char i = 0; i < 256; i++)
    {
        if (1 == gmul(x, i))
        {
            return i;
        }
    }

    return 0;
}

/**
 * @brief 对字节执行 AES 仿射变换。
 *
 * 将字节循环移位后的结果与常量异或。
 *
 * @param[in] x 待变换的字节。
 *
 * @return 变换后的字节。
 */
unsigned char affineTransform(unsigned char x)
{
    /* 保存仿射变换结果。 */
    unsigned char result = x ^ ((x << 1) | (x >> 7)) ^
                           ((x << 2) | (x >> 6)) ^
                           ((x << 3) | (x >> 5)) ^
                           ((x << 4) | (x >> 4)) ^ 0x63;
    return result & 0xFF;
}

/**
 * @brief 生成 AES 正向 S 盒。
 *
 * 逐个计算乘法逆元与仿射变换，并处理零元素。
 *
 * @param[out] sbox 接收 256 个表项的数组。
 *
 * @return 无返回值
 */
void generate_aes_sbox(unsigned char sbox[256])
{
    for (int i = 0; i < 256; i++)
    {
        /* 当前字节的乘法逆元。 */
        unsigned char x = ginv(i);
        sbox[i] = affineTransform(x);  /* 应用仿射变换。 */
    }
    sbox[0] = 0x63;  /* 零元素对应的固定表项。 */
}

/**
 * @brief 由正向 S 盒生成逆向 S 盒。
 *
 * 按正向映射将原始索引写入逆向表。
 *
 * @param[in] sbox 包含 256 个表项的正向 S 盒。
 * @param[out] inv_sbox 接收逆向 S 盒的数组。
 *
 * @return 无返回值
 */
void generate_aes_inverse_sbox(const unsigned char sbox[256],
                               unsigned char inv_sbox[256])
{
    for (int i = 0; i < 256; i++)
    {
        inv_sbox[sbox[i]] = i;
    }
}

/**
 * @brief 打印动态生成的逆向 S 盒。
 *
 * 先生成正向 S 盒，再生成并输出逆向 S 盒。
 *
 * @return 无返回值
 */
void test_sbox(void)
{
    /* 正向 S 盒。 */
    unsigned char sbox[256];
    /* 逆向 S 盒。 */
    unsigned char inv_sbox[256];

    // 1. 生成 AES 加密使用的正向 S 盒。
    generate_aes_sbox(sbox);

    // 2. 根据正向 S 盒生成逆向 S 盒。
    generate_aes_inverse_sbox(sbox, inv_sbox);

    // 3. 按每行 16 项输出逆向 S 盒。
    elog_i(TAG, "AES Inverse S-Box:\n");
    for (int i = 0; i < 256; i++)
    {
        printf("0x%02X", inv_sbox[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
        else
            printf(", ");
    }
}

/**************************** S-Box Tables ****************************/
/* AES 正向字节替换表。 */
static code const unsigned char sBox[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/* AES 逆向字节替换表。 */
static code const unsigned char invSBox[256] =
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};
/**************************** S-Box Tables ****************************/

/************************ Function Definitions ************************/
/**
 * @brief 将一个四字节字循环左移一字节。
 *
 * 指针为空时直接返回。
 *
 * @param[in,out] pWord 指向四字节字的指针。
 *
 * @return 无返回值
 */
static void rotation_word(unsigned char *pWord)
{
    if (NULL == pWord)
    {
        return;
    }

    /* 暂存循环移位前的首字节。 */
    unsigned char temp = pWord[0];
    pWord[0] = pWord[1];
    pWord[1] = pWord[2];
    pWord[2] = pWord[3];
    pWord[3] = temp;
}

/**
 * @brief 使用正向或逆向 S 盒替换字节。
 *
 * 根据反向标志选择查找表，然后逐字节替换。
 *
 * @param[in,out] pState 待替换的状态数据。
 * @param[in] nCount 需要处理的字节数。
 * @param[in] bInvert 非零时使用逆向 S 盒。
 *
 * @return 无返回值
 */
static void sub_bytes(unsigned char *pState,
                      unsigned char nCount, BOOL bInvert)
{
    /* 根据方向选择字节替换表。 */
    const unsigned char code *pSBox = bInvert ? invSBox : sBox;

    for (int i = 0; i < nCount; i++)
    {
        pState[i] = pSBox[pState[i]];
    }
}

/**
 * @brief 将两组数据逐字节异或并写回第一组。
 *
 * 按指定字节数遍历数据。
 *
 * @param[in,out] pData1 接收异或结果的数据。
 * @param[in] pData2 参与异或的数据。
 * @param[in] nCount 需要处理的字节数。
 *
 * @return 无返回值
 */
static void xor_bytes(unsigned char *pData1,
                      const unsigned char *pData2,
                      unsigned char nCount)
{
    for (int i = 0; i < nCount; i++)
    {
        pData1[i] ^= pData2[i];
    }
}

/**
 * @brief 在 GF(2^8) 中将字节乘以二。
 *
 * 最高位为一时，对左移结果进行多项式约减。
 *
 * @param[in] num 待计算的字节。
 *
 * @return 乘以二后的字节。
 */
static unsigned char gf_mult_by02(unsigned char num)
{
    if (0 == (0x80 & num))
    {
        num = num << 1;
    }
    else
    {
        num = (num << 1) ^ BPOLY;
    }

    return num;
}

/* 将轮密钥逐字节异或到状态中。 */
#define AddRoundKey(pState, pRoundKey) xor_bytes((pState), (pRoundKey), 4 * Nb)

/**
 * @brief 对 AES 状态矩阵执行行移位。
 *
 * 逐行备份，再按加密或解密方向写回。
 *
 * @param[in,out] pState AES 状态矩阵。
 * @param[in] bInvert 非零时执行逆向行移位。
 *
 * @return 无返回值
 */
static void shift_rows(unsigned char *pState, BOOL bInvert)
{
    /* 行、列索引和位移量。 */
    unsigned char r, c, temp;
    /* 暂存当前行的四个字节。 */
    unsigned char rowData[4];

    for (r = 1; r < 4; r++)
    {
        for (c = 0; c < 4; c++)
        {
            rowData[c] = pState[r + 4 * c];
        }

        temp = bInvert ? (4 - r) : r;
        for (c = 0; c < 4; c++)
        {
            pState[r + 4 * c] = rowData[(c + temp) % 4];
        }
    }
}

/**
 * @brief 对状态矩阵执行正向或逆向列混合。
 *
 * 逐列计算正向列混合；解密时追加逆向修正。
 *
 * @param[in,out] pState AES 状态矩阵。
 * @param[in] bInvert 非零时执行逆向列混合。
 *
 * @return 无返回值
 */
static void mix_columns(unsigned char *pState, BOOL bInvert)
{
    unsigned char i;          /* 当前列索引。 */
    unsigned char temp;       /* 保存当前列的中间结果。 */
    unsigned char a0Pa2_M4;   /* 保存 4(a0 + a2)。 */
    unsigned char a1Pa3_M4;   /* 保存 4(a1 + a3)。 */
    unsigned char result[4];  /* 保存当前列的混合结果。 */

    // 对每一列进行混合操作
    for (i = 0; i < 4; i++, pState += 4)
    {
        // 计算 b0 = 2a0 + 3a1 + a2 + a3
        // temp 是所有四个字节的异或结果
        temp = pState[0] ^ pState[1] ^ pState[2] ^ pState[3];

        // 计算混合后的每个字节
        result[0] = temp ^ pState[0] ^
                    gf_mult_by02((unsigned char)(pState[0] ^ pState[1]));
        result[1] = temp ^ pState[1] ^
                    gf_mult_by02((unsigned char)(pState[1] ^ pState[2]));
        result[2] = temp ^ pState[2] ^
                    gf_mult_by02((unsigned char)(pState[2] ^ pState[3]));
        result[3] = temp ^ pState[3] ^
                    gf_mult_by02((unsigned char)(pState[3] ^ pState[0]));

        // 如果是解密，进行反向混合操作
        if (bInvert)
        {
            // 反向混合：b0' = 14a0 + 11a1 + 13a2 + 9a3
            a0Pa2_M4 = gf_mult_by02(
                gf_mult_by02((unsigned char)(pState[0] ^ pState[2]))
            );
            a1Pa3_M4 = gf_mult_by02(
                gf_mult_by02((unsigned char)(pState[1] ^ pState[3]))
            );
            temp = gf_mult_by02(
                (unsigned char)(a0Pa2_M4 ^ a1Pa3_M4)
            );

            // 更新结果
            result[0] ^= temp ^ a0Pa2_M4;
            result[1] ^= temp ^ a1Pa3_M4;
            result[2] ^= temp ^ a0Pa2_M4;
            result[3] ^= temp ^ a1Pa3_M4;
        }

        // 将混合后的结果写回状态数据
        memcpy(pState, result, 4);
    }
}

/**
 * @brief 加密一个 AES 数据块。
 *
 * 依次执行轮密钥加、字节替换、行移位和列混合。
 *
 * @param[in,out] pState 指向一个数据块的指针。
 *
 * @return 无返回值
 */
static void block_encrypt(unsigned char *pState)
{
    /* 当前加密轮次。 */
    unsigned char i;

    // 添加初始轮密钥
    AddRoundKey(pState, g_roundKeyTable);

    // 进行 Nr 轮加密操作，i 从 1 到 Nr
    for (i = 1; i <= Nr; i++)  /* 轮次从 1 到 Nr。 */
    {
        // 替换字节
        sub_bytes(pState, 4 * Nb, 0);

        // 行移位
        shift_rows(pState, 0);

        // 只有在最后一轮前，才进行列混合
        if (i != Nr)
        {
            mix_columns(pState, 0);
        }

        // 添加当前轮的轮密钥
        AddRoundKey(pState, &g_roundKeyTable[4 * Nb * i]);
    }

    // 首轮或末轮已在循环中处理，无须重复执行。
    // sub_bytes(pState, 4*Nb);
    // shift_rows(pState, 0);
    // AddRoundKey(pState, &g_roundKeyTable[4*Nb*Nr]);
}

/**
 * @brief 解密一个 AES 数据块。
 *
 * 按相反轮次执行逆向行移位、逆向字节替换和列混合。
 *
 * @param[in,out] pState 指向一个数据块的指针。
 *
 * @return 无返回值
 */
static void block_decrypt(unsigned char *pState)
{
    /* 当前解密轮次。 */
    unsigned char i;

    // 添加最后一轮的轮密钥
    AddRoundKey(pState, &g_roundKeyTable[4 * Nb * Nr]);

    // 进行 Nr 轮解密操作，i 从 Nr 到 1
    for (i = Nr; i > 0; i--)  /* 轮次从 Nr 到 1。 */
    {
        // 反向行移位
        shift_rows(pState, 1);

        // 反向替换字节
        sub_bytes(pState, 4 * Nb, 1);

        // 添加当前轮的轮密钥
        AddRoundKey(pState, &g_roundKeyTable[4 * Nb * (i - 1)]);

        // 只有在最后一轮前，才进行列反混合
        if (i != 1)
        {
            mix_columns(pState, 1);
        }
    }

    // 首轮或末轮已在循环中处理，无须重复执行。
    // shift_rows(pState, 1);
    // sub_bytes(pState, 4*Nb, 1);
    // AddRoundKey(pState, g_roundKeyTable);
}

/**
 * @brief 根据原始密钥生成 AES 轮密钥。
 *
 * 先复制原始密钥，再按密钥长度扩展剩余轮密钥。
 *
 * @param[in] pKey 指向原始密钥，长度由 AES_KEY_LENGTH 指定。
 *
 * @return 无返回值
 */
void aes_init(const void *pKey)
{
    /* 当前轮密钥的字索引。 */
    unsigned char i;
    unsigned char *pRoundKey;  /* 指向当前待生成的轮密钥。 */
    /* 密钥扩展使用的轮常量初始值。 */
    unsigned char Rcon[4] = {0x01, 0x00, 0x00, 0x00};

    // 将原始密钥复制到轮密钥表的开始部分
    memcpy(g_roundKeyTable, pKey, 4 * Nk);

    // 设置指向当前轮密钥的指针
    pRoundKey = &g_roundKeyTable[4 * Nk];

    // 生成从第 Nk 到 Nb*(Nr+1) 的轮密钥
    for (i = Nk; i < Nb * (Nr + 1); pRoundKey += 4, i++)
    {
        // 将前一个轮密钥复制到当前轮密钥
        memcpy(pRoundKey, pRoundKey - 4, 4);

        // 每扩展 Nk 个字时进行特殊处理
        if (i % Nk == 0)
        {
            // 旋转当前轮密钥
            rotation_word(pRoundKey);

            // 对轮密钥进行字节替换
            sub_bytes(pRoundKey, 4, 0);

            // 将轮常量与当前轮密钥异或
            xor_bytes(pRoundKey, Rcon, 4);

            // 更新轮常量
            Rcon[0] = gf_mult_by02(Rcon[0]);
        }
        // 针对 Nk 大于 6 的情况进行字节替换
        else if (Nk > 6 && i % Nk == Nb)
        {
            sub_bytes(pRoundKey, 4, 0);
        }

        // 与 Nk 前的轮密钥异或，生成新的轮密钥
        xor_bytes(pRoundKey, pRoundKey - 4 * Nk, 4);
    }
}

/**
 * @brief 加密完整的 AES 数据块。
 *
 * 可原地处理；CBC 模式逐块与初始化向量或前一密文块异或。
 *
 * @param[in] pPlainText 明文缓冲区。
 * @param[out] pCipherText 密文缓冲区，可与明文缓冲区相同。
 * @param[in] nDataLen 数据长度，按完整数据块处理。
 * @param[in] pIV CBC 模式使用的初始化向量。
 *
 * @return 无返回值
 */
void aes_encrypt(const unsigned char *pPlainText,
                 unsigned char *pCipherText,
                 unsigned int nDataLen,
                 const unsigned char *pIV)
{
    /* 剩余待处理的数据块数。 */
    unsigned int i;

    // 若密文和明文指针不同，将明文复制到密文存储区
    if (pPlainText != pCipherText)
    {
        memcpy(pCipherText, pPlainText, nDataLen);
    }

    // 分块加密，每个块大小为 4*Nb 字节
    for (i = nDataLen / (4 * Nb); i > 0; i--, pCipherText += 4 * Nb)
    {
        // 若使用 CBC 模式，进行块 XOR 操作
#if AES_MODE == AES_MODE_CBC
        xor_bytes(pCipherText, pIV, 4 * Nb);
#endif

        // 对当前块进行加密
        block_encrypt(pCipherText);

        // 更新 IV 为当前密文，用于下一块加密
        pIV = pCipherText;
    }
}

/**
 * @brief 解密完整的 AES 数据块。
 *
 * 从末块向前处理，以支持原地 CBC 解密。
 *
 * @param[in] pCipherText 密文缓冲区。
 * @param[out] pPlainText 明文缓冲区，可与密文缓冲区相同。
 * @param[in] nDataLen 数据长度，按完整数据块处理。
 * @param[in] pIV CBC 模式使用的初始化向量。
 *
 * @return 无返回值
 */
void aes_decrypt(const unsigned char *pCipherText,
                 unsigned char *pPlainText,
                 unsigned int nDataLen,
                 const unsigned char *pIV)
{
    /* 剩余待处理的数据块数。 */
    unsigned int i;

    // 若密文和明文指针不同，将密文复制到明文存储区
    if (pPlainText != pCipherText)
    {
        memcpy(pPlainText, pCipherText, nDataLen);
    }

    // 设置指针从最后一个数据块开始解密，避免开辟额外空间保存 IV
    pPlainText += nDataLen - 4 * Nb;
    for (i = nDataLen / (4 * Nb); i > 0; i--, pPlainText -= 4 * Nb)
    {
        // 对当前数据块进行解密
        block_decrypt(pPlainText);

        // 若为 CBC 模式，对解密后的数据块进行 XOR 操作
#if AES_MODE == AES_MODE_CBC
        if (i == 1)
        {
            // 第一块（初始块），与 IV 异或
            xor_bytes(pPlainText, pIV, 4 * Nb);
        }
        else
        {
            // 其余块，与前一个密文块异或
            xor_bytes(pPlainText, pPlainText - 4 * Nb, 4 * Nb);
        }
#endif
    }
}

/**
 * @brief 将字节数组按十六进制输出。
 *
 * 每个字节输出两位十六进制字符，最后换行。
 *
 * @param[in] bytes 待输出的字节数组。
 * @param[in] length 数组长度。
 *
 * @return 无返回值
 */
void print_hex(uint8_t *bytes, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x", bytes[i]);
    }
    printf("\n");
}

/**
 * @brief 运行 AES 加解密示例并打印结果。
 *
 * 初始化密钥、加密测试数据、解密并比较结果。
 *
 * @return 无返回值
 */
void test(void)
{
    /* 原始明文块。 */
    uint8_t buf[16] = {0};
    /* 保存加密后的数据块。 */
    uint8_t saveBuf[16] = {0};
    /* 保存解密结果并预留字符串结束符。 */
    uint8_t descryptBuf[17] = {0};
    /* 示例使用的 128 位密钥。 */
    const uint8_t AES128key[16] = "123456789abcdefa";
    /* CBC 模式的示例初始化向量。 */
    const uint8_t AES_IV[16] = "0102030405123456";
    /* 待加密的示例文本。 */
    const char *string = "JesMicro";

    memcpy(buf, string, strlen(string));
    aes_init(AES128key);
    aes_encrypt(buf, saveBuf, sizeof(buf), AES_IV);
    printf("aes_encrypt = ");
    print_hex(saveBuf, sizeof(saveBuf));

    aes_decrypt(saveBuf, descryptBuf, sizeof(buf), AES_IV);
    printf("descryptBuf = [%s]\r\n", (char *)descryptBuf);
    printf("AES roundtrip = %s\r\n",
           memcmp(buf, descryptBuf, sizeof(buf)) == 0 ? "PASS" : "FAIL");
}
