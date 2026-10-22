/******************************************************************************
 * @file keros_demo.c
 * @par dependencies
 * - keros_demo.h
 * - stddef.h
 * - string.h
 * - elog.h
 * - keros_lib.h
 * - keros_interface.h
 * - keros_i2c_interface.h
 * @brief Tests EEPROM user pages and distinguishes SDK range limits.
 * Processing flow: init, prerequisites, write all if selected, read all.
 * @version V1.0 2026-10-13
 * @note 1 tab == 4 spaces! No RTOS, display, sensors or bootloader dependency.
 ******************************************************************************/

/****************************** Includes ******************************/
#include "keros_demo.h"          /* 应用层：综合测试接口 */

#include <stddef.h>             /* C 标准库 */
#include <string.h>

#include "elog.h"               /* 中间件层：已有 EasyLogger */

#include "keros_lib.h"          /* BSP 层：CID1018 厂家库和平台接口 */
#include "keros_interface.h"
#include "keros_i2c_interface.h"
/****************************** Includes ******************************/

/****************************** Defines ******************************/
#define KEROS_DEMO_TAG             "keros"
#define KEROS_DEMO_PAGE_SIZE       KEROS_DEMO_EEPROM_PAGE_SIZE
#define KEROS_DEMO_PASSWORD        0U
#define KEROS_DEMO_POWER_DELAY_MS  2U
#define KEROS_DEMO_WRITE_DELAY_MS  5U
#define KEROS_DEMO_STATUS_UNUSED   0xFFU
#define KEROS_DEMO_LOG_ROW_SIZE    16U
#define KEROS_DEMO_LOG_DELAY_MS    10U

/* UTF-8 字节常量兼容 ARMCC5：未识别到 EEPROM 芯片。 */
#define KEROS_DEMO_EEPROM_MESSAGE \
    "\xE6\x9C\xAA\xE8\xAF\x86\xE5\x88\xAB\xE5\x88\xB0" \
    " EEPROM " "\xE8\x8A\xAF\xE7\x89\x87"
/****************************** Defines ******************************/

/************************ Variable Definitions ************************/
static uint8_t g_initialized = 0U;
static keros_demo_status_t g_init_status = KEROS_DEMO_NOT_INITIALIZED;
static uint8_t g_power_on_status = KEROS_DEMO_STATUS_UNUSED;
static uint8_t g_lib_init_status = KEROS_DEMO_STATUS_UNUSED;
static uint8_t g_serial_number[KEROS_DEMO_SERIAL_SIZE] = {0U};
static keros_port_error_t g_init_error = {0U, 0U, 0U};

/* 逐页报告留在模块静态区，避免扩大 main 的局部报告栈占用。 */
static keros_demo_page_result_t
    g_page_results[KEROS_DEMO_EEPROM_PROBE_PAGES];

static const char * const g_case_names[KEROS_DEMO_CASE_COUNT] =
{
    "BYPASS", "Authentication AES-256", "AES roundtrip",
    "EEPROM requested pages 0..15", "EEPROM requested pages 16..31",
    "EEPROM requested pages 32..35 (beyond 2 KB)"
};
/************************ Variable Definitions ************************/

/************************ Function Definitions ************************/
/**
 * @brief 填充原工程的递增字节模式。
 * @param[out] p_data 有效的测试缓冲区。
 * @param[in] size 缓冲区字节数。
 * @param[in] pattern 起始字节。
 */
static void KerosDemo_Fill(uint8_t *p_data, uint8_t size, uint8_t pattern)
{
    uint8_t index = 0U;

    if (NULL == p_data)
    {
        return;
    }
    for (index = 0U; index < size; index++)
    {
        p_data[index] = (uint8_t)(pattern + index);
    }
}

/**
 * @brief 按每行 16 字节输出完整页数据，便于比较两次运行日志。
 * @param[in] page 页索引。
 * @param[in] p_label 数据标签，区分期望值和实际读回值。
 * @param[in] p_data 64 字节明文页数据。
 * @note 每行间隔 10 ms，给 RTT 主机留出读取大量日志的时间。
 */
static void KerosDemo_LogPage(uint8_t page, const char *p_label,
                              const uint8_t *p_data)
{
    static const char hex_digits[] = "0123456789ABCDEF";
    char row[KEROS_DEMO_LOG_ROW_SIZE * 3U] = {0};
    uint8_t offset = 0U;
    uint8_t index = 0U;
    uint8_t value = 0U;

    if ((NULL == p_label) || (NULL == p_data))
    {
        return;
    }
    for (offset = 0U; offset < KEROS_DEMO_PAGE_SIZE;
         offset += KEROS_DEMO_LOG_ROW_SIZE)
    {
        for (index = 0U; index < KEROS_DEMO_LOG_ROW_SIZE; index++)
        {
            value = p_data[offset + index];
            row[index * 3U] = hex_digits[value >> 4U];
            row[index * 3U + 1U] = hex_digits[value & 0x0FU];
            row[index * 3U + 2U] = ' ';
        }
        row[sizeof(row) - 1U] = '\0';
        elog_i(KEROS_DEMO_TAG, "EEPROM page %u %s [%02X]: %s",
               (unsigned int)page, p_label, (unsigned int)offset, row);
        keros_delay(KEROS_DEMO_LOG_DELAY_MS);
    }
}

/**
 * @brief 比较全部字节，记录首个不匹配位置。
 * @param[in] p_expected 期望数据。
 * @param[in] p_actual 实际数据。
 * @param[in] size 比较长度。
 * @param[in,out] p_result 当前测试项结果。
 * @return 1 表示相同，0 表示参数无效或数据不匹配。
 */
static uint8_t KerosDemo_Compare(const uint8_t *p_expected,
                                const uint8_t *p_actual, uint8_t size,
                                keros_demo_item_result_t *p_result)
{
    uint8_t index = 0U;

    if ((NULL == p_expected) || (NULL == p_actual) || (NULL == p_result))
    {
        return 0U;
    }
    p_result->data_match = 0U;
    for (index = 0U; index < size; index++)
    {
        if (p_expected[index] != p_actual[index])
        {
            p_result->outcome = KEROS_DEMO_FAILED;
            p_result->reason = KEROS_DEMO_REASON_DATA_MISMATCH;
            p_result->mismatch_index = index;
            elog_e(KEROS_DEMO_TAG,
                   "Data mismatch [%u]: expected=%02X actual=%02X",
                   (unsigned int)index, (unsigned int)p_expected[index],
                   (unsigned int)p_actual[index]);
            return 0U;
        }
    }
    p_result->data_match = 1U;
    return 1U;
}

/**
 * @brief 检查厂家库调用中的首次底层失败，避免返回 0 掩盖错误。
 * @param[in,out] p_result 当前测试项。
 * @return 1 表示底层无错误，0 表示平台/I2C 失败。
 */
static uint8_t KerosDemo_CheckTransport(keros_demo_item_result_t *p_result)
{
    keros_port_error_t error = KerosPort_GetError();
    keros_port_activity_t activity = KerosPort_GetActivity();

    if (NULL == p_result)
    {
        return 0U;
    }
    p_result->transport_status = error.status;
    p_result->transport_address = error.sub_address;
    p_result->transport_is_read = error.is_read;
    p_result->io_read_calls = activity.read_calls;
    p_result->io_write_calls = activity.write_calls;
    if (0U != error.status)
    {
        p_result->outcome = KEROS_DEMO_FAILED;
        p_result->reason = KEROS_DEMO_REASON_TRANSPORT;
        p_result->data_match = 0U;
        return 0U;
    }
    return 1U;
}

/**
 * @brief 同时检查普通库返回值和底层错误；认证布尔值单独处理。
 * @param[in,out] p_result 当前测试项。
 * @param[in] stage 当前调用阶段。
 * @param[in] status 原始库状态。
 * @return 库返回 OK 且底层无错误时为 1，其余情况为 0。
 */
static uint8_t KerosDemo_CheckStatus(keros_demo_item_result_t *p_result,
                                    keros_demo_stage_t stage,
                                    uint8_t status)
{
    if (NULL == p_result)
    {
        return 0U;
    }
    p_result->stage = stage;
    p_result->raw_status = status;
    p_result->raw_status_valid = 1U;
    if (0U == KerosDemo_CheckTransport(p_result))
    {
        return 0U;
    }
    if (KEROS_STATUS_OK != status)
    {
        p_result->outcome = KEROS_DEMO_FAILED;
        p_result->reason = KEROS_DEMO_REASON_LIBRARY_STATUS;
        p_result->data_match = 0U;
        return 0U;
    }
    return 1U;
}

/**
 * @brief 校验 BYPASS 的 16 字节反码输出。
 * @param[out] p_result 当前测试项结果。
 */
static void KerosDemo_Bypass(keros_demo_item_result_t *p_result)
{
    uint8_t request[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t response[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t expected[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t index = 0U;
    uint8_t status = KEROS_STATUS_FAILED;

    if (NULL == p_result)
    {
        return;
    }
    KerosDemo_Fill(request, MAX_AES_BUFFER_SIZE, 0x30U);
    for (index = 0U; index < MAX_AES_BUFFER_SIZE; index++)
    {
        expected[index] = request[index] ^ 0xFFU;
    }
    KerosPort_ClearError();
    status = keros_bypass_mode(request, response);
    if (0U == KerosDemo_CheckStatus(p_result, KEROS_DEMO_STAGE_BYPASS,
                                    status))
    {
        return;
    }
    if (0U != KerosDemo_Compare(expected, response, MAX_AES_BUFFER_SIZE,
                                p_result))
    {
        p_result->outcome = KEROS_DEMO_PASSED;
        p_result->reason = KEROS_DEMO_REASON_NONE;
        p_result->completed_cycles = 1U;
    }
}

/**
 * @brief 执行 AES-256 认证和原工程的防跳过验证。
 * @param[out] p_result 当前测试项结果。
 */
static void KerosDemo_Authenticate(keros_demo_item_result_t *p_result)
{
    uint8_t request[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t response[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t expected[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t index = 0U;

    if (NULL == p_result)
    {
        return;
    }
    for (index = 0U; index < MAX_AES_BUFFER_SIZE; index++)
    {
        request[index] = keros_random();
    }
    memcpy(expected, request, sizeof(expected));
    p_result->stage = KEROS_DEMO_STAGE_AUTHENTICATION;
    KerosPort_ClearError();
    p_result->raw_status = keros_authentication(SET_AES_KEY_SIZE_256, 0U,
                                                request, response);
    p_result->raw_status_valid = 1U;
    if (0U == KerosDemo_CheckTransport(p_result))
    {
        return;
    }

    /* 认证成功返回 TRUE=1，其他普通库接口成功返回 0。 */
    if (TRUE != p_result->raw_status)
    {
        p_result->outcome = KEROS_DEMO_FAILED;
        p_result->reason = KEROS_DEMO_REASON_AUTH_RETURN;
        return;
    }
    for (index = 0U; index < MAX_AES_BUFFER_SIZE; index++)
    {
        response[index] ^= (index < MAX_AES_BUFFER_SIZE / 2U) ?
                            expected[0] : expected[MAX_AES_BUFFER_SIZE - 1U];
    }
    if (0U != KerosDemo_Compare(expected, response, MAX_AES_BUFFER_SIZE,
                                p_result))
    {
        p_result->outcome = KEROS_DEMO_PASSED;
        p_result->reason = KEROS_DEMO_REASON_NONE;
        p_result->completed_cycles = 1U;
    }
}

/**
 * @brief 加密后解密，使用独立的原始明文完成自动回环比较。
 * @param[out] p_result 当前测试项结果。
 */
static void KerosDemo_AesRoundtrip(keros_demo_item_result_t *p_result)
{
    uint8_t expected[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t request[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t encrypted[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t decrypted[MAX_AES_BUFFER_SIZE] = {0U};
    uint8_t status = KEROS_STATUS_FAILED;

    if (NULL == p_result)
    {
        return;
    }
    KerosDemo_Fill(expected, MAX_AES_BUFFER_SIZE, 0x30U);
    memcpy(request, expected, sizeof(request));
    KerosPort_ClearError();
    status = keros_req_enc_dec(request, encrypted, AES_REQ_ENCODING);
    if (0U == KerosDemo_CheckStatus(p_result,
                                    KEROS_DEMO_STAGE_ENCRYPTION, status))
    {
        return;
    }
    KerosPort_ClearError();
    status = keros_req_enc_dec(encrypted, decrypted, AES_REQ_DECODING);
    if (0U == KerosDemo_CheckStatus(p_result,
                                    KEROS_DEMO_STAGE_DECRYPTION, status))
    {
        return;
    }
    if (0U != KerosDemo_Compare(expected, decrypted, MAX_AES_BUFFER_SIZE,
                                p_result))
    {
        p_result->outcome = KEROS_DEMO_PASSED;
        p_result->reason = KEROS_DEMO_REASON_NONE;
        p_result->completed_cycles = 1U;
    }
}

/**
 * @brief 按页生成独立的 v2 数据，跨烧录重现，不依赖前次 RAM。
 * @param[out] p_data 64 字节有效缓冲区。
 * @param[in] page 库页号，范围 0~35。
 * @note 前五字节包含标记、版本、页号及其反码，用于识别地址重复。
 */
static void KerosDemo_PagePattern(uint8_t *p_data, uint8_t page)
{
    uint8_t index = 0U;

    if (NULL == p_data)
    {
        return;
    }
    for (index = 0U; index < KEROS_DEMO_PAGE_SIZE; index++)
    {
        p_data[index] = (uint8_t)(0x5AU + page * 37U + index * 13U);
    }
    p_data[0] = 0x4BU;
    p_data[1] = 0x52U;
    p_data[2] = 0x02U;
    p_data[3] = page;
    p_data[4] = (uint8_t)(page ^ 0xFFU);
}

/**
 * @brief 初始化逐页诊断，保留未执行与失败的区别。
 * @param[out] p_result 有效的单次访问结果。
 */
static void KerosDemo_ResetResult(keros_demo_item_result_t *p_result)
{
    if (NULL == p_result)
    {
        return;
    }
    memset(p_result, 0, sizeof(*p_result));
    p_result->mismatch_index = -1;
    p_result->reason = KEROS_DEMO_REASON_PREREQUISITE;
}

/**
 * @brief 区分实际访问失败、库边界拒绝及没有传输的成功返回。
 * @param[in,out] p_result 本次库调用结果。
 * @param[in] page 请求页号。
 * @param[in] stage 写入或读取阶段。
 * @param[in] status 原始库返回值。
 * @return 1 为库成功且发生底层传输，其余情况为 0。
 * @note 30 起且原始状态为 4/5/18、底层调用为零时才标记 SDK_LIMIT。
 */
static uint8_t KerosDemo_CheckPage(keros_demo_item_result_t *p_result,
                                  uint8_t page, keros_demo_stage_t stage,
                                  uint8_t status)
{
    uint8_t valid = KerosDemo_CheckStatus(p_result, stage, status);

    if (NULL == p_result)
    {
        return 0U;
    }
    if ((0U == p_result->io_read_calls) &&
        (0U == p_result->io_write_calls) &&
        (0U == p_result->transport_status))
    {
        if ((page >= KEROS_DEMO_EEPROM_USER_PAGES) &&
            ((KEROS_NOT_SUPPORT_PAGE_INDEX == status) ||
             (KEROS_NOT_SUPPORT_BLOCK_INDEX == status) ||
             (KEROS_FAILED_PASSWORD == status)))
        {
            p_result->outcome = KEROS_DEMO_SDK_LIMIT;
            p_result->reason = KEROS_DEMO_REASON_SDK_LIMIT;
            return 0U;
        }
        if (0U != valid)
        {
            p_result->outcome = KEROS_DEMO_FAILED;
            p_result->reason = KEROS_DEMO_REASON_NO_TRANSFER;
            p_result->data_match = 0U;
            return 0U;
        }
    }
    return valid;
}

/**
 * @brief 输出每页访问结果和实际 I2C 次数，库限制不作硬件失败。
 * @param[in] page 请求页号。
 * @param[in] p_label WRITE 或 READ。
 * @param[in] p_result 本次访问结果。
 */
static void KerosDemo_LogAccess(uint8_t page, const char *p_label,
                               const keros_demo_item_result_t *p_result)
{
    const char *p_outcome = "FAIL";

    if ((NULL == p_label) || (NULL == p_result))
    {
        return;
    }
    if (KEROS_DEMO_PASSED == p_result->outcome)
    {
        p_outcome = "COMMAND_OK";
    }
    else if (KEROS_DEMO_UNVERIFIED == p_result->outcome)
    {
        p_outcome = "MATCH_UNVERIFIED";
    }
    else if (KEROS_DEMO_SDK_LIMIT == p_result->outcome)
    {
        p_outcome = "SDK_LIMIT (not a hardware boundary)";
    }
    elog_i(KEROS_DEMO_TAG,
           "EEPROM page %u %s: %s raw=%u reason=%u io_read=%lu io_write=%lu",
           (unsigned int)page, p_label, p_outcome,
           (unsigned int)p_result->raw_status,
           (unsigned int)p_result->reason,
           (unsigned long)p_result->io_read_calls,
           (unsigned long)p_result->io_write_calls);
    if (KEROS_DEMO_FAILED == p_result->outcome)
    {
        elog_e(KEROS_DEMO_TAG,
               "EEPROM page %u: " KEROS_DEMO_EEPROM_MESSAGE
               " / EEPROM access or data verification failed",
               (unsigned int)page);
        if (0U != p_result->transport_status)
        {
            elog_e(KEROS_DEMO_TAG,
                   "EEPROM page %u: transport=%u sub=%04X direction=%s",
                   (unsigned int)page,
                   (unsigned int)p_result->transport_status,
                   (unsigned int)p_result->transport_address,
                   (0U != p_result->transport_is_read) ? "read" : "write");
        }
    }
}

/**
 * @brief 检查读回数据是否完整匹配另一个请求页的模式。
 * @param[in] p_data 实际读回的完整页。
 * @return 匹配的模式页号；无法匹配时为 -1，仅作地址重复线索。
 */
static int16_t KerosDemo_FindPattern(const uint8_t *p_data)
{
    uint8_t candidate[KEROS_DEMO_PAGE_SIZE] = {0U};
    uint8_t page = 0U;

    if (NULL == p_data)
    {
        return -1;
    }
    for (page = 0U; page < KEROS_DEMO_EEPROM_PROBE_PAGES; page++)
    {
        KerosDemo_PagePattern(candidate, page);
        if (0 == memcmp(candidate, p_data, sizeof(candidate)))
        {
            return (int16_t)page;
        }
    }
    return -1;
}

/**
 * @brief 先尝试写入全部请求页；一个页失败不阻止后续页。
 * @note 覆盖可写用户页，包括页 0；库范围之外不绕过库访问寄存器。
 */
static void KerosDemo_WritePages(void)
{
    uint8_t data[KEROS_DEMO_PAGE_SIZE] = {0U};
    uint8_t page = 0U;
    uint8_t status = KEROS_STATUS_FAILED;
    keros_demo_item_result_t *p_result = NULL;

    elog_i(KEROS_DEMO_TAG, "EEPROM phase=WRITE_ALL pages=0..35");
    for (page = 0U; page < KEROS_DEMO_EEPROM_PROBE_PAGES; page++)
    {
        p_result = &g_page_results[page].write;
        KerosDemo_PagePattern(data, page);
        KerosPort_ClearError();
        status = keros_eeprom_write(KEROS_DEMO_PASSWORD, page, data,
                                    REQ_AES_NONE_WRITE);
        if (0U != KerosDemo_CheckPage(p_result, page,
                                      KEROS_DEMO_STAGE_EEPROM_WRITE, status))
        {
            p_result->outcome = KEROS_DEMO_PASSED;
            p_result->reason = KEROS_DEMO_REASON_NONE;
            p_result->completed_cycles = 1U;
            keros_delay(KEROS_DEMO_WRITE_DELAY_MS);
        }
        KerosDemo_LogAccess(page, "WRITE", p_result);
    }
}

/**
 * @brief 统一读取全部请求页，保留完整数据并核对独立页模式。
 * @note 不调用 EEPROM 写入，写读模式的最终读取也复用此流程。
 */
static void KerosDemo_ReadPages(void)
{
    uint8_t expected[KEROS_DEMO_PAGE_SIZE] = {0U};
    uint8_t actual[KEROS_DEMO_PAGE_SIZE] = {0U};
    uint8_t page = 0U;
    uint8_t status = KEROS_STATUS_FAILED;
    keros_demo_page_result_t *p_page = NULL;
    keros_demo_item_result_t *p_result = NULL;

    elog_i(KEROS_DEMO_TAG, "EEPROM phase=READ_ALL pages=0..35");
    for (page = 0U; page < KEROS_DEMO_EEPROM_PROBE_PAGES; page++)
    {
        p_page = &g_page_results[page];
        p_result = &p_page->read;
        KerosDemo_PagePattern(expected, page);
        memset(actual, 0, sizeof(actual));
        elog_i(KEROS_DEMO_TAG, "EEPROM page %u offset=%04X length=64",
               (unsigned int)page,
               (unsigned int)(page * KEROS_DEMO_PAGE_SIZE));
        KerosDemo_LogPage(page, "EXPECTED", expected);
        KerosPort_ClearError();
        status = keros_eeprom_read(KEROS_DEMO_PASSWORD, page, actual,
                                   REQ_AES_NONE_READ);
        if (0U != KerosDemo_CheckPage(p_result, page,
                                      KEROS_DEMO_STAGE_EEPROM_READ, status))
        {
            KerosDemo_LogPage(page, "READ", actual);
            p_page->observed_page = KerosDemo_FindPattern(actual);
            if (0U != KerosDemo_Compare(expected, actual,
                                        KEROS_DEMO_PAGE_SIZE, p_result))
            {
                p_result->outcome = KEROS_DEMO_UNVERIFIED;
                p_result->reason = KEROS_DEMO_REASON_EEPROM_UNVERIFIED;
                p_result->completed_cycles = 1U;
            }
            else if (p_page->observed_page >= 0)
            {
                elog_w(KEROS_DEMO_TAG,
                       "EEPROM page %u: ALIAS_CANDIDATE pattern_page=%d",
                       (unsigned int)page, (int)p_page->observed_page);
            }
        }
        KerosDemo_LogAccess(page, "READ", p_result);
    }
}

/**
 * @brief 按每页完整结果统计，将三个请求范围汇总到原有六项报告。
 * @param[in,out] p_report 有效报告；不将匹配字节数作为芯片物理容量。
 */
static void KerosDemo_CountPages(keros_demo_report_t *p_report)
{
    uint32_t matched[3] = {0U};
    uint32_t failed[3] = {0U};
    uint32_t limited[3] = {0U};
    uint8_t page = 0U;
    uint8_t group = 0U;
    const keros_demo_item_result_t *p_failure = NULL;
    keros_demo_item_result_t *p_item = NULL;
    const keros_demo_page_result_t *p_page = NULL;

    if (NULL == p_report)
    {
        return;
    }
    for (page = 0U; page < KEROS_DEMO_EEPROM_PROBE_PAGES; page++)
    {
        group = (page < 16U) ? 0U : ((page < 32U) ? 1U : 2U);
        p_page = &g_page_results[page];
        p_item = &p_report->items[KEROS_DEMO_CASE_EEPROM_PLAIN + group];
        p_failure = NULL;
        if (KEROS_DEMO_FAILED == p_page->read.outcome)
        {
            p_failure = &p_page->read;
        }
        if ((KEROS_DEMO_MODE_WRITE_READ == p_report->mode) &&
            (KEROS_DEMO_FAILED == p_page->write.outcome))
        {
            p_failure = &p_page->write;
        }
        if (NULL != p_failure)
        {
            if (0U == failed[group])
            {
                *p_item = *p_failure;
            }
            failed[group]++;
            p_report->page_failed_count++;
        }
        else if ((KEROS_DEMO_SDK_LIMIT == p_page->read.outcome) ||
                 ((KEROS_DEMO_MODE_WRITE_READ == p_report->mode) &&
                  (KEROS_DEMO_SDK_LIMIT == p_page->write.outcome)))
        {
            limited[group]++;
            p_report->page_sdk_limit_count++;
        }
        else if (KEROS_DEMO_UNVERIFIED == p_page->read.outcome)
        {
            matched[group]++;
            p_report->page_match_count++;
        }
        else
        {
            p_report->page_skipped_count++;
        }
    }
    for (group = 0U; group < 3U; group++)
    {
        p_item = &p_report->items[KEROS_DEMO_CASE_EEPROM_PLAIN + group];
        p_item->completed_cycles = matched[group];
        if (0U != failed[group])
        {
            p_item->outcome = KEROS_DEMO_FAILED;
        }
        else if (0U != matched[group])
        {
            p_item->outcome = KEROS_DEMO_UNVERIFIED;
            p_item->reason = (0U != limited[group]) ?
                KEROS_DEMO_REASON_SDK_LIMIT :
                KEROS_DEMO_REASON_EEPROM_UNVERIFIED;
            p_item->data_match = (0U == limited[group]) ? 1U : 0U;
        }
        else if (0U != limited[group])
        {
            p_item->outcome = KEROS_DEMO_SDK_LIMIT;
            p_item->reason = KEROS_DEMO_REASON_SDK_LIMIT;
        }
        elog_i(KEROS_DEMO_TAG, "%s: matched=%lu failed=%lu sdk_limit=%lu",
               g_case_names[KEROS_DEMO_CASE_EEPROM_PLAIN + group],
               (unsigned long)matched[group], (unsigned long)failed[group],
               (unsigned long)limited[group]);
    }
    p_report->matched_bytes = p_report->page_match_count * KEROS_DEMO_PAGE_SIZE;
    elog_i(KEROS_DEMO_TAG,
           "EEPROM pages: requested=%lu match=%lu fail=%lu sdk_limit=%lu",
           (unsigned long)p_report->page_count,
           (unsigned long)p_report->page_match_count,
           (unsigned long)p_report->page_failed_count,
           (unsigned long)p_report->page_sdk_limit_count);
    elog_w(KEROS_DEMO_TAG,
           "EEPROM matched_bytes=%lu; physical_capacity=UNDETERMINED",
           (unsigned long)p_report->matched_bytes);
}

/**
 * @brief 为全部测试项输出日志并按完整测试项统计。
 * @param[in,out] p_report 本次测试报告。
 */
static void KerosDemo_Summarize(keros_demo_report_t *p_report)
{
    uint32_t index = 0U;
    const keros_demo_item_result_t *p_item = NULL;

    if (NULL == p_report)
    {
        return;
    }
    p_report->total_count = KEROS_DEMO_CASE_COUNT;
    for (index = 0U; index < KEROS_DEMO_CASE_COUNT; index++)
    {
        p_item = &p_report->items[index];
        if (KEROS_DEMO_PASSED == p_item->outcome)
        {
            p_report->passed_count++;
            elog_i(KEROS_DEMO_TAG, "%s: PASS (cycles=%lu)",
                   g_case_names[index],
                   (unsigned long)p_item->completed_cycles);
        }
        else if (KEROS_DEMO_FAILED == p_item->outcome)
        {
            p_report->failed_count++;
            if (index >= KEROS_DEMO_CASE_EEPROM_PLAIN)
            {
                elog_e(KEROS_DEMO_TAG,
                       "%s: " KEROS_DEMO_EEPROM_MESSAGE
                       " / EEPROM access failed",
                       g_case_names[index]);
            }
            elog_e(KEROS_DEMO_TAG,
                   "%s: FAIL reason=%u stage=%u raw=%u cycles=%lu byte=%d",
                   g_case_names[index], (unsigned int)p_item->reason,
                   (unsigned int)p_item->stage,
                   (unsigned int)p_item->raw_status,
                   (unsigned long)p_item->completed_cycles,
                   (int)p_item->mismatch_index);
            if (0U != p_item->transport_status)
            {
                elog_e(KEROS_DEMO_TAG,
                       "%s: transport=%u sub=%04X direction=%s",
                       g_case_names[index],
                       (unsigned int)p_item->transport_status,
                       (unsigned int)p_item->transport_address,
                       (0U != p_item->transport_is_read) ? "read" : "write");
            }
        }
        else if (KEROS_DEMO_UNVERIFIED == p_item->outcome)
        {
            p_report->unverified_count++;
            elog_w(KEROS_DEMO_TAG,
                   "%s: matched_pages=%lu, EEPROM persistence unverified",
                   g_case_names[index],
                   (unsigned long)p_item->completed_cycles);
        }
        else if (KEROS_DEMO_SDK_LIMIT == p_item->outcome)
        {
            p_report->sdk_limit_count++;
            elog_w(KEROS_DEMO_TAG, "%s: SDK_LIMIT, hardware not probed",
                   g_case_names[index]);
        }
        else
        {
            p_report->skipped_count++;
            elog_w(KEROS_DEMO_TAG, "%s: SKIP reason=%u",
                   g_case_names[index], (unsigned int)p_item->reason);
        }
    }
    elog_i(KEROS_DEMO_TAG,
           "Total=%lu passed=%lu failed=%lu skipped=%lu unverified=%lu "
           "sdk_limit=%lu",
           (unsigned long)p_report->total_count,
           (unsigned long)p_report->passed_count,
           (unsigned long)p_report->failed_count,
           (unsigned long)p_report->skipped_count,
           (unsigned long)p_report->unverified_count,
           (unsigned long)p_report->sdk_limit_count);
}

/**
 * @brief 初始化软件 I2C、唤醒芯片、初始化 CID1018 库并读取序列号。
 * @return 成功返回 KEROS_DEMO_OK；否则返回具体初始化错误。
 * @note 主循环专用；HAL、系统时钟和日志须先初始化。
 */
keros_demo_status_t keros_demo_init(void)
{
    g_initialized = 0U;
    g_init_status = KEROS_DEMO_NOT_INITIALIZED;
    g_power_on_status = KEROS_DEMO_STATUS_UNUSED;
    g_lib_init_status = KEROS_DEMO_STATUS_UNUSED;
    memset(g_serial_number, 0, sizeof(g_serial_number));
    memset(g_page_results, 0, sizeof(g_page_results));
    KerosPort_ClearError();
    g_init_error = KerosPort_GetError();

    elog_i(KEROS_DEMO_TAG, "Initializing KEROS (CID1018, PB6/PB7)");
    I2CSetInitial();
    g_power_on_status = keros_power_on();
    if (0U != g_power_on_status)
    {
        g_init_status = KEROS_DEMO_POWER_ON_FAILED;
        elog_e(KEROS_DEMO_TAG, "Power ON failed, I2C status=%u",
               (unsigned int)g_power_on_status);
        return g_init_status;
    }
    keros_delay(KEROS_DEMO_POWER_DELAY_MS);
    KerosPort_ClearError();
    g_lib_init_status = keros_init(g_serial_number);
    g_init_error = KerosPort_GetError();
    if ((KEROS_STATUS_OK != g_lib_init_status) ||
        (0U != g_init_error.status))
    {
        /* 初始化失败时，不把部分输出当作有效序列号。 */
        memset(g_serial_number, 0, sizeof(g_serial_number));
        g_init_status = (0U != g_init_error.status) ?
            KEROS_DEMO_TRANSPORT_FAILED : KEROS_DEMO_LIB_INIT_FAILED;
        elog_e(KEROS_DEMO_TAG,
               "Library init failed, status=%u transport=%u sub=%04X",
               (unsigned int)g_lib_init_status,
               (unsigned int)g_init_error.status,
               (unsigned int)g_init_error.sub_address);
        return g_init_status;
    }
    g_initialized = 1U;
    g_init_status = KEROS_DEMO_OK;
    elog_i(KEROS_DEMO_TAG, "Serial (read only): %02X %02X %02X %02X %02X",
           (unsigned int)g_serial_number[0],
           (unsigned int)g_serial_number[1],
           (unsigned int)g_serial_number[2],
           (unsigned int)g_serial_number[3],
           (unsigned int)g_serial_number[4]);
    return KEROS_DEMO_OK;
}

/**
 * @brief 共用六项汇总流程，先写全部请求页再统一读取，或只读。
 * @param[out] p_report 非空报告，每次调用清零并重新统计。
 * @param[in] mode 写读模式或只读模式。
 * @return 实际失败优先，其次库范围受限，最后为匹配但未确认。
 * @note 请求页号 0~35，库限制和硬件访问失败分别记录。
 *       前置测试失败时跳过依赖项，不按配置推断芯片型号。
 */
static keros_demo_status_t KerosDemo_Run(keros_demo_report_t *p_report,
                                        keros_demo_mode_t mode)
{
    uint32_t index = 0U;

    if (NULL == p_report)
    {
        return KEROS_DEMO_INVALID_PARAMETER;
    }
    memset(p_report, 0, sizeof(*p_report));
    p_report->mode = mode;
    p_report->init_status = g_init_status;
    p_report->power_on_status = g_power_on_status;
    p_report->lib_init_status = g_lib_init_status;
    p_report->init_transport_status = g_init_error.status;
    p_report->init_transport_address = g_init_error.sub_address;
    p_report->init_transport_is_read = g_init_error.is_read;
    memcpy(p_report->serial_number, g_serial_number,
           sizeof(p_report->serial_number));
    p_report->page_count = KEROS_DEMO_EEPROM_PROBE_PAGES;
    p_report->page_skipped_count = KEROS_DEMO_EEPROM_PROBE_PAGES;
    p_report->p_pages = g_page_results;
    for (index = 0U; index < KEROS_DEMO_EEPROM_PROBE_PAGES; index++)
    {
        g_page_results[index].page = (uint8_t)index;
        g_page_results[index].observed_page = -1;
        KerosDemo_ResetResult(&g_page_results[index].write);
        KerosDemo_ResetResult(&g_page_results[index].read);
        if (0U == g_initialized)
        {
            g_page_results[index].write.reason =
                KEROS_DEMO_REASON_NOT_INITIALIZED;
            g_page_results[index].read.reason =
                KEROS_DEMO_REASON_NOT_INITIALIZED;
        }
    }
    for (index = 0U; index < KEROS_DEMO_CASE_COUNT; index++)
    {
        p_report->items[index].mismatch_index = -1;
        p_report->items[index].reason = (0U == g_initialized) ?
            KEROS_DEMO_REASON_NOT_INITIALIZED : KEROS_DEMO_REASON_PREREQUISITE;
    }
    if (0U == g_initialized)
    {
        KerosDemo_Summarize(p_report);
        return KEROS_DEMO_NOT_INITIALIZED;
    }

    elog_i(KEROS_DEMO_TAG,
           "Starting EEPROM %s probe (pattern=v2, pages=0..35, bytes=2304)",
           (KEROS_DEMO_MODE_READ_ONLY == mode) ? "READ_ONLY" : "WRITE_READ");
    elog_w(KEROS_DEMO_TAG,
           "SDK user pages=0..29 (1920 bytes); "
           "page 30 onward may be SDK_LIMIT");
    KerosDemo_Bypass(&p_report->items[KEROS_DEMO_CASE_BYPASS]);
    if (KEROS_DEMO_PASSED ==
        p_report->items[KEROS_DEMO_CASE_BYPASS].outcome)
    {
        KerosDemo_Authenticate(
            &p_report->items[KEROS_DEMO_CASE_AUTHENTICATION]);
        if (KEROS_DEMO_PASSED ==
            p_report->items[KEROS_DEMO_CASE_AUTHENTICATION].outcome)
        {
            KerosDemo_AesRoundtrip(
                &p_report->items[KEROS_DEMO_CASE_AES_ROUNDTRIP]);
            if (KEROS_DEMO_MODE_WRITE_READ == mode)
            {
                KerosDemo_WritePages();
            }
            KerosDemo_ReadPages();
            p_report->page_skipped_count = 0U;
            KerosDemo_CountPages(p_report);
        }
    }
    KerosDemo_Summarize(p_report);
    if ((0U == p_report->failed_count) &&
        (0U == p_report->skipped_count))
    {
        if (0U != p_report->page_sdk_limit_count)
        {
            return KEROS_DEMO_EEPROM_SDK_LIMIT;
        }
        return (0U != p_report->unverified_count) ?
            KEROS_DEMO_EEPROM_UNVERIFIED : KEROS_DEMO_OK;
    }
    return KEROS_DEMO_TEST_FAILED;
}

/**
 * @brief 写入全部请求页后统一读回，以独立页模式检查地址重复。
 * @param[out] p_report 非空报告，每次调用清零并重新统计。
 * @return 实际失败优先，其次 SDK_LIMIT，再次为 UNVERIFIED。
 * @note 初始化后从主循环调用；请求库页号 0~35，不绕过库边界。
 */
keros_demo_status_t keros_demo_test(keros_demo_report_t *p_report)
{
    return KerosDemo_Run(p_report, KEROS_DEMO_MODE_WRITE_READ);
}

/**
 * @brief 只读全部请求页，按新版 v2 页模式核对断电后数据。
 * @param[out] p_report 非空报告，每次调用清零并重新统计。
 * @return 实际失败优先，其次 SDK_LIMIT，再次为 UNVERIFIED。
 * @note 每页尝试一次，不调用 EEPROM 写入；须先运行新版写读入口。
 *       实际断电须由使用者完成，程序不自动认定断电保持已通过。
 */
keros_demo_status_t keros_demo_test_read_only(keros_demo_report_t *p_report)
{
    return KerosDemo_Run(p_report, KEROS_DEMO_MODE_READ_ONLY);
}
/************************ Function Definitions ************************/
