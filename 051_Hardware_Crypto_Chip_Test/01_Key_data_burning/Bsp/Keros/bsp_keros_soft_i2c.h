/******************************************************************************
 * @file bsp_keros_soft_i2c.h
 *
 * @par dependencies
 * - keros_lib.h
 *
 * @brief Software I2C on PB6/PB7 with optional RAM transaction tracing.
 *
 * Processing flow:
 * Initialize open-drain GPIO, then transfer bytes and check ACKs.
 * Optional diagnostics capture transactions without bus-time logging.
 *
 * @version V0.3
 * @date 2026-10-12
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

#ifndef BSP_KEROS_SOFT_I2C_H
#define BSP_KEROS_SOFT_I2C_H

/***************************** Includes *****************************/

#include "keros_lib.h" /* 芯片 SDK 层 */

/***************************** Includes *****************************/

/**************************** Declaring *****************************/

#ifdef _SOURCE_I2CBUS_
#define I2C_EXT
#else
#define I2C_EXT extern
#endif

/* 8 位写地址 0x38，对应 7 位地址 0x1C。 */
#define KEROS_DEVID_ADDR 0x38

/* SCL 电平位掩码。 */
#define KEROS_BUS_PROBE_SCL 0x01U
/* SDA 电平位掩码。 */
#define KEROS_BUS_PROBE_SDA 0x02U

/**
 * @brief 依次设置 SDA、SCL 的输出状态，供接线诊断使用。
 *
 * @param[in] levels 位 0 为 SCL，位 1 为 SDA；置 1 释放，置 0 拉低。
 *
 * @return 无。
 *
 * @note 先调用 I2CSetInitial()；本函数不改变 GPIO 模式。
 */
void I2CBusProbeSetLines(uint8_t levels);

/**
 * @brief 读取 PB6/SCL 与 PB7/SDA 的实际输入电平。
 *
 * @return 位 0 为 SCL，位 1 为 SDA；对应位为 1 表示高电平。
 */
uint8_t I2CBusProbeReadLines(void);

/* 保留的通道编号；当前移植实际使用 PB6/PB7。 */
enum
{
    BUS0,   /**< 保留通道 0。 */
    BUS1,   /**< 保留通道 1。 */
    BUS2,   /**< 保留通道 2。 */
    BUS3,   /**< 保留通道 3。 */
    BUS4,   /**< 旧接口的外部通道。 */
    BUS_MAX /**< 通道枚举上界。 */
};

/* 保留错误码；当前事务实现不返回此值。 */
#define ERR_I2C_BUS_DISABLE 0x01
/* 地址阶段未收到 ACK。 */
#define ERR_I2C_NO_ADDRESS_ACK 0x02
/* 保留错误码；当前子地址 NACK 归入 0x08。 */
#define ERR_I2C_NO_SUBADDRESS_ACK 0x04
/* 子地址或数据阶段未收到 ACK。 */
#define ERR_I2C_NO_DATA_ACK 0x08
/* 慢速模式下等待 SCL 变高超时。 */
#define ERR_I2C_CLOCK_TIMEOUT 0x10U

/* 每轮最多保存 48 条事务。 */
#define KEROS_I2C_DIAG_CAPACITY 48U
/* 每条记录最多保存 17 个接收字节。 */
#define KEROS_I2C_DIAG_DATA_SIZE 17U
typedef enum
{
    KEROS_I2C_STAGE_ADDR_W = 0, /**< 发送写地址。 */
    KEROS_I2C_STAGE_SUBADDR,    /**< 发送子地址。 */
    KEROS_I2C_STAGE_ADDR_R,     /**< 发送读地址。 */
    KEROS_I2C_STAGE_TX_DATA,    /**< 发送数据。 */
    KEROS_I2C_STAGE_RX_DATA,    /**< 接收数据。 */
    KEROS_I2C_STAGE_DONE        /**< 事务成功结束。 */
} KerosI2C_Stage;

typedef struct
{
    uint16_t sub_address;     /**< 记录的子地址，最多两字节。 */
    uint8_t phase;            /**< 调用方业务阶段。 */
    uint8_t is_read;          /**< 非 0 为读事务。 */
    uint8_t address;          /**< 包含读写位的 8 位地址。 */
    uint8_t sub_length;       /**< 子地址长度，单位字节。 */
    uint8_t requested_length; /**< 请求传输长度，单位字节。 */
    uint8_t status;           /**< 最终事务状态，0 成功。 */
    uint8_t stage;            /**< 最后到达的 I2C 阶段。 */
    uint8_t byte_index;       /**< 当前阶段的字节索引。 */
    uint8_t address_attempts; /**< 写地址发送尝试次数。 */
    uint8_t idle_before;      /**< 事务前电平，位 0 SCL、位 1 SDA。 */
    uint8_t idle_after;       /**< STOP 后电平，位 0 SCL、位 1 SDA。 */
    uint8_t clock_timeout;    /**< 非 0 表示 SCL 等待超时。 */
    uint8_t data_length;      /**< 实际保存的接收字节数。 */
    uint8_t data[KEROS_I2C_DIAG_DATA_SIZE]; /**< 接收数据快照，最多 17 字节。 */
} KerosI2C_Trace;

/**
 * @brief 重置 RAM 事务记录计数、错误计数及当前诊断阶段。
 *
 * @param[in] enable 0 关闭记录，非 0 启用记录。
 *
 * @return 无。
 *
 * @note 记录过程中不打印日志；旧数组内容不会清零，有效范围由计数决定。
 */
void I2CDiagnostic_Reset(uint8_t enable);

/**
 * @brief 为后续事务设置业务阶段标签。
 *
 * @param[in] phase 调用方定义的阶段编号。
 *
 * @return 无。
 */
void I2CDiagnostic_SetPhase(uint8_t phase);

/**
 * @brief 选择原始延时或 DWT 慢速时序。
 *
 * @param[in] slow 0 使用原始时序，非 0 启用慢速时序。
 *
 * @return 无。
 *
 * @note 启用时根据 HCLK 计算每微秒周期数，并开启 DWT 计数器。
 */
void I2CDiagnostic_SetSlowTiming(uint8_t slow);

/**
 * @brief 设置 STOP 后额外的总线空闲延时。
 *
 * @param[in] delay_us 延时时间，单位 us；0 关闭额外延时。
 *
 * @return 无。
 *
 * @note 非 0 时初始化 DWT 计时；读事务内部的重复 START 不增加该延时。
 */
void I2CDiagnostic_SetBusFreeDelayUs(uint32_t delay_us);

/**
 * @brief 取得已保存的事务记录数量。
 *
 * @return 当前有效记录数，最多 48 条。
 */
uint8_t I2CDiagnostic_GetCount(void);

/**
 * @brief 取得记录容量耗尽后丢弃的事务数量。
 *
 * @return 自上次重置以来的丢弃计数。
 */
uint16_t I2CDiagnostic_GetDropped(void);

/**
 * @brief 取得已记录的失败事务数量。
 *
 * @return 启用记录期间、自上次重置以来的失败计数。
 */
uint16_t I2CDiagnostic_GetErrorCount(void);

/**
 * @brief 按索引取得内部事务记录。
 *
 * @param[in] index 记录索引，从 0 开始。
 *
 * @return 有效索引返回内部只读指针，否则返回 NULL。
 *
 * @note 重置并开始新一轮记录后，原指针所指的数据可能被覆盖。
 */
const KerosI2C_Trace *I2CDiagnostic_GetTrace(uint8_t index);

/* 写地址最多发送三次。 */
#define MAXATTEMPTS 3
/* 旧接口常量，当前 DWT 超时逻辑未使用。 */
#define MAX_CLOCKSTRETCHING 5000

/**
 * @brief 初始化 PB6/SCL 和 PB7/SDA，并发送 STOP。
 *
 * @return 无。
 *
 * @note 使能 GPIOB 时钟；两线均为开漏输出，无内部上下拉，需外部上拉。
 */
I2C_EXT void I2CSetInitial(void);

/**
 * @brief 保留旧接口的总线使能声明。
 *
 * @param[in] cEnable 旧接口的使能参数。
 *
 * @return 无。
 *
 * @note 当前移植未提供实现，不应直接调用此接口。
 */
I2C_EXT void I2CSetEnable(uint8_t cEnable);

/**
 * @brief 保留旧接口的总线使能查询声明。
 *
 * @return 当前移植未实现，返回值语义需由实现方定义。
 *
 * @note 当前移植未提供实现，不应直接调用此接口。
 */
I2C_EXT uint8_t I2CGetEnable(void);

/**
 * @brief 向指定器件发送子地址并写入数据，最后发送 STOP。
 *
 * @param[in] bDevAddr 包含读写位的 8 位写地址。
 * @param[in] pbAddr 子地址缓冲区；长度为 0 时可为 NULL。
 * @param[in] wAddrLen 子地址长度，单位字节。
 * @param[in] pbData 数据缓冲区；长度为 0 时可为 NULL。
 * @param[in] wData 数据长度，单位字节。
 *
 * @return 0 成功；0x02 地址 NACK；0x08 子地址或数据 NACK；0x10 时钟超时。
 *
 * @note 调用方保证正长度对应的缓冲区有效；失败也会发送 STOP。
 * @note 诊断启用时记录状态，事务内部不打印日志。
 */
I2C_EXT uint8_t I2CWrite(uint8_t bDevAddr, uint8_t *pbAddr, uint8_t wAddrLen,
                         uint8_t *pbData, uint8_t wData);

/**
 * @brief 向指定器件发送子地址并读取数据，最后发送 STOP。
 *
 * @param[in] bDevAddr 包含读写位的 8 位写地址。
 * @param[in] pbAddr 子地址缓冲区；长度为 0 时可为 NULL。
 * @param[in] wAddrLen 子地址长度，单位字节。
 * @param[out] pbData 数据缓冲区；长度为 0 时可为 NULL。
 * @param[in] wData 数据长度，单位字节。
 *
 * @return 0 成功；0x02 地址 NACK；0x08 子地址或数据 NACK；0x10 时钟超时。
 *
 * @note 调用方保证正长度对应的缓冲区有效；失败也会发送 STOP。
 * @note 读事务使用重复 START，最后一个接收字节发送 NACK。
 */
I2C_EXT uint8_t I2CRead(uint8_t bDevAddr, uint8_t *pbAddr, uint8_t wAddrLen,
                        uint8_t *pbData, uint8_t wData);

/**
 * @brief 发送 START 和写方向器件地址，最多尝试三次。
 *
 * @param[in] bDevAddr 包含读写位的 8 位写地址。
 *
 * @return 0 表示 ACK；多次无应答时返回 ERR_I2C_NO_ADDRESS_ACK。
 *
 * @note 本函数不主动发送 STOP，由上层事务收尾。
 */
I2C_EXT uint8_t I2CSendDeviceAddress(uint8_t bDevAddr);

/**
 * @brief 在 SCL 高电平期间释放 SDA，产生 STOP。
 *
 * @return 无。
 *
 * @note 先拉低两线再依次释放 SCL、SDA；按配置增加总线空闲延时。
 */
I2C_EXT void I2CMasterStop(void);

/**
 * @brief 检查 SDA；若为低电平，则尝试发送 STOP。
 *
 * @return 无。
 *
 * @note 不初始化 GPIO，也不执行九个时钟脉冲的总线恢复。
 */
I2C_EXT void I2CMasterInit(void);

/**
 * @brief 在 SCL 高电平期间拉低 SDA，产生 START。
 *
 * @return 无。
 *
 * @note 先将 SDA 切换为开漏输出；结束时保持 SCL 为低电平。
 */
I2C_EXT void I2CMasterStart(void);

/**
 * @brief 按最高位在前发送一个字节，并采样从机应答。
 *
 * @param[in] bDataByte 待发送字节。
 *
 * @return 0 表示 ACK，1 表示 NACK。
 *
 * @note 发送后将 SDA 切换为输入采样 ACK，再恢复开漏输出。
 */
I2C_EXT uint8_t I2CMasterWrite(uint8_t bDataByte);

/**
 * @brief 按最高位在前接收一个字节，并发送主机应答。
 *
 * @param[in] bAck 非 0 发送 ACK；0 发送 NACK，用于最后一个字节。
 *
 * @return 接收到的字节。
 */
I2C_EXT uint8_t I2CMasterRead(uint8_t bAck);

/**
 * @brief 发送重复 START 和读方向器件地址。
 *
 * @param[in] cAddr 8 位器件地址；函数将最低位置 1。
 *
 * @return 0 表示 ACK，1 表示 NACK。
 */
I2C_EXT uint8_t I2CMasterReceiver(uint8_t cAddr);

/**************************** Declaring *****************************/

#endif
