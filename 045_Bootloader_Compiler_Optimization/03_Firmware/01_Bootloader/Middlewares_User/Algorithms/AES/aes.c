/******************************************************************************
 * Copyright (C) 2026 JesMicro.
 *
 * All Rights Reserved.
 *
 * @file aes.c
 *
 * @par dependencies
 * - aes.h
 * - string.h
 *
 * @author Jace | Development Dept. | JesMicro
 *
 * @brief Implements the AES block cipher (FIPS-197) supporting 128/192/256-bit
 * keys, including key expansion and optional CBC-style IV chaining.
 *
 * Processing flow:
 *
 * XOR the data block with the IV when chaining is enabled, expand the user
 * key into round keys, then run the SubBytes / ShiftRows / MixColumns /
 * AddRoundKey rounds; decryption applies the inverse transformations in
 * reverse order.
 *
 * @version V1.0 2026-09-27
 *
 * @note 1 tab == 4 spaces!
 *
 ******************************************************************************/

/****************************** Includes ******************************/
#include "aes.h"

#include <string.h>
/****************************** Includes ******************************/

/****************************** Variables ******************************/
/* 轮秘钥缓存：原始秘钥 + 扩展产生的各轮子秘钥 */
static unsigned char Round_Key[256];

/* S 盒替换表，字节代替（SubBytes）变换中查表使用：高 4 位为行、低 4 位为列 */
const unsigned char aes_s_box[16][16] = {
    /* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,  /* 0 */
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,  /* 1 */
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,  /* 2 */
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,  /* 3 */
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,  /* 4 */
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,  /* 5 */
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,  /* 6 */
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,  /* 7 */
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,  /* 8 */
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,  /* 9 */
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,  /* a */
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,  /* b */
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,  /* c */
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,  /* d */
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,  /* e */
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16   /* f */
};

/* 逆 S 盒替换表，逆向字节代替（InvSubBytes）变换中查表使用 */
const unsigned char aes_inv_s_box[16][16] = {
    /* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,  /* 0 */
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,  /* 1 */
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,  /* 2 */
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,  /* 3 */
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,  /* 4 */
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,  /* 5 */
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,  /* 6 */
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,  /* 7 */
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,  /* 8 */
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,  /* 9 */
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,  /* a */
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,  /* b */
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,  /* c */
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,  /* d */
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,  /* e */
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d   /* f */
};

/* 列混淆（MixColumns）计算矩阵：左乘状态矩阵，即本矩阵的行乘以状态矩阵的列 */
const unsigned char aes_MixColumns[4][4] = {
    0x02, 0x03, 0x01, 0x01,  /* 0 */
    0x01, 0x02, 0x03, 0x01,  /* 1 */
    0x01, 0x01, 0x02, 0x03,  /* 2 */
    0x03, 0x01, 0x01, 0x02,  /* 3 */
};

/* 逆列混淆（InvMixColumns）计算矩阵 */
const unsigned char aes_invMixColumns[4][4] = {
    0x0E, 0x0B, 0x0D, 0x09,  /* 0 */
    0x09, 0x0E, 0x0B, 0x0D,  /* 1 */
    0x0D, 0x09, 0x0E, 0x0B,  /* 2 */
    0x0B, 0x0D, 0x09, 0x0E,  /* 3 */
};

/* 轮常数，秘钥扩展 G 函数使用；后一个值为前一个值在 GF(2^8) 域上乘 2 的结果 */
const unsigned char aes_Rcon[14] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
    0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d
};
/****************************** Variables ******************************/

/****************************** Functions ******************************/
/**
 * @brief AES 字节代替（SubBytes）：逐字节查 S 盒完成替换。
 *
 * @param[in,out] Byte_IN_OUT  待替换的数据缓冲区，结果就地写回。
 * @param[in]     Len          数据长度（字节）。
 *
 * @return None.
 */
void Aes_SubBytes(unsigned char *Byte_IN_OUT, unsigned int Len)
{
    unsigned char S_X;
    unsigned char S_Y;
    unsigned int Count = 0;

    for (Count = 0; Count < Len; Count++)
    {
        S_X = Byte_IN_OUT[Count] >> 4;  /* 取高 4 位作为 S 盒行索引 */
        S_Y = Byte_IN_OUT[Count] & 0x0f;  /* 取低 4 位作为 S 盒列索引 */
        Byte_IN_OUT[Count] = aes_s_box[S_X][S_Y];  /* 查表替换 */
    }
}

/**
 * @brief AES 逆向字节代替（InvSubBytes）：逐字节查逆 S 盒完成替换。
 *
 * @param[in,out] Byte_IN_OUT  待替换的数据缓冲区，结果就地写回。
 * @param[in]     Len          数据长度（字节）。
 *
 * @return None.
 */
void Aes_invSubBytes(unsigned char *Byte_IN_OUT, unsigned int Len)
{
    unsigned char S_X;
    unsigned char S_Y;
    unsigned int Count = 0;

    for (Count = 0; Count < Len; Count++)
    {
        S_X = Byte_IN_OUT[Count] >> 4;  /* 取高 4 位作为逆 S 盒行索引 */
        S_Y = Byte_IN_OUT[Count] & 0x0f;  /* 取低 4 位作为逆 S 盒列索引 */
        Byte_IN_OUT[Count] = aes_inv_s_box[S_X][S_Y];  /* 查表替换 */
    }
}

/**
 * @brief 将 4x4 状态矩阵中指定行的数据循环左移指定字节数。
 *
 * @param[in,out] SrcInOut  16 字节状态缓冲区，内部按 4x4 矩阵方式访问。
 * @param[in]     Rows      行号（0~3）。
 * @param[in]     Number    循环左移的字节数（1~3）。
 *
 * @return None.
 */
void ShiftByteNumRows(unsigned char *SrcInOut,
                      unsigned char Rows,
                      unsigned char Number)
{
    unsigned char Data[10];  /* 被移出数据的备份缓存 */
    unsigned char Count;

    if ((Number > 10) || (Number == 0))
    {
        return;
    }
    for (Count = 0; Count < Number; Count++)  /* 先备份将被移出该行的数据 */
    {
        Data[Count] = SrcInOut[Rows + 4 * Count];
    }
    for (Count = 0; Count < (4 - Number); Count++)  /* 行内其余数据依次左移 */
    {
        SrcInOut[Rows + 4 * Count] = SrcInOut[Rows + 4 * (Number + Count)];
    }
    for (Count = 0; Count < Number; Count++)  /* 备份数据回填到行尾，完成循环移位 */
    {
        SrcInOut[Rows + 4 * ((4 - Number) + Count)] = Data[Count];
    }
}

/**
 * @brief AES 行移位（ShiftRows）：第 0 行不移位，第 1/2/3 行分别循环左移
 * 1/2/3 个字节。
 *
 * @param[in,out] SrcInOut  16 字节状态矩阵。
 *
 * @return None.
 */
void Aes_ShiftRows(unsigned char *SrcInOut)
{
    /* 第 0 行不移位 */
    ShiftByteNumRows(SrcInOut, 1, 1);  /* 第 1 行左移 1 个字节 */
    ShiftByteNumRows(SrcInOut, 2, 2);  /* 第 2 行左移 2 个字节 */
    ShiftByteNumRows(SrcInOut, 3, 3);  /* 第 3 行左移 3 个字节 */
}

/**
 * @brief AES 逆向行移位（InvShiftRows）：与加密移位方向相反，第 0 行不移位，
 * 第 1/2/3 行分别循环左移 3/2/1 个字节。
 *
 * @param[in,out] SrcInOut  16 字节状态矩阵。
 *
 * @return None.
 */
void Aes_invShiftRows(unsigned char *SrcInOut)
{
    /* 第 0 行不移位 */
    ShiftByteNumRows(SrcInOut, 1, 3);  /* 加密左移 1 字节，逆向左移 3 字节 */
    ShiftByteNumRows(SrcInOut, 2, 2);  /* 加密左移 2 字节，逆向左移 2 字节 */
    ShiftByteNumRows(SrcInOut, 3, 1);  /* 加密左移 3 字节，逆向左移 1 字节 */
}

/**
 * @brief 计算两个字节在 GF(2^8) 域上的乘法。
 *
 * 把乘数按 bit 分解后逐位累加部分积：某一位为 1 时，结果异或被乘数的
 * 当前值；每轮被乘数左移一位，最高位溢出时异或不可约多项式系数 0x1b。
 *
 * @param[in] data0  被乘数。
 * @param[in] data1  乘数。
 *
 * @return GF(2^8) 域上的乘积。
 */
unsigned char Get_Calculate_GF28(unsigned char data0, unsigned char data1)
{
    unsigned char Val = 0;
    unsigned char Count = 0;

    for (Count = 0; Count < 8; Count++)
    {
        if (data1 & 0x01)  /* 当前位为 1，累加一次被乘数 */
        {
            Val ^= data0;
        }
        data0 = (data0 << 1) ^ ((data0 & 0x80) ? 0x1b : 0);  /* 被乘数翻倍，溢出时约减 */
        data1 >>= 1;  /* 扫描下一位 */
    }
    return Val;
}

/**
 * @brief AES 列混淆（MixColumns）：列混淆矩阵左乘状态矩阵的每一列。
 *
 * @param[in,out] SrcInOut  16 字节状态矩阵，结果就地写回。
 *
 * @return None.
 */
void Aes_MixColumns(unsigned char *SrcInOut)
{
    unsigned char Data_buff[4];  /* 一列数据的备份 */
    unsigned char Calculate_val[4];  /* 混淆计算后的结果 */
    unsigned char Count;  /* 列计数 */

    for (Count = 0; Count < 4; Count++)  /* 依次处理 4 列 */
    {
        memcpy(Data_buff, &SrcInOut[Count * 4], 4);  /* 备份当前列数据 */

        /* GF(2^8) 域乘加：列混淆矩阵每行与状态矩阵该列对应相乘后异或 */
        Calculate_val[0] =
            Get_Calculate_GF28(Data_buff[0], aes_MixColumns[0][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_MixColumns[0][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_MixColumns[0][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_MixColumns[0][3]);

        Calculate_val[1] =
            Get_Calculate_GF28(Data_buff[0], aes_MixColumns[1][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_MixColumns[1][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_MixColumns[1][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_MixColumns[1][3]);

        Calculate_val[2] =
            Get_Calculate_GF28(Data_buff[0], aes_MixColumns[2][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_MixColumns[2][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_MixColumns[2][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_MixColumns[2][3]);

        Calculate_val[3] =
            Get_Calculate_GF28(Data_buff[0], aes_MixColumns[3][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_MixColumns[3][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_MixColumns[3][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_MixColumns[3][3]);

        memcpy(&SrcInOut[Count * 4], Calculate_val, 4);  /* 结果写回该列 */
    }
}

/**
 * @brief AES 逆向列混淆（InvMixColumns）：逆列混淆矩阵左乘状态矩阵的每一列。
 *
 * @param[in,out] SrcInOut  16 字节状态矩阵，结果就地写回。
 *
 * @return None.
 */
void Aes_invMixColumns(unsigned char *SrcInOut)
{
    unsigned char Data_buff[4];  /* 一列数据的备份 */
    unsigned char Calculate_val[4];  /* 混淆计算后的结果 */
    unsigned char Count;  /* 列计数 */

    for (Count = 0; Count < 4; Count++)  /* 依次处理 4 列 */
    {
        memcpy(Data_buff, &SrcInOut[Count * 4], 4);  /* 备份当前列数据 */

        /* GF(2^8) 域乘加：逆列混淆矩阵每行与状态矩阵该列对应相乘后异或 */
        Calculate_val[0] =
            Get_Calculate_GF28(Data_buff[0], aes_invMixColumns[0][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_invMixColumns[0][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_invMixColumns[0][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_invMixColumns[0][3]);

        Calculate_val[1] =
            Get_Calculate_GF28(Data_buff[0], aes_invMixColumns[1][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_invMixColumns[1][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_invMixColumns[1][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_invMixColumns[1][3]);

        Calculate_val[2] =
            Get_Calculate_GF28(Data_buff[0], aes_invMixColumns[2][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_invMixColumns[2][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_invMixColumns[2][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_invMixColumns[2][3]);

        Calculate_val[3] =
            Get_Calculate_GF28(Data_buff[0], aes_invMixColumns[3][0]) ^
            Get_Calculate_GF28(Data_buff[1], aes_invMixColumns[3][1]) ^
            Get_Calculate_GF28(Data_buff[2], aes_invMixColumns[3][2]) ^
            Get_Calculate_GF28(Data_buff[3], aes_invMixColumns[3][3]);

        memcpy(&SrcInOut[Count * 4], Calculate_val, 4);  /* 结果写回该列 */
    }
}

/**
 * @brief 秘钥扩展中的 G 函数：先循环左移 1 个字节，再做 S 盒替换，最后与
 * 轮常数异或。
 *
 * @param[in,out] Wjbuff_IN_OUT  参与扩展的 4 字节数据，结果就地写回。
 * @param[in]     Count_j        轮计数，用于索引轮常数 aes_Rcon。
 *
 * @return None.
 */
void Aes_G_function(unsigned char *Wjbuff_IN_OUT, unsigned char Count_j)
{
    unsigned char temp;

    temp = Wjbuff_IN_OUT[0];  /* 循环左移 1 个字节 */
    memcpy(Wjbuff_IN_OUT, &Wjbuff_IN_OUT[1], 3);
    Wjbuff_IN_OUT[3] = temp;
    Aes_SubBytes(Wjbuff_IN_OUT, 4);  /* S 盒替换 */

    Wjbuff_IN_OUT[0] ^= aes_Rcon[Count_j];  /* 与轮常数异或 */
    Wjbuff_IN_OUT[1] ^= 0x00;  /* 其余字节异或 0x00，保持不变 */
    Wjbuff_IN_OUT[2] ^= 0x00;
    Wjbuff_IN_OUT[3] ^= 0x00;
}

/**
 * @brief 两个缓冲区按字节异或，结果写回 Byte_IO。
 *
 * @param[in,out] Byte_IO  参与异或并接收结果的缓冲区。
 * @param[in]     Byte_I   另一个参与异或的输入缓冲区。
 * @param[in]     Len      异或的长度（字节）。
 *
 * @return None.
 */
void Aes_Bytes_Xor(unsigned char *Byte_IO,
                   unsigned char *Byte_I,
                   unsigned int Len)
{
    while (Len--)
    {
        *Byte_IO ^= *Byte_I;
        Byte_IO++;
        Byte_I++;
    }
}

/**
 * @brief 秘钥扩展：由原始秘钥推导出各轮子秘钥，直接追加写回秘钥缓冲区。
 *
 * @param[in,out] KeyData_In  秘钥缓冲区：输入为原始秘钥，输出为原始秘钥加
 *                            各轮子秘钥（调用前需保证缓冲区预留足够长度）。
 * @param[in]     KeySize     秘钥长度（字节），只支持 16/24/32。
 *
 * @return None.
 */
void Key_Schedule(unsigned char *KeyData_In, unsigned char KeySize)
{
    unsigned char Datatemp[4];
    unsigned char Datatemp0[4];  /* 上一个字 W(i-1)，初值为原始秘钥最后一字 */
    unsigned char Count;
    unsigned char Round_Count;  /* 轮数：10/12/14 */

    if (KeySize == 16)  /* 16 字节秘钥对应 10 轮 */
    {
        Round_Count = 10;
    }
    else if (KeySize == 24)  /* 24 字节秘钥对应 12 轮 */
    {
        Round_Count = 12;
    }
    else if (KeySize == 32)  /* 32 字节秘钥对应 14 轮 */
    {
        Round_Count = 14;
    }
    else  /* 非法秘钥长度，直接返回 */
    {
        return;
    }

    memcpy(Datatemp0, &KeyData_In[KeySize - 4], 4);  /* 取原始秘钥最后一列 */

    /* 每轮需要一个 16 字节子秘钥，每次循环产生 4 字节；当原始秘钥大于
     * 16 字节时，超出的部分即为第一个子秘钥的数据（如 24 字节秘钥的后
     * 8 字节为第一个子秘钥的前 8 字节，32 字节秘钥的后 16 字节为第一个
     * 子秘钥数据），因此循环次数需减去原始秘钥已填充为子秘钥的长度 */
    for (Count = 0;
         Count < (Round_Count * 4 - ((KeySize - 16) / 8) * 2);
         Count++)
    {
        /* 列数为秘钥字数的整数倍时需执行 G 函数，即
         * Wi = W(i-Nk) ^ G(W(i-1))；其余列只需与对应列异或 */
        if ((Count % (KeySize / 4)) == 0)
        {
            Aes_G_function(Datatemp0, Count / (KeySize / 4));  /* 结果保存在原数组中 */
        }
        else
        {
            /* 上一轮的计算结果即为待异或数据，无需重新读取；
             * 32 字节秘钥下，当余数为 4 时需再做一次 S 盒替换 */
            if (((Count % (KeySize / 4)) == 4) && (KeySize == 32))
            {
                Aes_SubBytes(Datatemp0, 4);  /* S 盒替换 */
            }
        }
        memcpy(Datatemp, &KeyData_In[Count * 4], 4);  /* 取对应列的数据 */
        Aes_Bytes_Xor(Datatemp0, Datatemp, 4);  /* 两列数据异或 */
        memcpy(&KeyData_In[KeySize + Count * 4], Datatemp0, 4);  /* 写回扩展结果 */
    }
}

/**
 * @brief 生成轮秘钥：把原始秘钥拷贝到全局轮秘钥缓存并完成扩展。
 *
 * 128bit 秘钥扩展出 10 个子秘钥，192bit 为 12 个，256bit 为 14 个，
 * 每个子秘钥固定 16 字节。
 *
 * @param[in] KeyData      原始秘钥缓冲区。
 * @param[in] KeyByteSize  秘钥长度（字节），只支持 16/24/32。
 *
 * @return None.
 */
void Aes_Key_Schedule_Create(unsigned char *KeyData,
                             unsigned char KeyByteSize)
{
    memcpy(Round_Key, KeyData, KeyByteSize);  /* 原始秘钥拷贝到全局缓存 */
    Key_Schedule(Round_Key, KeyByteSize);  /* 扩展产生各轮子秘钥 */
}

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
                              unsigned char *key128bit)
{
    unsigned char Count;

    if (key128bit == NULL)
    {
        return;
    }
    if (IV_IN_OUT != NULL)  /* 有链接向量时，明文先与向量异或 */
    {
        Aes_Bytes_Xor(State_IN_OUT, IV_IN_OUT, 16);
    }

    Aes_Key_Schedule_Create(key128bit, 16);  /* 生成轮秘钥：共 11 组 x 16 字节 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[0], 16);  /* 第 0 轮秘钥与明文异或 */

    for (Count = 1; Count < 10; Count++)  /* 运行 9 轮，第 10 轮无列混淆，单独处理 */
    {
        Aes_SubBytes(State_IN_OUT, 16);  /* 字节替代 */
        Aes_ShiftRows(State_IN_OUT);  /* 行移位 */
        Aes_MixColumns(State_IN_OUT);  /* 列混淆 */
        Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 与轮秘钥异或 */
    }

    /* 第 10 轮：无列混淆 */
    Aes_SubBytes(State_IN_OUT, 16);  /* 字节替代 */
    Aes_ShiftRows(State_IN_OUT);  /* 行移位 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[16 * Count], 16);  /* 异或轮秘钥 */

    if (IV_IN_OUT != NULL)  /* 密文拷贝到向量缓存，供下一块链接 */
    {
        memcpy(IV_IN_OUT, State_IN_OUT, 16);
    }
}

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
                             unsigned char *key128bit)
{
    unsigned char Count;
    unsigned char Temp[16];  /* 原始密文备份 */

    if (key128bit == NULL)
    {
        return;
    }
    if (IV_IN_OUT != NULL)
    {
        memcpy(Temp, State_IN_OUT, 16);  /* 备份原始密文 */
    }

    Aes_Key_Schedule_Create(key128bit, 16);  /* 生成轮秘钥：共 11 组 x 16 字节 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[10 * 16], 16);  /* 第 10 轮秘钥与密文异或 */
    Aes_invShiftRows(State_IN_OUT);  /* 逆向行移位 */
    Aes_invSubBytes(State_IN_OUT, 16);  /* 逆向字节替代 */

    for (Count = 9; Count > 0; Count--)  /* 运行 9 轮，首尾两轮无列混淆，单独处理 */
    {
        Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 与轮秘钥异或 */
        Aes_invMixColumns(State_IN_OUT);  /* 逆向列混淆 */
        Aes_invShiftRows(State_IN_OUT);  /* 逆向行移位 */
        Aes_invSubBytes(State_IN_OUT, 16);  /* 逆向字节替代 */
    }

    /* 最后一轮：异或第 0 轮秘钥（即原始秘钥） */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[16 * Count], 16);

    if (IV_IN_OUT != NULL)
    {
        Aes_Bytes_Xor(State_IN_OUT, IV_IN_OUT, 16);  /* 解密结果与向量异或 */
        memcpy(IV_IN_OUT, Temp, 16);  /* 原始密文拷回向量缓存，供下一块链接 */
    }
}

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
                              unsigned char *key192bit)
{
    unsigned char Count;

    if (key192bit == NULL)
    {
        return;
    }
    if (IV_IN_OUT != NULL)  /* 有链接向量时，明文先与向量异或 */
    {
        Aes_Bytes_Xor(State_IN_OUT, IV_IN_OUT, 16);
    }

    Aes_Key_Schedule_Create(key192bit, 24);  /* 生成轮秘钥：共 13 组 x 16 字节 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[0], 16);  /* 第 0 轮秘钥与明文异或 */

    for (Count = 1; Count < 12; Count++)  /* 运行 11 轮，第 12 轮无列混淆，单独处理 */
    {
        Aes_SubBytes(State_IN_OUT, 16);  /* 字节替代 */
        Aes_ShiftRows(State_IN_OUT);  /* 行移位 */
        Aes_MixColumns(State_IN_OUT);  /* 列混淆 */
        Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 与轮秘钥异或 */
    }

    /* 第 12 轮：无列混淆 */
    Aes_SubBytes(State_IN_OUT, 16);  /* 字节替代 */
    Aes_ShiftRows(State_IN_OUT);  /* 行移位 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 异或轮秘钥 */

    if (IV_IN_OUT != NULL)  /* 密文拷贝到向量缓存，供下一块链接 */
    {
        memcpy(IV_IN_OUT, State_IN_OUT, 16);
    }
}

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
                             unsigned char *key192bit)
{
    unsigned char Count;
    unsigned char Temp[16];  /* 原始密文备份 */

    if (key192bit == NULL)
    {
        return;
    }
    if (IV_IN_OUT != NULL)
    {
        memcpy(Temp, State_IN_OUT, 16);  /* 备份原始密文 */
    }

    Aes_Key_Schedule_Create(key192bit, 24);  /* 生成轮秘钥：共 13 组 x 16 字节 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[12 * 16], 16);  /* 第 12 轮秘钥与密文异或 */
    Aes_invShiftRows(State_IN_OUT);  /* 逆向行移位 */
    Aes_invSubBytes(State_IN_OUT, 16);  /* 逆向字节替代 */

    for (Count = 11; Count > 0; Count--)  /* 运行 11 轮，首尾两轮无列混淆，单独处理 */
    {
        Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 与轮秘钥异或 */
        Aes_invMixColumns(State_IN_OUT);  /* 逆向列混淆 */
        Aes_invShiftRows(State_IN_OUT);  /* 逆向行移位 */
        Aes_invSubBytes(State_IN_OUT, 16);  /* 逆向字节替代 */
    }

    /* 最后一轮：异或第 0 轮秘钥（即原始秘钥） */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[16 * Count], 16);

    if (IV_IN_OUT != NULL)
    {
        Aes_Bytes_Xor(State_IN_OUT, IV_IN_OUT, 16);  /* 解密结果与向量异或 */
        memcpy(IV_IN_OUT, Temp, 16);  /* 原始密文拷回向量缓存，供下一块链接 */
    }
}

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
                              unsigned char *key256bit)
{
    unsigned char Count;

    if (key256bit == NULL)
    {
        return;
    }
    if (IV_IN_OUT != NULL)  /* 有链接向量时，明文先与向量异或 */
    {
        Aes_Bytes_Xor(State_IN_OUT, IV_IN_OUT, 16);
    }

    Aes_Key_Schedule_Create(key256bit, 32);  /* 生成轮秘钥：共 15 组 x 16 字节 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[0], 16);  /* 第 0 轮秘钥与明文异或 */

    for (Count = 1; Count < 14; Count++)  /* 运行 13 轮，第 14 轮无列混淆，单独处理 */
    {
        Aes_SubBytes(State_IN_OUT, 16);  /* 字节替代 */
        Aes_ShiftRows(State_IN_OUT);  /* 行移位 */
        Aes_MixColumns(State_IN_OUT);  /* 列混淆 */
        Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 与轮秘钥异或 */
    }

    /* 第 14 轮：无列混淆 */
    Aes_SubBytes(State_IN_OUT, 16);  /* 字节替代 */
    Aes_ShiftRows(State_IN_OUT);  /* 行移位 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 异或轮秘钥 */

    if (IV_IN_OUT != NULL)  /* 密文拷贝到向量缓存，供下一块链接 */
    {
        memcpy(IV_IN_OUT, State_IN_OUT, 16);
    }
}

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
                             unsigned char *key256bit)
{
    unsigned char Count;
    unsigned char Temp[16];  /* 原始密文备份 */

    if (key256bit == NULL)
    {
        return;
    }
    if (IV_IN_OUT != NULL)
    {
        memcpy(Temp, State_IN_OUT, 16);  /* 备份原始密文 */
    }

    Aes_Key_Schedule_Create(key256bit, 32);  /* 生成轮秘钥：共 15 组 x 16 字节 */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[14 * 16], 16);  /* 第 14 轮秘钥与密文异或 */
    Aes_invShiftRows(State_IN_OUT);  /* 逆向行移位 */
    Aes_invSubBytes(State_IN_OUT, 16);  /* 逆向字节替代 */

    for (Count = 13; Count > 0; Count--)  /* 运行 13 轮，首尾两轮无列混淆，单独处理 */
    {
        Aes_Bytes_Xor(State_IN_OUT, &Round_Key[Count * 16], 16);  /* 与轮秘钥异或 */
        Aes_invMixColumns(State_IN_OUT);  /* 逆向列混淆 */
        Aes_invShiftRows(State_IN_OUT);  /* 逆向行移位 */
        Aes_invSubBytes(State_IN_OUT, 16);  /* 逆向字节替代 */
    }

    /* 最后一轮：异或第 0 轮秘钥（即原始秘钥） */
    Aes_Bytes_Xor(State_IN_OUT, &Round_Key[16 * Count], 16);

    if (IV_IN_OUT != NULL)
    {
        Aes_Bytes_Xor(State_IN_OUT, IV_IN_OUT, 16);  /* 解密结果与向量异或 */
        memcpy(IV_IN_OUT, Temp, 16);  /* 原始密文拷回向量缓存，供下一块链接 */
    }
}
/****************************** Functions ******************************/
