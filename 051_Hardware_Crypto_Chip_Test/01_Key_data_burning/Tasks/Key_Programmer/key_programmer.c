/******************************************************************************
 * @file key_programmer.c
 *
 * @par dependencies
 * - key_programmer.h
 * - string.h
 * - stm32f4xx_hal.h
 * - elog.h
 * - bsp_keros_port.h
 * - bsp_keros_soft_i2c.h
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

/***************************** Includes *****************************/

#include "key_programmer.h" /* 本模块接口 */

#include <string.h> /* C 标准库 */

#include "stm32f4xx_hal.h" /* Core/HAL 层 */

#include "elog.h" /* 中间件层 */

#include "bsp_keros_port.h" /* 用户 BSP 层 */
#include "bsp_keros_soft_i2c.h"

/***************************** Includes *****************************/

/***************************** Defines ******************************/

/* 烧录日志标签、配置映射与 64 字节页内各数据区的长度及偏移。 */
#define KEY_PROGRAMMER_TAG            "key_programmer"
#define KEY_PROGRAMMER_PAGE           KEY_PROGRAMMER_EEPROM_PAGE
#define KEY_PROGRAMMER_PASSWORD       KEY_PROGRAMMER_EEPROM_PASSWORD
#define KEY_PROGRAMMER_IV_SIZE        16U
#define KEY_PROGRAMMER_KEY_SIZE       32U
#define KEY_PROGRAMMER_PAYLOAD_SIZE   48U
#define KEY_PROGRAMMER_CRC_OFFSET     48U
#define KEY_PROGRAMMER_CRC_INV_OFFSET 49U
#define KEY_PROGRAMMER_SERIAL_SIZE    5U

/***************************** Defines ******************************/

/*********************** Variable Definitions ***********************/

#if !KEY_PROGRAMMER_DIAGNOSTIC_MODE

/* 由配置给出的应用 AES 初始向量，16 字节。 */
static const uint8_t s_initial_vector[] = KEY_PROGRAMMER_IV_BYTES;
/* 由配置给出的应用 AES-256 密钥，32 字节。 */
static const uint8_t s_aes_key[] = KEY_PROGRAMMER_AES_KEY_BYTES;
/* 编译期检查 IV、密钥、页尺寸及页号，长度错误将使编译失败。 */
typedef char KeyProgrammer_IvSizeCheck
    [sizeof(s_initial_vector) == KEY_PROGRAMMER_IV_SIZE ? 1 : -1];
typedef char KeyProgrammer_KeySizeCheck
    [sizeof(s_aes_key) == KEY_PROGRAMMER_KEY_SIZE ? 1 : -1];
typedef char KeyProgrammer_PageSizeCheck[EEPROM_BANK_LEN == 64U ? 1 : -1];
typedef char KeyProgrammer_PageIndexCheck[KEY_PROGRAMMER_PAGE < 30U ? 1 : -1];
#endif

/* 模块初始化标志；诊断模式下仅代表 GPIO 已准备。 */
static uint8_t s_initialized;

/*********************** Variable Definitions ***********************/

/*********************** Function Definitions ***********************/

/**
 * @brief 设置总线电平、保持 500 ms 后回读，并按需输出日志。
 *
 * @param[in] expected 预期的 SCL/SDA 位掩码。
 * @param[in] phase 日志阶段名称，必须为有效字符串。
 * @param[in] log_step 非 0 时打印本次采样。
 *
 * @return 0 表示匹配，1 表示电平不符。
 */
static uint8_t KeyProgrammer_ProbeHold(uint8_t expected, const char *phase,
                                       uint8_t log_step)
{
    uint8_t actual;

    I2CBusProbeSetLines(expected);
    HAL_Delay(500U);
    actual = I2CBusProbeReadLines();
    if (log_step != 0U)
    {
        elog_i(KEY_PROGRAMMER_TAG,
               "[Probe] %s expected SCL/SDA=%u/%u actual=%u/%u", phase,
               (unsigned int)((expected & KEROS_BUS_PROBE_SCL) != 0U),
               (unsigned int)((expected & KEROS_BUS_PROBE_SDA) != 0U),
               (unsigned int)((actual & KEROS_BUS_PROBE_SCL) != 0U),
               (unsigned int)((actual & KEROS_BUS_PROBE_SDA) != 0U));
    }
    return (uint8_t)(actual != expected);
}

/**
 * @brief 持续输出 1 Hz 的 GPIO 诊断脉冲并统计电平不符次数。
 *
 * @return 无。
 *
 * @note HAL、时钟和日志初始化后调用，以此替代 Init/Run；阻塞直到复位。
 * @note 先保持 SDA 释放并翻转 SCL，再保持 SCL 低电平并翻转 SDA。
 * @note 清除模块初始化标志；不调用 SDK，不执行认证或 EEPROM 写入。
 */
void KeyProgrammer_BusProbe(void)
{
    uint8_t i;
    uint8_t scl_mismatches;
    uint8_t sda_mismatches;
    uint8_t idle_mismatches;
    uint32_t round = 0U;
    const uint8_t released = KEROS_BUS_PROBE_SCL | KEROS_BUS_PROBE_SDA;

    s_initialized = 0U;
    I2CSetInitial();
    I2CBusProbeSetLines(released);
    elog_i(KEY_PROGRAMMER_TAG,
           "[Probe] GPIO only: SCL=PB6 SDA=PB7, open drain, 1 Hz, continuous");

    while (1)
    {
        ++round;
        scl_mismatches = 0U;
        sda_mismatches = 0U;
        idle_mismatches = KeyProgrammer_ProbeHold(released, "Idle", 1U);

        /* SDA 保持释放，仅翻转 SCL，不产生 START。 */
        for (i = 0U; i < 5U; ++i)
        {
            scl_mismatches += KeyProgrammer_ProbeHold(
                KEROS_BUS_PROBE_SDA, "SCL low", (uint8_t)(i == 0U));
            scl_mismatches += KeyProgrammer_ProbeHold(released, "SCL released",
                                                      (uint8_t)(i == 0U));
        }

        /* 先拉低 SCL，再翻转 SDA，避免产生 START。 */
        I2CBusProbeSetLines(KEROS_BUS_PROBE_SDA);
        for (i = 0U; i < 5U; ++i)
        {
            sda_mismatches += KeyProgrammer_ProbeHold(
                0U, "SDA low (SCL held low)", (uint8_t)(i == 0U));
            sda_mismatches += KeyProgrammer_ProbeHold(
                KEROS_BUS_PROBE_SDA, "SDA released (SCL held low)",
                (uint8_t)(i == 0U));
        }
        idle_mismatches +=
            KeyProgrammer_ProbeHold(released, "Idle restored", 1U);
        elog_i(KEY_PROGRAMMER_TAG,
               "[Probe] Round %lu mismatches: idle=%u SCL=%u/10 SDA=%u/10",
               (unsigned long)round, (unsigned int)idle_mismatches,
               (unsigned int)scl_mismatches, (unsigned int)sda_mismatches);
        HAL_Delay(1000U);
    }
}

#if KEY_PROGRAMMER_DIAGNOSTIC_MODE
/* 诊断轮数、原始帧尺寸、固定非零种子及业务阶段标签。 */
#define KEY_DIAG_ROUNDS       3U
#define KEY_DIAG_RAW_READS    3U
#define KEY_DIAG_SERIAL_REG   0x5000U
#define KEY_DIAG_FRAME_SIZE   6U
#define KEY_DIAG_SEED         0x13579BDFU
#define KEY_DIAG_BUS_FREE_US  2000U
#define KEY_DIAG_PHASE_WAKE   1U
#define KEY_DIAG_PHASE_INIT   2U
#define KEY_DIAG_PHASE_RAW    3U
#define KEY_DIAG_PHASE_AUTH   4U
#define KEY_DIAG_PHASE_BYPASS 5U
#define KEY_DIAG_PHASE_CRYPTO 6U

/**
 * @brief 读取并打印已链接 SDK 的版本信息。
 *
 * @return 无。
 *
 * @note 须在 SDK 初始化后调用；库名称不用于判定实物芯片的 CID。
 * @note 输出后延时 40 ms，供 RTT 读取缓冲区。
 */
static void KeyDiag_PrintSdk(void)
{
    uint8_t version[128] = {0};
    uint8_t length = 0U;
    uint8_t status = get_lib_version(version, &length);

    /* SDK 版本只标识主机库；此处补齐字符串结束符。 */
    version[length < sizeof(version) ? length : sizeof(version) - 1U] = '\0';
    elog_raw(
        "[DIAG] SDK=KEROS_STM32F411CEU6_CID1018.lib (chip CID unverified)\r\n");
    elog_raw("[DIAG] SDK version rc=%u len=%u: %s\r\n", (unsigned int)status,
             (unsigned int)length, (char *)version);
    HAL_Delay(40U);
}

/* 保存一次诊断初始化的 SDK 状态、帧结果和传输证据。 */
typedef struct
{
    uint8_t power_status; /**< 唤醒命令的 I2C 状态。 */
    uint8_t init_status;  /**< SDK 初始化返回状态。 */
    uint8_t wake_first;   /**< 首次地址唤醒的应答电平。 */
    uint8_t wake_second;  /**< 第二次地址唤醒的应答电平。 */
    uint8_t
        serial[KEY_PROGRAMMER_SERIAL_SIZE]; /**< SDK 解析后的五字节序列号。 */
    uint8_t frame_ready;       /**< 初始化响应是否满足帧就绪条件。 */
    uint16_t transport_errors; /**< 本次唤醒和初始化中的失败事务数。 */
} KeyDiag_InitResult;

/**
 * @brief 将字节序列转换为以空格分隔的大写十六进制字符串。
 *
 * @param[in] data 数据缓冲区，正长度时必须有效。
 * @param[in] length 字节数，最多转换 17 字节。
 * @param[out] text 有效输出区，容量至少为 3 * min(length, 17) + 1。
 *
 * @return 无。
 *
 * @note 零长度输出 "-"，此时输出区仍需至少 2 字节。
 */
static void KeyDiag_Hex(const uint8_t *data, uint8_t length, char *text)
{
    static const char digits[] = "0123456789ABCDEF";
    uint8_t i;
    if (length > KEROS_I2C_DIAG_DATA_SIZE)
    {
        length = KEROS_I2C_DIAG_DATA_SIZE;
    }
    if (length == 0U)
    {
        text[0] = '-';
        text[1] = '\0';
        return;
    }
    for (i = 0U; i < length; ++i)
    {
        text[i * 3U] = digits[data[i] >> 4];
        text[i * 3U + 1U] = digits[data[i] & 0x0FU];
        text[i * 3U + 2U] = ' ';
    }
    text[length * 3U - 1U] = '\0';
}

/**
 * @brief 将业务诊断阶段编号转换为日志名称。
 *
 * @param[in] phase 业务阶段编号。
 *
 * @return 静态字符串；未知阶段返回 OTHER。
 */
static const char *KeyDiag_Phase(uint8_t phase)
{
    switch (phase)
    {
        case KEY_DIAG_PHASE_WAKE:
            return "WAKE";
        case KEY_DIAG_PHASE_INIT:
            return "INIT";
        case KEY_DIAG_PHASE_RAW:
            return "RAW";
        case KEY_DIAG_PHASE_AUTH:
            return "AUTH";
        case KEY_DIAG_PHASE_BYPASS:
            return "BYPASS";
        case KEY_DIAG_PHASE_CRYPTO:
            return "CRYPTO";
        default:
            return "OTHER";
    }
}

/**
 * @brief 将 I2C 事务阶段转换为日志名称。
 *
 * @param[in] stage 事务阶段编号。
 *
 * @return 静态字符串；未单独匹配的值返回 DONE。
 */
static const char *KeyDiag_Stage(uint8_t stage)
{
    switch (stage)
    {
        case KEROS_I2C_STAGE_ADDR_W:
            return "ADDR_W";
        case KEROS_I2C_STAGE_SUBADDR:
            return "SUBADDR";
        case KEROS_I2C_STAGE_ADDR_R:
            return "ADDR_R";
        case KEROS_I2C_STAGE_TX_DATA:
            return "TX_DATA";
        case KEROS_I2C_STAGE_RX_DATA:
            return "RX_DATA";
        default:
            return "DONE";
    }
}

/**
 * @brief 在通信结束后打印 RAM 中保存的事务记录。
 *
 * @return 无。
 *
 * @note 每条日志后等待 40 ms；记录溢出时同时报告丢弃数量。
 */
static void KeyDiag_DumpTrace(void)
{
    uint8_t i;
    char data[KEROS_I2C_DIAG_DATA_SIZE * 3U + 1U];
    const KerosI2C_Trace *trace;
    for (i = 0U; i < I2CDiagnostic_GetCount(); ++i)
    {
        trace = I2CDiagnostic_GetTrace(i);
        KeyDiag_Hex(trace->data, trace->data_length, data);
        elog_raw(
            "[DIAG] I2C#%u %s %c a7=%02X sub=%04X/%u n=%u rc=%02X "
            "stage=%s idx=%u attempts=%u bus=%u%u->%u%u clk_to=%u RX=%s\r\n",
            (unsigned int)(i + 1U), KeyDiag_Phase(trace->phase),
            trace->is_read ? 'R' : 'W', (unsigned int)(trace->address >> 1),
            (unsigned int)trace->sub_address, (unsigned int)trace->sub_length,
            (unsigned int)trace->requested_length, (unsigned int)trace->status,
            KeyDiag_Stage(trace->stage), (unsigned int)trace->byte_index,
            (unsigned int)trace->address_attempts,
            (unsigned int)((trace->idle_before & KEROS_BUS_PROBE_SCL) != 0U),
            (unsigned int)((trace->idle_before & KEROS_BUS_PROBE_SDA) != 0U),
            (unsigned int)((trace->idle_after & KEROS_BUS_PROBE_SCL) != 0U),
            (unsigned int)((trace->idle_after & KEROS_BUS_PROBE_SDA) != 0U),
            (unsigned int)trace->clock_timeout, data);

        HAL_Delay(40U);
    }
    if (I2CDiagnostic_GetDropped() != 0U)
    {
        elog_raw("[DIAG] Trace overflow: %u records omitted\r\n",
                 (unsigned int)I2CDiagnostic_GetDropped());
    }
}

/**
 * @brief 检查 INIT 阶段最后一次 0x5000 读取的传输状态和帧头。
 *
 * @return 1 表示满足帧就绪条件，0 表示不满足。
 *
 * @note 要求长度为 6、非全 0/全 FF、帧头低四位至少为 5。
 * @note 这里只检查传输与帧格式，不代表认证或序列号校验通过。
 */
static uint8_t KeyDiag_FrameReady(void)
{
    uint8_t i;
    uint8_t j;
    uint8_t all_zero;
    uint8_t all_ff;
    const KerosI2C_Trace *trace;
    const KerosI2C_Trace *last = NULL;
    for (i = 0U; i < I2CDiagnostic_GetCount(); ++i)
    {
        trace = I2CDiagnostic_GetTrace(i);
        if ((trace->phase == KEY_DIAG_PHASE_INIT) && (trace->is_read != 0U) &&
            (trace->sub_address == KEY_DIAG_SERIAL_REG))
        {
            last = trace;
        }
    }
    if ((last == NULL) || (last->status != 0U) ||
        (last->data_length != KEY_DIAG_FRAME_SIZE))
    {
        return 0U;
    }
    all_zero = 1U;
    all_ff = 1U;
    for (j = 0U; j < KEY_DIAG_FRAME_SIZE; ++j)
    {
        if (last->data[j] != 0U)
        {
            all_zero = 0U;
        }
        if (last->data[j] != 0xFFU)
        {
            all_ff = 0U;
        }
    }

    return (uint8_t)((all_zero == 0U) && (all_ff == 0U) &&
                     ((last->data[0] & 0x0FU) >= 5U));
}

/**
 * @brief 唤醒并初始化芯片，收集 SDK、应答及传输结果。
 *
 * @param[out] result 有效结果结构体，接收序列号和各项状态。
 *
 * @return 仅当唤醒、SDK、帧格式、错误计数及总线空闲电平均通过时返回 1。
 *
 * @note 开始时重置 RAM 记录；唤醒后按配置等待，再执行初始化。
 */
static uint8_t KeyDiag_Initialize(KeyDiag_InitResult *result)
{
    memset(result, 0, sizeof(*result));
    memset(result->serial, 0xA5, sizeof(result->serial));
    /* 重置 RAM 诊断计数，通信过程中不打印日志。 */
    I2CDiagnostic_Reset(1U);
    I2CDiagnostic_SetPhase(KEY_DIAG_PHASE_WAKE);
    result->power_status = keros_power_on();
    KerosPort_GetWakeAcks(&result->wake_first, &result->wake_second);
    keros_delay(KEY_PROGRAMMER_POWER_DELAY_MS);
    I2CDiagnostic_SetPhase(KEY_DIAG_PHASE_INIT);
    result->init_status = keros_init(result->serial);
    result->transport_errors = I2CDiagnostic_GetErrorCount();
    result->frame_ready = KeyDiag_FrameReady();
    return (uint8_t)((result->power_status == 0U) &&
                     (result->init_status == KEROS_STATUS_OK) &&
                     (result->transport_errors == 0U) &&
                     (result->frame_ready != 0U) &&
                     (I2CBusProbeReadLines() == 3U));
}

/**
 * @brief 打印唤醒状态、传输状态及 SDK 解析后的序列号。
 *
 * @param[in] result 有效初始化结果结构体。
 *
 * @return 无。
 *
 * @note 输出后等待 40 ms。
 */
static void KeyDiag_PrintInit(const KeyDiag_InitResult *result)
{
    char serial[KEROS_I2C_DIAG_DATA_SIZE * 3U + 1U];
    KeyDiag_Hex(result->serial, KEY_PROGRAMMER_SERIAL_SIZE, serial);
    elog_raw("[DIAG] Wake ACK levels=%u/%u (0=ACK 1=NACK); power_rc=%02X\r\n",
             (unsigned int)result->wake_first,
             (unsigned int)result->wake_second,
             (unsigned int)result->power_status);
    elog_raw("[DIAG] lib_init_rc=%u transport_errors=%u frame_ready=%u "
             "decoded_serial=%s\r\n",
             (unsigned int)result->init_status,
             (unsigned int)result->transport_errors,
             (unsigned int)result->frame_ready, serial);
    HAL_Delay(40U);
}

/**
 * @brief 在当前认证会话中完成单块 AES 加解密回环。
 *
 * @return 加密、解密、明文比对和 I2C 错误检查全部通过时返回 1。
 *
 * @note 输入固定为 0x30 至 0x3F；加密失败时不执行解密。
 * @note 调用前须已通过认证，全部通信完成后才打印结果。
 */
static uint8_t KeyDiag_EncryptDecrypt(void)
{
    static const uint8_t expected[MAX_AES_BUFFER_SIZE] = {
        0x30U, 0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U,
        0x38U, 0x39U, 0x3AU, 0x3BU, 0x3CU, 0x3DU, 0x3EU, 0x3FU};
    uint8_t plain[MAX_AES_BUFFER_SIZE];
    /* 单块 AES 加密结果。 */
    uint8_t encrypted[MAX_AES_BUFFER_SIZE] = {0};
    /* 单块 AES 解密结果。 */
    uint8_t decrypted[MAX_AES_BUFFER_SIZE] = {0};
    uint8_t encrypt_status;
    uint8_t decrypt_status = KEROS_STATUS_FAILED;
    uint8_t match = 0U;
    char text[KEROS_I2C_DIAG_DATA_SIZE * 3U + 1U];

    memcpy(plain, expected, sizeof(plain));
    I2CDiagnostic_SetPhase(KEY_DIAG_PHASE_CRYPTO);

    /* 沿用认证会话；加密失败时不继续解密。 */
    encrypt_status = keros_req_enc_dec(plain, encrypted, AES_REQ_ENCODING);
    if (encrypt_status == KEROS_STATUS_OK)
    {
        decrypt_status =
            keros_req_enc_dec(encrypted, decrypted, AES_REQ_DECODING);
        if (decrypt_status == KEROS_STATUS_OK)
        {
            match =
                (uint8_t)(memcmp(expected, decrypted, sizeof(expected)) == 0);
        }
    }
    elog_raw("[DIAG] CRYPTO enc_rc=%u dec_rc=%u plaintext_match=%u\r\n",
             (unsigned int)encrypt_status, (unsigned int)decrypt_status,
             (unsigned int)match);
    if (encrypt_status == KEROS_STATUS_OK)
    {
        KeyDiag_Hex(encrypted, sizeof(encrypted), text);
        elog_raw("[DIAG] CRYPTO encrypted=%s\r\n", text);
    }
    if (decrypt_status == KEROS_STATUS_OK)
    {
        KeyDiag_Hex(decrypted, sizeof(decrypted), text);
        elog_raw("[DIAG] CRYPTO decrypted=%s (expected 30..3F)\r\n", text);
    }
    return (uint8_t)((encrypt_status == KEROS_STATUS_OK) &&
                     (decrypt_status == KEROS_STATUS_OK) && (match != 0U) &&
                     (I2CDiagnostic_GetErrorCount() == 0U));
}

/**
 * @brief 建立新会话，验证认证响应，并在通过后执行 AES 回环。
 *
 * @param[out] crypto_pass 有效输出指针，接收 AES 回环结果：1 通过，0 未通过。
 *
 * @return 认证返回 TRUE、响应数据匹配且无 I2C 错误时返回 1，否则返回 0。
 *
 * @note 使用固定挑战和非零种子；认证响应按 SDK 约定异或还原后比对。
 */
static uint8_t KeyDiag_Authenticate(uint8_t *crypto_pass)
{
    KeyDiag_InitResult init;
    uint8_t input[MAX_AES_BUFFER_SIZE];
    uint8_t output[MAX_AES_BUFFER_SIZE] = {0};
    uint8_t status = FALSE;
    uint8_t match = 0U;
    uint8_t verified = 0U;
    uint8_t i;
    char text[KEROS_I2C_DIAG_DATA_SIZE * 3U + 1U];

    *crypto_pass = 0U;

    elog_raw("[DIAG] AUTH begin: fresh wake/init, fixed challenge and nonzero "
             "seed\r\n");
    if (KeyDiag_Initialize(&init) != 0U)
    {
        for (i = 0U; i < MAX_AES_BUFFER_SIZE; ++i)
        {
            input[i] = 0x30U + i;
        }
        I2CDiagnostic_SetPhase(KEY_DIAG_PHASE_AUTH);
        status = keros_authentication(SET_AES_KEY_SIZE_256, KEY_DIAG_SEED,
                                      input, output);
        if (status == TRUE)
        {
            /* 按 SDK 防跳过校验约定还原后半段，再还原前半段。 */
            for (i = MAX_AES_BUFFER_SIZE / 2U; i < MAX_AES_BUFFER_SIZE; ++i)
            {
                output[i] ^= input[MAX_AES_BUFFER_SIZE - 1U];
            }
            for (i = 0U; i < MAX_AES_BUFFER_SIZE / 2U; ++i)
            {
                output[i] ^= input[0];
            }
            match = (uint8_t)(memcmp(input, output, sizeof(input)) == 0);
        }
        elog_raw("[DIAG] AUTH returned: bool=%u (1=success) data_match=%u\r\n",
                 (unsigned int)status, (unsigned int)match);
        if (status == TRUE)
        {
            KeyDiag_Hex(output, sizeof(output), text);
            elog_raw("[DIAG] AUTH decoded test output=%s\r\n", text);
        }
        verified = (uint8_t)((status == TRUE) && (match != 0U) &&
                             (I2CDiagnostic_GetErrorCount() == 0U));
        if (verified != 0U)
        {
            *crypto_pass = KeyDiag_EncryptDecrypt();
        }
        else
        {
            elog_raw("[DIAG] CRYPTO skipped: authentication not verified\r\n");
        }
    }
    else
    {
        elog_raw("[DIAG] AUTH skipped: transport/header check failed\r\n");
    }
    KeyDiag_PrintInit(&init);
    KeyDiag_DumpTrace();
    return verified;
}

/**
 * @brief 建立新会话，验证 bypass 输出是否为输入逐字节取反。
 *
 * @return SDK 状态、逐字节结果和 I2C 错误检查全部通过时返回 1。
 *
 * @note 固定输入为 0x30 至 0x3F，预期输出为 0xCF 至 0xC0。
 */
static uint8_t KeyDiag_Bypass(void)
{
    KeyDiag_InitResult init;
    uint8_t input[MAX_AES_BUFFER_SIZE];
    uint8_t output[MAX_AES_BUFFER_SIZE] = {0};
    uint8_t status = KEROS_STATUS_FAILED;
    uint8_t match = 0U;
    uint8_t i;
    char text[KEROS_I2C_DIAG_DATA_SIZE * 3U + 1U];

    elog_raw(
        "[DIAG] BYPASS begin: fresh wake/init; expected output CF..C0\r\n");
    if (KeyDiag_Initialize(&init) != 0U)
    {
        for (i = 0U; i < MAX_AES_BUFFER_SIZE; ++i)
        {
            input[i] = 0x30U + i;
        }

        keros_srand(KEY_DIAG_SEED);
        I2CDiagnostic_SetPhase(KEY_DIAG_PHASE_BYPASS);
        status = keros_bypass_mode(input, output);
        if (status == KEROS_STATUS_OK)
        {
            match = 1U;
            for (i = 0U; i < MAX_AES_BUFFER_SIZE; ++i)
            {
                if (output[i] != (uint8_t)(input[i] ^ 0xFFU))
                {
                    match = 0U;
                }
            }
            KeyDiag_Hex(output, sizeof(output), text);
            elog_raw("[DIAG] BYPASS test output=%s\r\n", text);
        }
        elog_raw("[DIAG] BYPASS returned: rc=%u (0=success) data_match=%u\r\n",
                 (unsigned int)status, (unsigned int)match);
    }
    else
    {
        elog_raw("[DIAG] BYPASS skipped: transport/header check failed\r\n");
    }
    KeyDiag_PrintInit(&init);
    KeyDiag_DumpTrace();
    return (uint8_t)((status == KEROS_STATUS_OK) && (match != 0U) &&
                     (I2CDiagnostic_GetErrorCount() == 0U));
}

/**
 * @brief 比较三组时序下的初始化、序列号、认证及运算结果。
 *
 * @return 任一组全部通过返回 KEY_PROGRAMMER_OK，否则返回诊断失败。
 *
 * @note 比较原始时序、10 us 慢速时序和额外 2 ms 空闲延时。
 * @note 每组要求三次初始化有效、解析序列号稳定、认证及两项运算通过。
 * @note 不写 EEPROM/eFuse；结束时关闭诊断记录及额外时序配置。
 */
static KeyProgrammer_Status KeyDiag_Run(void)
{
    static const char *const profiles[] = {"ORIGINAL", "SLOW_10US",
                                           "SLOW_10US_GAP_2MS"};
    KeyDiag_InitResult init;
    uint8_t profile;
    uint8_t round;
    uint8_t sample;
    uint8_t ready;
    uint8_t valid_count;
    uint8_t stable;
    uint8_t auth;
    uint8_t bypass;
    uint8_t crypto;
    uint8_t sdk_reported = 0U;
    uint8_t any_pass = 0U;
    /* 首个有效初始化返回的序列号，作为稳定性基准。 */
    uint8_t reference[KEY_PROGRAMMER_SERIAL_SIZE];
    /* 包含帧头的原始响应，不能直接当作序列号。 */
    uint8_t raw[KEY_DIAG_FRAME_SIZE];
    const char *name;

    elog_raw("[DIAG] RUN v3 CID1018: serial/auth/bypass/AES roundtrip; NO "
             "EEPROM/eFuse programming\r\n");
    elog_raw("[DIAG] I2C rc: 00=OK 02=address NACK 08=sub/data NACK 10=SCL "
             "timeout\r\n");
    elog_raw("[DIAG] Raw 0x5000 response includes a header and may be encoded; "
             "compare decoded serials, not raw frame equality\r\n");
    HAL_Delay(1000U);
    for (profile = 0U; profile < 3U; ++profile)
    {
        name = profiles[profile];
        valid_count = 0U;
        stable = 1U;
        I2CDiagnostic_SetSlowTiming((uint8_t)(profile != 0U));
        I2CDiagnostic_SetBusFreeDelayUs(profile == 2U ? KEY_DIAG_BUS_FREE_US
                                                      : 0U);
        I2CSetInitial();
        elog_raw("[DIAG] PROFILE %s begin; idle SCL/SDA=%u/%u\r\n", name,
                 (unsigned int)((I2CBusProbeReadLines() &
                                 KEROS_BUS_PROBE_SCL) != 0U),
                 (unsigned int)((I2CBusProbeReadLines() &
                                 KEROS_BUS_PROBE_SDA) != 0U));
        if (profile == 2U)
        {
            elog_raw("[DIAG] Datasheet tBUF >= 1ms; this profile waits 2ms "
                     "after each STOP\r\n");
        }
        if (I2CBusProbeReadLines() != 3U)
        {
            elog_raw("[DIAG] PROFILE %s skipped: idle should be 1/1; "
                     "check power/pins/pull-ups\r\n",
                     name);
            continue;
        }
        for (round = 0U; round < KEY_DIAG_ROUNDS; ++round)
        {
            elog_raw("[DIAG] %s round %u/%u INIT+RAW begin\r\n", name,
                     (unsigned int)(round + 1U), (unsigned int)KEY_DIAG_ROUNDS);
            ready = KeyDiag_Initialize(&init);
            if (ready != 0U)
            {
                if (valid_count == 0U)
                {
                    memcpy(reference, init.serial, sizeof(reference));
                }
                else if (memcmp(reference, init.serial, sizeof(reference)) != 0)
                {
                    stable = 0U;
                }
                ++valid_count;
            }
            I2CDiagnostic_SetPhase(KEY_DIAG_PHASE_RAW);
            for (sample = 0U; sample < KEY_DIAG_RAW_READS; ++sample)
            {
                memset(raw, 0xA5, sizeof(raw));
                (void)keros_read_data(KEY_DIAG_SERIAL_REG, sizeof(raw), raw);
                keros_delay(2U);
            }
            KeyDiag_PrintInit(&init);
            KeyDiag_DumpTrace();

            if ((ready != 0U) && (sdk_reported == 0U))
            {
                KeyDiag_PrintSdk();
                sdk_reported = 1U;
            }
            HAL_Delay(100U);
        }
        if (valid_count != 0U)
        {
            auth = KeyDiag_Authenticate(&crypto);
            bypass = KeyDiag_Bypass();
        }
        else
        {
            auth = 0U;
            bypass = 0U;
            crypto = 0U;
            elog_raw("[DIAG] AUTH/BYPASS skipped: no valid initialization "
                     "frame\r\n");
        }
        elog_raw("[DIAG] SUMMARY %s: init_frames_ok=%u/%u serial_stable=%s "
                 "auth_pass=%u bypass_pass=%u crypto_pass=%u\r\n",
                 name, (unsigned int)valid_count, (unsigned int)KEY_DIAG_ROUNDS,
                 (valid_count < 2U) ? "UNKNOWN" : (stable ? "YES" : "NO"),
                 (unsigned int)auth, (unsigned int)bypass,
                 (unsigned int)crypto);
        if ((valid_count == KEY_DIAG_ROUNDS) && (stable != 0U) &&
            (auth != 0U) && (bypass != 0U) && (crypto != 0U))
        {
            any_pass = 1U;
        }
        HAL_Delay(200U);
    }
    I2CDiagnostic_Reset(0U);
    I2CDiagnostic_SetBusFreeDelayUs(0U);
    I2CDiagnostic_SetSlowTiming(0U);
    elog_raw("[DIAG] DONE. checks_passed_in_a_profile=%u; EEPROM/eFuse "
             "unchanged\r\n",
             (unsigned int)any_pass);
    return any_pass ? KEY_PROGRAMMER_OK : KEY_PROGRAMMER_DIAGNOSTIC_FAILED;
}
#endif

#if !KEY_PROGRAMMER_DIAGNOSTIC_MODE

/**
 * @brief 统一记录失败阶段、SDK 状态和 I2C 错误数量。
 *
 * @param[in] stage 有效失败阶段名称。
 * @param[in] result 待返回的模块错误码。
 * @param[in] sdk_status 对应 SDK 或底层操作状态。
 *
 * @return 原样返回 result。
 */
static KeyProgrammer_Status KeyProgrammer_Fail(const char *stage,
                                               KeyProgrammer_Status result,
                                               uint8_t sdk_status)
{
    elog_e(KEY_PROGRAMMER_TAG,
           "[Burn] FAILED stage=%s result=%u sdk_status=%u i2c_errors=%u",
           stage, (unsigned int)result, (unsigned int)sdk_status,
           (unsigned int)I2CDiagnostic_GetErrorCount());
    return result;
}

/**
 * @brief 检查最新 0x5000 初始化响应的传输状态和帧格式。
 *
 * @return 1 表示帧就绪，0 表示缺失或无效。
 *
 * @note 要求 6 字节、非全 0/全 FF、帧头低四位至少为 5。
 * @note 此检查补充 SDK 状态检查，不代表认证通过。
 */
static uint8_t KeyProgrammer_InitFrameReady(void)
{
    const KerosI2C_Trace *last = NULL;
    const KerosI2C_Trace *trace;
    uint8_t i;
    uint8_t all_zero = 1U;
    uint8_t all_ff = 1U;

    for (i = 0U; i < I2CDiagnostic_GetCount(); ++i)
    {
        trace = I2CDiagnostic_GetTrace(i);
        if ((trace->is_read != 0U) && (trace->sub_address == 0x5000U))
        {
            last = trace;
        }
    }
    if ((last == NULL) || (last->status != 0U) || (last->data_length != 6U))
    {
        return 0U;
    }
    for (i = 0U; i < 6U; ++i)
    {
        if (last->data[i] != 0U)
        {
            all_zero = 0U;
        }
        if (last->data[i] != 0xFFU)
        {
            all_ff = 0U;
        }
    }
    return (uint8_t)((all_zero == 0U) && (all_ff == 0U) &&
                     ((last->data[0] & 0x0FU) >= 5U));
}

/**
 * @brief 重新唤醒并初始化芯片，检查传输、响应帧及空闲电平。
 *
 * @param[out] serial_number 有效缓冲区，至少 5 字节，用于接收 SDK 解析的序列
 * 号。
 *
 * @return 成功返回 KEY_PROGRAMMER_OK；否则返回唤醒、初始化或 I2C 错误。
 *
 * @note 成功路径不插入日志，便于上层紧接着发起认证。
 */
static KeyProgrammer_Status KeyProgrammer_OpenSession(uint8_t *serial_number)
{
    uint8_t status;

    /* 重置 RAM 诊断计数，通信过程中不打印日志。 */
    I2CDiagnostic_Reset(1U);
    status = keros_power_on();
    if (status != 0U)
    {
        return KeyProgrammer_Fail("power-on", KEY_PROGRAMMER_POWER_ON_FAILED,
                                  status);
    }
    if (I2CDiagnostic_GetErrorCount() != 0U)
    {
        return KeyProgrammer_Fail("power-on-i2c", KEY_PROGRAMMER_I2C_FAILED,
                                  status);
    }

    keros_delay(KEY_PROGRAMMER_POWER_DELAY_MS);
    /* 重置 RAM 诊断计数，通信过程中不打印日志。 */
    I2CDiagnostic_Reset(1U);
    status = keros_init(serial_number);
    if (status != KEROS_STATUS_OK)
    {
        return KeyProgrammer_Fail("library-init", KEY_PROGRAMMER_INIT_FAILED,
                                  status);
    }
    if (I2CDiagnostic_GetErrorCount() != 0U)
    {
        return KeyProgrammer_Fail("init-i2c", KEY_PROGRAMMER_I2C_FAILED,
                                  status);
    }
    if ((KeyProgrammer_InitFrameReady() == 0U) ||
        (I2CBusProbeReadLines() != (KEROS_BUS_PROBE_SCL | KEROS_BUS_PROBE_SDA)))
    {
        return KeyProgrammer_Fail("init-response", KEY_PROGRAMMER_INIT_FAILED,
                                  status);
    }
    return KEY_PROGRAMMER_OK;
}

/**
 * @brief 读取并打印当前链接的 CID1018 SDK 版本。
 *
 * @return 无。
 *
 * @note 在 SDK 初始化成功后调用；打印前保证版本字符串终止。
 */
static void KeyProgrammer_LogSdk(void)
{
    uint8_t version[128] = {0};
    uint8_t length = 0U;
    uint8_t status = get_lib_version(version, &length);

    version[length < sizeof(version) ? length : sizeof(version) - 1U] = '\0';
    elog_i(KEY_PROGRAMMER_TAG, "[Init] SDK=CID1018 version_status=%u: %s",
           (unsigned int)status, (char *)version);
}

/**
 * @brief 计算应用 IV 和 AES 密钥数据的 CRC-8。
 *
 * @param[in] data 数据缓冲区，正长度时必须有效。
 * @param[in] size 待计算字节数。
 *
 * @return CRC-8 结果。
 *
 * @note 多项式 0x07，初值 0，不反射，无最终异或。
 */
static uint8_t KeyProgrammer_Crc8(const uint8_t *data, uint8_t size)
{
    uint8_t crc = 0U;
    uint8_t i;
    uint8_t bit;

    for (i = 0U; i < size; ++i)
    {
        crc ^= data[i];
        for (bit = 0U; bit < 8U; ++bit)
        {
            if ((crc & 0x80U) != 0U)
            {
                crc = (uint8_t)((crc << 1) ^ 0x07U);
            }
            else
            {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

/**
 * @brief 生成挑战并执行芯片认证与响应数据校验。
 *
 * @return 通过返回 KEY_PROGRAMMER_OK；否则返回认证、数据或 I2C 错误。
 *
 * @note 沿用 SDK 的非零伪随机种子和防跳过异或比对流程。
 * @note 挑战数据不是应用 AES 密钥；认证成功的 SDK 返回值为 TRUE。
 */
static KeyProgrammer_Status KeyProgrammer_Authenticate(void)
{
    uint8_t input[MAX_AES_BUFFER_SIZE];
    uint8_t output[MAX_AES_BUFFER_SIZE] = {0};
    uint8_t status;
    uint8_t i;
    /* SDK 挑战用伪随机种子，随后排除零值。 */
    uint32_t seed = HAL_GetTick() ^ 0x13579BDFU;

    /* 排除零种子；此伪随机数只用于 SDK 挑战，不生成应用密钥。 */
    if (seed == 0U)
    {
        seed = 0x13579BDFU;
    }
    keros_srand(seed);
    for (i = 0U; i < MAX_AES_BUFFER_SIZE; ++i)
    {
        input[i] = keros_random();
    }

    /* 重置 RAM 诊断计数，通信过程中不打印日志。 */
    I2CDiagnostic_Reset(1U);
    status = keros_authentication(SET_AES_KEY_SIZE_256, seed, input, output);

    /* 认证成功返回 TRUE，区别于初始化和 EEPROM 的状态 0。 */
    if (status != TRUE)
    {
        return KeyProgrammer_Fail("authentication", KEY_PROGRAMMER_AUTH_FAILED,
                                  status);
    }
    if (I2CDiagnostic_GetErrorCount() != 0U)
    {
        return KeyProgrammer_Fail("authentication-i2c",
                                  KEY_PROGRAMMER_I2C_FAILED, status);
    }

    /* 按 SDK 防跳过校验约定还原后半段，再还原前半段。 */
    for (i = MAX_AES_BUFFER_SIZE / 2U; i < MAX_AES_BUFFER_SIZE; ++i)
    {
        output[i] ^= input[MAX_AES_BUFFER_SIZE - 1U];
    }
    for (i = 0U; i < MAX_AES_BUFFER_SIZE / 2U; ++i)
    {
        output[i] ^= input[0];
    }
    if (memcmp(input, output, sizeof(input)) != 0)
    {
        return KeyProgrammer_Fail("authentication-data",
                                  KEY_PROGRAMMER_AUTH_DATA_MISMATCH, status);
    }

    elog_i(KEY_PROGRAMMER_TAG,
           "[Burn] Authentication and data verification PASSED");
    return KEY_PROGRAMMER_OK;
}
#endif

/**
 * @brief 初始化软件 I2C 和芯片，并打印 SDK 解析的五字节序列号。
 *
 * @return 成功返回 KEY_PROGRAMMER_OK；失败返回唤醒、初始化或 I2C 错误。
 *
 * @note 在 HAL、时钟及日志初始化后调用；此步骤不执行认证或写入。
 * @note 启用诊断模式时仅准备 GPIO，实际芯片检查由 Run 执行。
 */
KeyProgrammer_Status KeyProgrammer_Init(void)
{
#if KEY_PROGRAMMER_DIAGNOSTIC_MODE
    I2CDiagnostic_Reset(0U);
    I2CDiagnostic_SetBusFreeDelayUs(0U);
    I2CDiagnostic_SetSlowTiming(0U);
    I2CSetInitial();
    s_initialized = 1U;
    elog_raw(
        "[DIAG] INIT v3 CID1018 ready: SCL=PB6 SDA=PB7 a7=1C HCLK=%lu Hz\r\n",
        (unsigned long)HAL_RCC_GetHCLKFreq());
    return KEY_PROGRAMMER_OK;
#else
    /* SDK 解析出的五字节序列号。 */
    uint8_t serial_number[KEY_PROGRAMMER_SERIAL_SIZE] = {0};
    KeyProgrammer_Status result;

    s_initialized = 0U;
    elog_i(KEY_PROGRAMMER_TAG, "[Init] AES key/IV EEPROM programmer (CID1018)");
    I2CDiagnostic_SetBusFreeDelayUs(0U);
    I2CDiagnostic_SetSlowTiming(0U);
    I2CSetInitial();
    /* 重新唤醒并检查初始化响应。 */
    result = KeyProgrammer_OpenSession(serial_number);
    if (result != KEY_PROGRAMMER_OK)
    {
        return result;
    }

    s_initialized = 1U;
    elog_i(KEY_PROGRAMMER_TAG, "[Init] PASSED Serial: %02X %02X %02X %02X %02X",
           (unsigned int)serial_number[0], (unsigned int)serial_number[1],
           (unsigned int)serial_number[2], (unsigned int)serial_number[3],
           (unsigned int)serial_number[4]);
    KeyProgrammer_LogSdk();
    elog_i(KEY_PROGRAMMER_TAG,
           "[Init] EEPROM-equipped model required; CK02AT has no user EEPROM");
    return KEY_PROGRAMMER_OK;
#endif
}

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
KeyProgrammer_Status KeyProgrammer_Run(void)
{
#if KEY_PROGRAMMER_DIAGNOSTIC_MODE
    if (s_initialized == 0U)
    {
        return KEY_PROGRAMMER_NOT_INITIALIZED;
    }
    return KeyDiag_Run();
#else

    /* 待写入的完整 64 字节页，未使用部分保持为 0。 */
    uint8_t write_buffer[EEPROM_BANK_LEN] = {0};
    /* 读回页数据，用于 CRC 和逐字节比较。 */
    uint8_t read_buffer[EEPROM_BANK_LEN] = {0};
    /* SDK 解析出的五字节序列号。 */
    uint8_t serial_number[KEY_PROGRAMMER_SERIAL_SIZE] = {0};
    uint8_t status;
    uint8_t crc;
    /* 读回 CRC 的按位反码，用于交叉校验。 */
    uint8_t crc_inverse;
    KeyProgrammer_Status result;

    if (s_initialized == 0U)
    {
        elog_e(KEY_PROGRAMMER_TAG, "[Burn] Call KeyProgrammer_Init first");
        return KEY_PROGRAMMER_NOT_INITIALIZED;
    }

    /* 组装 IV、密钥和 CRC；剩余 14 字节沿用清零值。 */
    memcpy(write_buffer, s_initial_vector, sizeof(s_initial_vector));
    memcpy(write_buffer + KEY_PROGRAMMER_IV_SIZE, s_aes_key, sizeof(s_aes_key));
    crc = KeyProgrammer_Crc8(write_buffer, KEY_PROGRAMMER_PAYLOAD_SIZE);
    write_buffer[KEY_PROGRAMMER_CRC_OFFSET] = crc;
    write_buffer[KEY_PROGRAMMER_CRC_INV_OFFSET] = (uint8_t)~crc;

    elog_i(KEY_PROGRAMMER_TAG,
           "[Burn] BEGIN page=%u IV=16 key=32 CRC8=%02X inverse=%02X total=64",
           (unsigned int)KEY_PROGRAMMER_PAGE, (unsigned int)crc,
           (unsigned int)write_buffer[KEY_PROGRAMMER_CRC_INV_OFFSET]);
    elog_i(KEY_PROGRAMMER_TAG, "[Burn] Authenticating...");

    result = KeyProgrammer_OpenSession(serial_number);
    if (result != KEY_PROGRAMMER_OK)
    {
        return result;
    }
    /* 新会话建立后立即认证，失败则返回并禁止写入。 */
    result = KeyProgrammer_Authenticate();
    if (result != KEY_PROGRAMMER_OK)
    {
        return result;
    }

    elog_i(KEY_PROGRAMMER_TAG, "[Burn] Writing EEPROM page %u...",
           (unsigned int)KEY_PROGRAMMER_PAGE);
    /* 重置 RAM 诊断计数，通信过程中不打印日志。 */
    I2CDiagnostic_Reset(1U);
    /* 认证及响应校验通过后，才允许写入完整 EEPROM 页。 */
    status = keros_eeprom_write(KEY_PROGRAMMER_PASSWORD, KEY_PROGRAMMER_PAGE,
                                write_buffer, REQ_AES_NONE_WRITE);
    if (status != KEROS_STATUS_OK)
    {
        return KeyProgrammer_Fail("eeprom-write", KEY_PROGRAMMER_WRITE_FAILED,
                                  status);
    }
    if (I2CDiagnostic_GetErrorCount() != 0U)
    {
        return KeyProgrammer_Fail("eeprom-write-i2c", KEY_PROGRAMMER_I2C_FAILED,
                                  status);
    }

    keros_delay(KEY_PROGRAMMER_WRITE_DELAY_MS);
    elog_i(KEY_PROGRAMMER_TAG, "[Burn] Write PASSED; reading back...");
    /* 重置 RAM 诊断计数，通信过程中不打印日志。 */
    I2CDiagnostic_Reset(1U);
    /* 等待写入完成后，读取同一页用于验证。 */
    status = keros_eeprom_read(KEY_PROGRAMMER_PASSWORD, KEY_PROGRAMMER_PAGE,
                               read_buffer, REQ_AES_NONE_READ);
    if (status != KEROS_STATUS_OK)
    {
        return KeyProgrammer_Fail("eeprom-read", KEY_PROGRAMMER_READ_FAILED,
                                  status);
    }
    if (I2CDiagnostic_GetErrorCount() != 0U)
    {
        return KeyProgrammer_Fail("eeprom-read-i2c", KEY_PROGRAMMER_I2C_FAILED,
                                  status);
    }

    /* 先检查 CRC 及反码，再比较包括保留区在内的整页内容。 */
    crc = KeyProgrammer_Crc8(read_buffer, KEY_PROGRAMMER_PAYLOAD_SIZE);
    crc_inverse = (uint8_t)~read_buffer[KEY_PROGRAMMER_CRC_OFFSET];
    if ((crc != read_buffer[KEY_PROGRAMMER_CRC_OFFSET]) ||
        (crc_inverse != read_buffer[KEY_PROGRAMMER_CRC_INV_OFFSET]))
    {
        return KeyProgrammer_Fail("readback-crc", KEY_PROGRAMMER_CRC_MISMATCH,
                                  status);
    }

    if (memcmp(write_buffer, read_buffer, sizeof(write_buffer)) != 0)
    {
        return KeyProgrammer_Fail("readback-data", KEY_PROGRAMMER_DATA_MISMATCH,
                                  status);
    }

    elog_i(KEY_PROGRAMMER_TAG,
           "[Burn] Readback CRC and 64-byte comparison PASSED");
    elog_i(KEY_PROGRAMMER_TAG,
           "[Burn] SUCCESS page=%u application AES key/IV verified",
           (unsigned int)KEY_PROGRAMMER_PAGE);
    return KEY_PROGRAMMER_OK;
#endif
}

/*********************** Function Definitions ***********************/
