/******************************************************************************
 * @file keros_demo.h
 * @par dependencies
 * - stdint.h
 * @brief Declares initialization and EEPROM range test APIs.
 * Processing flow: initialize, then select one test mode per firmware run.
 * @version V1.0 2026-10-13
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

#ifndef KEROS_DEMO_H
#define KEROS_DEMO_H

/****************************** Includes ******************************/
#include <stdint.h> /* C 标准库 */
/****************************** Includes ******************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Defines ******************************/
#define KEROS_DEMO_SERIAL_SIZE 5U
#define KEROS_DEMO_EEPROM_PAGE_SIZE 64U
#define KEROS_DEMO_EEPROM_USER_PAGES 30U
#define KEROS_DEMO_EEPROM_PROBE_PAGES 36U

/* 应用 API 状态；与厂家库的状态码和认证布尔返回值独立。 */
typedef enum
{
    KEROS_DEMO_OK = 0,
    KEROS_DEMO_INVALID_PARAMETER,
    KEROS_DEMO_NOT_INITIALIZED,
    KEROS_DEMO_POWER_ON_FAILED,
    KEROS_DEMO_LIB_INIT_FAILED,
    KEROS_DEMO_TEST_FAILED,
    KEROS_DEMO_EEPROM_NOT_FOUND, /* 保留旧状态值，实际探测不按配置返回 */
    KEROS_DEMO_TRANSPORT_FAILED,
    KEROS_DEMO_EEPROM_UNVERIFIED, /* 数据匹配，但未确认非易失存储 */
    KEROS_DEMO_EEPROM_SDK_LIMIT   /* 库限制导致请求范围未全部实测 */
} keros_demo_status_t;

/* 只读模式不调用 EEPROM 写入；初始化和认证仍发送通信命令。 */
typedef enum
{
    KEROS_DEMO_MODE_WRITE_READ = 0,
    KEROS_DEMO_MODE_READ_ONLY
} keros_demo_mode_t;

/* 六个汇总项：三个基础功能及三个 EEPROM 请求范围。 */
typedef enum
{
    KEROS_DEMO_CASE_BYPASS = 0,
    KEROS_DEMO_CASE_AUTHENTICATION,
    KEROS_DEMO_CASE_AES_ROUNDTRIP,
    KEROS_DEMO_CASE_EEPROM_PLAIN,     /* 名称保留：请求页 0~15 */
    KEROS_DEMO_CASE_EEPROM_ENCRYPTED, /* 名称保留：请求页 16~31，现为明文 */
    KEROS_DEMO_CASE_EEPROM_CYCLE,     /* 名称保留：请求页 32~35 */
    KEROS_DEMO_CASE_COUNT
} keros_demo_case_t;

typedef enum
{
    KEROS_DEMO_SKIPPED = 0, /* 前置条件不满足，未执行 */
    KEROS_DEMO_PASSED,      /* 全部调用和数据校验成功 */
    KEROS_DEMO_FAILED,      /* 调用或数据校验失败 */
    KEROS_DEMO_UNVERIFIED,  /* EEPROM 数据匹配，断电保持未验证 */
    KEROS_DEMO_SDK_LIMIT    /* 库拒绝且未发出底层传输，不是硬件失败 */
} keros_demo_outcome_t;

typedef enum
{
    KEROS_DEMO_REASON_NONE = 0,
    KEROS_DEMO_REASON_NOT_INITIALIZED,
    KEROS_DEMO_REASON_PREREQUISITE,
    KEROS_DEMO_REASON_LIBRARY_STATUS,
    KEROS_DEMO_REASON_AUTH_RETURN,
    KEROS_DEMO_REASON_DATA_MISMATCH,
    KEROS_DEMO_REASON_EEPROM_NOT_FOUND, /* 保留，不用于配置推断 */
    KEROS_DEMO_REASON_TRANSPORT,
    KEROS_DEMO_REASON_EEPROM_UNVERIFIED,
    KEROS_DEMO_REASON_SDK_LIMIT,
    KEROS_DEMO_REASON_NO_TRANSFER /* 库返回成功，但未提交任何 I2C */
} keros_demo_reason_t;

/* 记录最后执行的阶段，用于解释 raw_status 的来源。 */
typedef enum
{
    KEROS_DEMO_STAGE_NONE = 0,
    KEROS_DEMO_STAGE_BYPASS,
    KEROS_DEMO_STAGE_AUTHENTICATION,
    KEROS_DEMO_STAGE_ENCRYPTION,
    KEROS_DEMO_STAGE_DECRYPTION,
    KEROS_DEMO_STAGE_EEPROM_WRITE,
    KEROS_DEMO_STAGE_EEPROM_READ
} keros_demo_stage_t;

typedef struct
{
    keros_demo_outcome_t outcome; /* 通过、失败、跳过或未确认 */
    keros_demo_reason_t reason;   /* 失败、跳过或未确认原因 */
    keros_demo_stage_t stage;     /* 最后一次厂家库调用阶段 */
    uint8_t raw_status_valid;    /* 1 表示 raw_status 有效 */
    uint8_t raw_status;          /* 该阶段的原始库返回值 */
    uint8_t transport_status;    /* 首次底层错误，0 为无错误 */
    uint16_t transport_address;  /* 该错误的芯片子地址 */
    uint8_t transport_is_read;   /* 1 为读，0 为写；错误非零时有效 */
    uint32_t completed_cycles;   /* 访问完成次数；范围汇总项为匹配页数 */
    int16_t mismatch_index;      /* 首个不匹配字节；-1 表示无记录 */
    uint8_t data_match;          /* 本项全部所需轮次数据匹配时为 1 */
    uint32_t io_read_calls;      /* 该库调用实际提交给 I2C 的读次数 */
    uint32_t io_write_calls;     /* 该库调用实际提交给 I2C 的写次数 */
} keros_demo_item_result_t;

/* 每页独立保存写入和读取状态，写失败也继续读取及测试后续页。 */
typedef struct
{
    uint8_t page;                        /* 库页号，使用零基索引 */
    keros_demo_item_result_t write;      /* 只读模式为 SKIPPED */
    keros_demo_item_result_t read;
    int16_t observed_page;               /* 匹配其他页模式时记录页号 */
} keros_demo_page_result_t;

typedef struct
{
    uint32_t total_count;            /* 固定六个测试项，支持 {0} 初始化 */
    keros_demo_status_t init_status; /* 最近一次初始化状态 */
    uint8_t power_on_status;         /* 最终唤醒命令 I2C 状态 */
    uint8_t lib_init_status;         /* keros_init 原始状态 */
    uint8_t init_transport_status;   /* 库初始化中的首次底层错误 */
    uint16_t init_transport_address;
    uint8_t init_transport_is_read;
    uint8_t serial_number[KEROS_DEMO_SERIAL_SIZE]; /* 读取的序列号 */
    keros_demo_item_result_t items[KEROS_DEMO_CASE_COUNT];
    uint32_t passed_count;
    uint32_t failed_count;
    uint32_t skipped_count;
    uint32_t unverified_count;       /* 数据匹配但非易失存储未确认 */
    keros_demo_mode_t mode;          /* 本次选择的 EEPROM 测试模式 */
    uint32_t sdk_limit_count;        /* 完全受库限制的汇总项数 */
    uint32_t page_count;             /* 请求的页数，当前为 36 */
    uint32_t page_match_count;       /* 完整读回匹配的页数 */
    uint32_t page_failed_count;      /* 实际访问或比对失败的页数 */
    uint32_t page_sdk_limit_count;   /* 库拒绝且未发生 I2C 的页数 */
    uint32_t page_skipped_count;     /* 前置失败导致未执行的页数 */
    uint32_t matched_bytes;          /* 匹配页数乘以 64，不是物理容量 */
    const keros_demo_page_result_t *p_pages; /* 模块内结果，只读借用 */
} keros_demo_report_t;
/****************************** Defines ******************************/

/****************************** Declaring ******************************/
/**
 * @brief 初始化 KEROS 软件 I2C、唤醒芯片并读取序列号。
 * @return KEROS_DEMO_OK 表示成功；否则返回唤醒、库或底层通信错误。
 * @note 在 HAL、系统时钟、EasyLogger 初始化后调用；主循环专用。
 *       使用 PB6/SCL、PB7/SDA 和 DWT；不修改 main.c 或其他外设。
 */
keros_demo_status_t keros_demo_init(void);

/**
 * @brief 先逐页写入，再统一读回整个请求范围，输出每页报告。
 * @param[out] p_report 报告缓冲区，不得为空；每次调用重新统计。
 * @return EEPROM 读写匹配但未验证断电保持时返回
 *         KEROS_DEMO_EEPROM_UNVERIFIED；库限制返回 SDK_LIMIT，
 *         真实访问或数据失败返回 TEST_FAILED。
 * @note 须先成功初始化；在中断开启的主循环调用，不可并发调用。
 *       请求库页号 0~35；0~29 为已说明用户页，30 起受库限制。
 *       所有页使用明文及密码 0，每页具有独立且可复现的 v2 数据。
 *       会覆盖全部可写用户页，包括页 0；不执行 eFuse 或密码修改。
 *       每页失败不终止扫描；最终读回可发现后续写入造成的地址重复。
 *       p_pages 在下一次初始化或测试时更新，不增加调用方的大栈数组。
 *       BYPASS 失败跳过后续项；认证失败跳过 AES 和 EEPROM。
 *       序列号仅确认读取，不验证唯一性；I2C 由功能调用间接验证。
 */
keros_demo_status_t keros_demo_test(keros_demo_report_t *p_report);

/**
 * @brief 执行只读核对的综合测试，不调用 EEPROM 写入。
 * @param[out] p_report 报告缓冲区，不得为空；每次调用重新统计。
 * @return 实际失败返回 TEST_FAILED，库限制返回 SDK_LIMIT，
 *         全部匹配返回 UNVERIFIED，不自动确认外部是否真正断电。
 * @note 须先成功初始化；在中断开启的主循环调用，不可并发调用。
 *       保留 BYPASS、认证和 AES 回环，再分别请求读取页 0~35 一次。
 *       期望值按页独立生成，必须先运行新版 v2 写读测试。
 *       明文读取，密码为 0；失败也继续后续页；输出完整读回数据。
 *       第一次运行写读测试后真正断电，第二次仅调用本接口。
 *       初始化、认证和读取过程仍会发送 I2C 命令，不代表写 EEPROM。
 *       返回值及 p_pages 生命周期与写读接口一致。
 */
keros_demo_status_t keros_demo_test_read_only(keros_demo_report_t *p_report);
/****************************** Declaring ******************************/

#ifdef __cplusplus
}
#endif
#endif /* KEROS_DEMO_H */
