/******************************************************************************
 * @file bsp_keros_soft_i2c.c
 *
 * @par dependencies
 * - bsp_keros_soft_i2c.h
 * - string.h
 * - stm32f4xx_hal.h
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

/***************************** Defines ******************************/

#define _SOURCE_I2CBUS_

/***************************** Defines ******************************/

/***************************** Includes *****************************/

#include "bsp_keros_soft_i2c.h" /* 本模块接口 */

#include <string.h> /* C 标准库 */

#include "stm32f4xx_hal.h" /* Core/HAL 层 */

#include "keros_lib.h" /* 芯片 SDK 层 */

/***************************** Includes *****************************/

/*********************** Variable Definitions ***********************/

/* 事务记录区，仅保存当前轮次的前 48 条记录。 */
static KerosI2C_Trace s_diag_trace[KEROS_I2C_DIAG_CAPACITY];
/* 正在组装的事务记录。 */
static KerosI2C_Trace s_diag_current;
/* 非 0 时启用 RAM 记录。 */
static uint8_t s_diag_enabled;
/* 后续事务使用的业务阶段标签。 */
static uint8_t s_diag_phase;
/* 当前有效记录数量。 */
static uint8_t s_diag_count;
/* 记录容量耗尽后的丢弃数量。 */
static uint16_t s_diag_dropped;
/* 本轮记录的失败事务数量。 */
static uint16_t s_diag_errors;
/* 非 0 时使用 DWT 慢速时序。 */
static uint8_t s_slow_timing;
/* 当前事务的 SCL 等待超时标志。 */
static uint8_t s_clock_timeout;
/* DWT 每微秒对应的 HCLK 周期数。 */
static uint32_t s_cycles_per_us;
/* STOP 后额外空闲时间，单位 us；0 关闭。 */
static uint32_t s_bus_free_delay_us;

/*********************** Variable Definitions ***********************/

/**************************** Declaring *****************************/

/**
 * @brief 在慢速模式中等待 SCL 实际变高。
 *
 * @return 无。
 *
 * @note 通过 DWT 计时；超过 1000 us 后设置超时标志并返回。
 */
static void I2CDiagnostic_WaitClock(void);

/**************************** Declaring *****************************/

/***************************** Defines ******************************/

#define DLY_TIM_1 10
#define DLY_TIM_2 1

#define SDA_GPIO_Port GPIOB
#define SCL_GPIO_Port GPIOB

#define SDA_Pin GPIO_PIN_7
#define SCL_Pin GPIO_PIN_6

#define SDAHigh0                                                               \
    {                                                                          \
        HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);               \
    }
#define SDALow0                                                                \
    {                                                                          \
        HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET);             \
    }

#define SCLHigh0                                                               \
    {                                                                          \
        HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);               \
        if (s_slow_timing != 0U)                                               \
        {                                                                      \
            I2CDiagnostic_WaitClock();                                         \
        }                                                                      \
    }
#define SCLLow0                                                                \
    {                                                                          \
        HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET);             \
    }
#define SDAIn0(bMask)                                                          \
    {                                                                          \
        bMask = HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin);                      \
    }

/***************************** Defines ******************************/

/*********************** Function Definitions ***********************/

/**
 * @brief 将 PB6 配置为低速推挽输出，无内部上下拉。
 *
 * @return 无。
 *
 * @note 保留的底层接口；常规 I2C 初始化使用开漏配置，并不调用此函数。
 */
void SCL0_PORT_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SCL_Pin;
    /* 保留旧接口的推挽模式；常规初始化另外配置开漏。 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief 将 PB7 配置为高速开漏输出，无内部上下拉。
 *
 * @return 无。
 *
 * @note 释放 SDA 后的高电平由外部上拉提供。
 */
void SDA0_PORT_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief 将 PB7 切换为输入，用于采样从机数据或 ACK。
 *
 * @return 无。
 */
void SDA0_PORT_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief 提供软件 I2C 的忙等延时。
 *
 * @param[in] ticks 原始模式的循环计数。
 *
 * @return 无。
 *
 * @note 原始模式递减计数；慢速模式通过 DWT 固定等待 10 us，忽略计数。
 * @note 此接口不调用操作系统，也不以系统节拍或毫秒为单位。
 */
void OSTimeDly(volatile uint16_t ticks)
{
    if (s_slow_timing != 0U)
    {
        uint32_t start = DWT->CYCCNT;
        uint32_t wait_cycles = s_cycles_per_us * 10U;
        (void)ticks;
        while ((uint32_t)(DWT->CYCCNT - start) < wait_cycles)
        {
        }
        return;
    }
    while (ticks--)
        ;
}

/**
 * @brief 在慢速模式中等待 SCL 实际变高。
 *
 * @return 无。
 *
 * @note 通过 DWT 计时；超过 1000 us 后设置超时标志并返回。
 */
static void I2CDiagnostic_WaitClock(void)
{
    uint32_t start = DWT->CYCCNT;
    while (HAL_GPIO_ReadPin(SCL_GPIO_Port, SCL_Pin) == GPIO_PIN_RESET)
    {
        if ((uint32_t)(DWT->CYCCNT - start) >= s_cycles_per_us * 1000U)
        {
            s_clock_timeout = 1U;
            break;
        }
    }
}

/**
 * @brief 选择原始延时或 DWT 慢速时序。
 *
 * @param[in] slow 0 使用原始时序，非 0 启用慢速时序。
 *
 * @return 无。
 *
 * @note 启用时根据 HCLK 计算每微秒周期数，并开启 DWT 计数器。
 */
void I2CDiagnostic_SetSlowTiming(uint8_t slow)
{
    s_slow_timing = slow;
    if (slow != 0U)
    {
        s_cycles_per_us = HAL_RCC_GetHCLKFreq() / 1000000U;
        if (s_cycles_per_us == 0U)
        {
            s_cycles_per_us = 1U;
        }
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

/**
 * @brief 设置 STOP 后额外的总线空闲延时。
 *
 * @param[in] delay_us 延时时间，单位 us；0 关闭额外延时。
 *
 * @return 无。
 *
 * @note 非 0 时初始化 DWT 计时；读事务内部的重复 START 不增加该延时。
 */
void I2CDiagnostic_SetBusFreeDelayUs(uint32_t delay_us)
{
    s_bus_free_delay_us = delay_us;
    if (delay_us != 0U)
    {
        s_cycles_per_us = HAL_RCC_GetHCLKFreq() / 1000000U;
        if (s_cycles_per_us == 0U)
        {
            s_cycles_per_us = 1U;
        }
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

/**
 * @brief 重置 RAM 事务记录计数、错误计数及当前诊断阶段。
 *
 * @param[in] enable 0 关闭记录，非 0 启用记录。
 *
 * @return 无。
 *
 * @note 记录过程中不打印日志；旧数组内容不会清零，有效范围由计数决定。
 */
void I2CDiagnostic_Reset(uint8_t enable)
{
    s_diag_enabled = enable;
    s_diag_count = 0U;
    s_diag_dropped = 0U;
    s_diag_errors = 0U;
    s_diag_phase = 0U;
    s_clock_timeout = 0U;
}

/**
 * @brief 为后续事务设置业务阶段标签。
 *
 * @param[in] phase 调用方定义的阶段编号。
 *
 * @return 无。
 */
void I2CDiagnostic_SetPhase(uint8_t phase)
{
    s_diag_phase = phase;
}

/**
 * @brief 取得已保存的事务记录数量。
 *
 * @return 当前有效记录数，最多 48 条。
 */
uint8_t I2CDiagnostic_GetCount(void)
{
    return s_diag_count;
}

/**
 * @brief 取得记录容量耗尽后丢弃的事务数量。
 *
 * @return 自上次重置以来的丢弃计数。
 */
uint16_t I2CDiagnostic_GetDropped(void)
{
    return s_diag_dropped;
}

/**
 * @brief 取得已记录的失败事务数量。
 *
 * @return 启用记录期间、自上次重置以来的失败计数。
 */
uint16_t I2CDiagnostic_GetErrorCount(void)
{
    return s_diag_errors;
}

/**
 * @brief 按索引取得内部事务记录。
 *
 * @param[in] index 记录索引，从 0 开始。
 *
 * @return 有效索引返回内部只读指针，否则返回 NULL。
 *
 * @note 重置并开始新一轮记录后，原指针所指的数据可能被覆盖。
 */
const KerosI2C_Trace *I2CDiagnostic_GetTrace(uint8_t index)
{
    return (index < s_diag_count) ? &s_diag_trace[index] : NULL;
}

/**
 * @brief 清除超时标志，并在启用诊断时建立事务记录。
 *
 * @param[in] is_read 0 表示写事务，非 0 表示读事务。
 * @param[in] address 包含读写位的 8 位器件地址。
 * @param[in] sub 子地址缓冲区；无子地址时可为 NULL。
 * @param[in] sub_length 子地址字节数。
 * @param[in] length 请求传输的数据字节数。
 *
 * @return 无。
 *
 * @note 最多将子地址的前两个字节按高字节在前的顺序记录。
 */
static void I2CDiagnostic_Begin(uint8_t is_read, uint8_t address,
                                const uint8_t *sub, uint8_t sub_length,
                                uint8_t length)
{
    s_clock_timeout = 0U;
    if (s_diag_enabled == 0U)
    {
        return;
    }
    memset(&s_diag_current, 0, sizeof(s_diag_current));
    s_diag_current.phase = s_diag_phase;
    s_diag_current.is_read = is_read;
    s_diag_current.address = address;
    s_diag_current.sub_length = sub_length;
    s_diag_current.requested_length = length;
    s_diag_current.stage = KEROS_I2C_STAGE_ADDR_W;
    s_diag_current.idle_before = I2CBusProbeReadLines();
    if ((sub != NULL) && (sub_length != 0U))
    {
        s_diag_current.sub_address = sub[0];
        if (sub_length >= 2U)
        {
            s_diag_current.sub_address = ((uint16_t)sub[0] << 8) | sub[1];
        }
    }
}

/**
 * @brief 更新当前事务所处阶段与字节索引。
 *
 * @param[in] stage KerosI2C_Stage 阶段值。
 * @param[in] index 当前阶段的字节索引。
 *
 * @return 无。
 */
static void I2CDiagnostic_Stage(uint8_t stage, uint8_t index)
{
    if (s_diag_enabled != 0U)
    {
        s_diag_current.stage = stage;
        s_diag_current.byte_index = index;
    }
}

/**
 * @brief 将接收到的字节追加到当前事务记录。
 *
 * @param[in] value 刚接收到的字节。
 *
 * @return 无。
 *
 * @note 仅在启用诊断且数据区未满时保存，最多记录 17 字节。
 */
static void I2CDiagnostic_ReadByte(uint8_t value)
{
    if ((s_diag_enabled != 0U) &&
        (s_diag_current.data_length < KEROS_I2C_DIAG_DATA_SIZE))
    {
        s_diag_current.data[s_diag_current.data_length++] = value;
    }
}

/**
 * @brief 汇总事务状态并保存诊断记录。
 *
 * @param[in] status 事务原始状态，0 表示成功。
 *
 * @return 最终状态；检测到 SCL 超时时优先返回 ERR_I2C_CLOCK_TIMEOUT。
 *
 * @note 记录区已满时增加丢弃计数；错误计数仍会更新。
 */
static uint8_t I2CDiagnostic_End(uint8_t status)
{
    /* 即使记录关闭，也向调用方返回检测到的时钟超时。 */
    if (s_clock_timeout != 0U)
    {
        status = ERR_I2C_CLOCK_TIMEOUT;
    }
    if (s_diag_enabled != 0U)
    {
        s_diag_current.status = status;
        s_diag_current.clock_timeout = s_clock_timeout;
        s_diag_current.idle_after = I2CBusProbeReadLines();
        if (status == 0U)
        {
            s_diag_current.stage = KEROS_I2C_STAGE_DONE;
        }
        else
        {
            ++s_diag_errors;
        }
        if (s_diag_count < KEROS_I2C_DIAG_CAPACITY)
        {
            s_diag_trace[s_diag_count++] = s_diag_current;
        }
        else
        {
            ++s_diag_dropped;
        }
    }
    return status;
}

/**
 * @brief 初始化 PB6/SCL 和 PB7/SDA，并发送 STOP。
 *
 * @return 无。
 *
 * @note 使能 GPIOB 时钟；两线均为开漏输出，无内部上下拉，需外部上拉。
 */
void I2CSetInitial(void)
{

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStructure.Pin = SDA_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = SCL_Pin;
    HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStructure);

    I2CMasterStop();
}

/**
 * @brief 依次设置 SDA、SCL 的输出状态，供接线诊断使用。
 *
 * @param[in] levels 位 0 为 SCL，位 1 为 SDA；置 1 释放，置 0 拉低。
 *
 * @return 无。
 *
 * @note 先调用 I2CSetInitial()；本函数不改变 GPIO 模式。
 */
void I2CBusProbeSetLines(uint8_t levels)
{
    HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin,
                      (levels & KEROS_BUS_PROBE_SDA) ? GPIO_PIN_SET
                                                     : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin,
                      (levels & KEROS_BUS_PROBE_SCL) ? GPIO_PIN_SET
                                                     : GPIO_PIN_RESET);
}

/**
 * @brief 读取 PB6/SCL 与 PB7/SDA 的实际输入电平。
 *
 * @return 位 0 为 SCL，位 1 为 SDA；对应位为 1 表示高电平。
 */
uint8_t I2CBusProbeReadLines(void)
{
    uint8_t levels = 0U;

    if (HAL_GPIO_ReadPin(SCL_GPIO_Port, SCL_Pin) == GPIO_PIN_SET)
    {
        levels |= KEROS_BUS_PROBE_SCL;
    }
    if (HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin) == GPIO_PIN_SET)
    {
        levels |= KEROS_BUS_PROBE_SDA;
    }
    return levels;
}

/**
 * @brief 在 SCL 高电平期间释放 SDA，产生 STOP。
 *
 * @return 无。
 *
 * @note 先拉低两线再依次释放 SCL、SDA；按配置增加总线空闲延时。
 */
void I2CMasterStop(void)
{
    SDA0_PORT_OUTPUT();

    SCLLow0;
    SDALow0;
    OSTimeDly(DLY_TIM_1);
    SCLHigh0;
    OSTimeDly(DLY_TIM_1);
    SDAHigh0;

    /* 按配置增加 STOP 后空闲时间；读事务的重复 START 不经过此处。 */
    if (s_bus_free_delay_us != 0U)
    {
        uint32_t start = DWT->CYCCNT;
        uint32_t wait_cycles = s_cycles_per_us * s_bus_free_delay_us;
        while ((uint32_t)(DWT->CYCCNT - start) < wait_cycles)
        {
        }
    }
}

/**
 * @brief 检查 SDA；若为低电平，则尝试发送 STOP。
 *
 * @return 无。
 *
 * @note 不初始化 GPIO，也不执行九个时钟脉冲的总线恢复。
 */
void I2CMasterInit(void)
{
    uint8_t cRc;

    SDAIn0(cRc);

    if (!cRc)
        I2CMasterStop();
}

/**
 * @brief 在 SCL 高电平期间拉低 SDA，产生 START。
 *
 * @return 无。
 *
 * @note 先将 SDA 切换为开漏输出；结束时保持 SCL 为低电平。
 */
void I2CMasterStart(void)
{
    SDA0_PORT_OUTPUT();

    SDAHigh0;
    SCLHigh0;
    OSTimeDly(DLY_TIM_1);
    SDALow0;
    OSTimeDly(DLY_TIM_1);
    SCLLow0;
}

/**
 * @brief 按最高位在前发送一个字节，并采样从机应答。
 *
 * @param[in] bDataByte 待发送字节。
 *
 * @return 0 表示 ACK，1 表示 NACK。
 *
 * @note 发送后将 SDA 切换为输入采样 ACK，再恢复开漏输出。
 */
uint8_t I2CMasterWrite(uint8_t bDataByte)
{
    uint8_t bMask;
    int i;

    bMask = 0x80;

    {
        for (i = 0; i < 8; i++)
        {
            if ((bMask & bDataByte))
                SDAHigh0
            else
                SDALow0

            bMask = bMask >> 1;

            SCLHigh0;
            OSTimeDly(DLY_TIM_1);
            SCLLow0;
            OSTimeDly(DLY_TIM_1);
        }

        SDAHigh0;
        SDA0_PORT_INPUT();

        SCLHigh0;
        OSTimeDly(DLY_TIM_1);
        SDAIn0(bMask);
        SCLLow0;

        SDA0_PORT_OUTPUT();
    }

    return bMask;
}

/**
 * @brief 按最高位在前接收一个字节，并发送主机应答。
 *
 * @param[in] bAck 非 0 发送 ACK；0 发送 NACK，用于最后一个字节。
 *
 * @return 接收到的字节。
 */
uint8_t I2CMasterRead(uint8_t bAck)
{
    uint8_t bMask, cReceiveByte, i;
    uint8_t cRc;

    cReceiveByte = 0;
    bMask = 0x80;

    {

        SDA0_PORT_INPUT();

        for (i = 0; i < 8; i++)
        {

            SCLHigh0;
            if (s_slow_timing != 0U)
            {
                OSTimeDly(DLY_TIM_1);
            }

            SDAIn0(cRc);
            if (cRc)
                cReceiveByte |= bMask;
            else
                cReceiveByte |= 0x00;

            SCLLow0;
            bMask = bMask >> 1;
            OSTimeDly(DLY_TIM_1);
        }

        SDA0_PORT_OUTPUT();

        SDAHigh0;

        if (bAck)
            SDALow0;

        if (s_slow_timing != 0U)
        {
            OSTimeDly(DLY_TIM_1);
        }

        SCLHigh0;
        OSTimeDly(DLY_TIM_1);
        SCLLow0;
    }

    return cReceiveByte;
}

/**
 * @brief 发送 START 和写方向器件地址，最多尝试三次。
 *
 * @param[in] bDevAddr 包含读写位的 8 位写地址。
 *
 * @return 0 表示 ACK；多次无应答时返回 ERR_I2C_NO_ADDRESS_ACK。
 *
 * @note 本函数不主动发送 STOP，由上层事务收尾。
 */
uint8_t I2CSendDeviceAddress(uint8_t bDevAddr)
{
    uint8_t i;

    I2CMasterInit();
    for (i = 0; i < MAXATTEMPTS; i++)
    {
        if (s_diag_enabled != 0U)
        {
            s_diag_current.address_attempts = i + 1U;
        }
        I2CMasterStart();
        if (I2CMasterWrite(bDevAddr) == 0)
        {
            return 0;
        }
    }
    return ERR_I2C_NO_ADDRESS_ACK;
}

/**
 * @brief 发送重复 START 和读方向器件地址。
 *
 * @param[in] bDevAddr 8 位器件地址；函数将最低位置 1。
 *
 * @return 0 表示 ACK，1 表示 NACK。
 */
uint8_t I2CMasterReceiver(uint8_t bDevAddr)
{
    bDevAddr |= 0x01;
    I2CMasterStart();
    return I2CMasterWrite(bDevAddr);
}

/**
 * @brief 向指定器件发送子地址并写入数据，最后发送 STOP。
 *
 * @param[in] bDevAddr 包含读写位的 8 位写地址。
 * @param[in] pbAddr 子地址缓冲区；长度为 0 时可为 NULL。
 * @param[in] wAddrLen 子地址长度，单位字节。
 * @param[in] pbData 数据缓冲区；长度为 0 时可为 NULL。
 * @param[in] wDataLen 数据长度，单位字节。
 *
 * @return 0 成功；0x02 地址 NACK；0x08 子地址或数据 NACK；0x10 时钟超时。
 *
 * @note 调用方保证正长度对应的缓冲区有效；失败也会发送 STOP。
 * @note 诊断启用时记录状态，事务内部不打印日志。
 */
uint8_t I2CWrite(uint8_t bDevAddr, uint8_t *pbAddr, uint8_t wAddrLen,
                 uint8_t *pbData, uint8_t wDataLen)
{
    uint8_t error;
    uint8_t index = 0U;
    I2CDiagnostic_Begin(0U, bDevAddr, pbAddr, wAddrLen, wDataLen);
    error = I2CSendDeviceAddress(bDevAddr);
    if (error != 0U)
    {
        I2CMasterStop();
        return I2CDiagnostic_End(error);
    }
    while (wAddrLen > 0U)
    {
        I2CDiagnostic_Stage(KEROS_I2C_STAGE_SUBADDR, index++);
        if (I2CMasterWrite(*pbAddr) != 0U)
        {
            I2CMasterStop();
            return I2CDiagnostic_End(ERR_I2C_NO_DATA_ACK);
        }
        ++pbAddr;
        --wAddrLen;
    }
    index = 0U;
    while (wDataLen > 0U)
    {
        I2CDiagnostic_Stage(KEROS_I2C_STAGE_TX_DATA, index++);
        if (I2CMasterWrite(*pbData) != 0U)
        {
            I2CMasterStop();
            return I2CDiagnostic_End(ERR_I2C_NO_DATA_ACK);
        }
        ++pbData;
        --wDataLen;
    }
    I2CMasterStop();
    return I2CDiagnostic_End(0U);
}

/**
 * @brief 向指定器件发送子地址并读取数据，最后发送 STOP。
 *
 * @param[in] bDevAddr 包含读写位的 8 位写地址。
 * @param[in] pbAddr 子地址缓冲区；长度为 0 时可为 NULL。
 * @param[in] wAddrLen 子地址长度，单位字节。
 * @param[out] pbData 数据缓冲区；长度为 0 时可为 NULL。
 * @param[in] wDataLen 数据长度，单位字节。
 *
 * @return 0 成功；0x02 地址 NACK；0x08 子地址或数据 NACK；0x10 时钟超时。
 *
 * @note 调用方保证正长度对应的缓冲区有效；失败也会发送 STOP。
 * @note 读事务使用重复 START，最后一个接收字节发送 NACK。
 */
uint8_t I2CRead(uint8_t bDevAddr, uint8_t *pbAddr, uint8_t wAddrLen,
                uint8_t *pbData, uint8_t wDataLen)
{
    uint8_t error;
    uint8_t index = 0U;
    I2CDiagnostic_Begin(1U, bDevAddr, pbAddr, wAddrLen, wDataLen);
    error = I2CSendDeviceAddress(bDevAddr);
    if (error != 0U)
    {
        I2CMasterStop();
        return I2CDiagnostic_End(error);
    }
    while (wAddrLen > 0U)
    {
        I2CDiagnostic_Stage(KEROS_I2C_STAGE_SUBADDR, index++);
        if (I2CMasterWrite(*pbAddr) != 0U)
        {
            I2CMasterStop();
            return I2CDiagnostic_End(ERR_I2C_NO_DATA_ACK);
        }
        ++pbAddr;
        --wAddrLen;
    }
    SDAHigh0;
    I2CDiagnostic_Stage(KEROS_I2C_STAGE_ADDR_R, 0U);
    error = I2CMasterReceiver(bDevAddr);
    if (error != 0U)
    {
        I2CMasterStop();
        return I2CDiagnostic_End(ERR_I2C_NO_ADDRESS_ACK);
    }
    index = 0U;
    while (wDataLen > 0U)
    {
        I2CDiagnostic_Stage(KEROS_I2C_STAGE_RX_DATA, index++);
        *pbData = I2CMasterRead((uint8_t)(wDataLen > 1U));
        I2CDiagnostic_ReadByte(*pbData);
        ++pbData;
        --wDataLen;
    }
    I2CMasterStop();
    return I2CDiagnostic_End(0U);
}

/*********************** Function Definitions ***********************/
