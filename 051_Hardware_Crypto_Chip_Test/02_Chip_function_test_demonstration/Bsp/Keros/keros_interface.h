/******************************************************************************
 * @file keros_interface.h
 * @par dependencies
 * - stdint.h
 * @brief Declares callbacks, configured capacity, errors and call counts.
 * @version V0.3 2026-10-13
 * @note 1 tab == 4 spaces!
 ******************************************************************************/

#ifndef KEROS_INTERFACE_H
#define KEROS_INTERFACE_H

/****************************** Includes ******************************/
#include <stdint.h> /* C 标准库 */
/****************************** Includes ******************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Defines ******************************/
/* 板级型号信息：CK02AT 为 0；仅用于说明，不阻止实际 EEPROM 探测。 */
#ifndef KEROS_EEPROM_CAPACITY_BYTES
#define KEROS_EEPROM_CAPACITY_BYTES 0U
#endif

#if (0U != KEROS_EEPROM_CAPACITY_BYTES) && \
    (2048U != KEROS_EEPROM_CAPACITY_BYTES)
#error KEROS_EEPROM_CAPACITY_BYTES must be 0 or 2048
#endif

/* 一次厂家库调用中的首个失败，后续成功操作不会覆盖该记录。 */
typedef struct
{
    uint8_t status;       /* 0 为无错误，否则为平台/I2C 状态 */
    uint16_t sub_address; /* 出错操作的芯片子地址 */
    uint8_t is_read;      /* 1 为读取，0 为写入 */
} keros_port_error_t;

/* 每次清除错误后，实际交给 I2C 驱动的调用次数。 */
typedef struct
{
    uint32_t read_calls;
    uint32_t write_calls;
} keros_port_activity_t;
/****************************** Defines ******************************/

/****************************** Declaring ******************************/
/**
 * @brief 获取板级型号配置的 EEPROM 容量，不进行硬件自动识别。
 * @return 配置容量，单位为字节；CK02AT 为 0，2 KB 型号为 2048。
 */
uint32_t KerosPort_GetEepromCapacity(void);

/** @brief 清除上次错误及传输计数；在每次库调用之前使用，不可并发。 */
void KerosPort_ClearError(void);

/**
 * @brief 获取首次错误快照，不清除记录。
 * @return status 为 0 表示无底层错误，其余字段在非零时有效。
 */
keros_port_error_t KerosPort_GetError(void);

/**
 * @brief 获取本次库调用中实际提交给 I2C 驱动的读写次数。
 * @return 计数快照；零次调用不能解释成硬件已拒绝该地址。
 */
keros_port_activity_t KerosPort_GetActivity(void);

/**
 * @brief 按高字节优先的 16 位子地址读取芯片数据。
 * @param[in] sub_address 芯片子地址。
 * @param[in] read_length 读取长度，范围 1~255。
 * @param[out] p_data 接收缓冲区，不得为空。
 * @return 0 为成功，否则为平台/I2C 错误；同时锁存首次失败。
 * @note 包含 EEPROM 地址的真实读请求；失败时清零有效读缓冲区。
 */
uint8_t keros_read_data(uint16_t sub_address, int read_length,
                        uint8_t *p_data);

/**
 * @brief 按高字节优先的 16 位子地址写入芯片数据。
 * @param[in] sub_address 芯片子地址。
 * @param[in] p_data 发送缓冲区，不得为空。
 * @param[in] write_length 写入长度，范围 1~255。
 * @return 0 为成功，否则为平台/I2C 错误；同时锁存首次失败。
 * @note 包含 EEPROM 地址的真实写请求，不按配置伪造访问失败。
 */
uint8_t keros_write_data(uint16_t sub_address, uint8_t *p_data,
                         int write_length);

/**
 * @brief 等待指定毫秒数。
 * @param[in] wait_time 等待时长，单位为毫秒。
 * @note 依赖 HAL 毫秒时基，只允许从中断开启的主循环调用。
 */
void keros_delay(uint32_t wait_time);

/**
 * @brief 保留 ECS100 的两次唤醒地址序列，随后发送 0xF0 命令。
 * @return 最终 Power ON 命令的 I2C 状态，0 为成功。
 */
uint8_t keros_power_on(void);
/****************************** Declaring ******************************/

#ifdef __cplusplus
}
#endif
#endif /* KEROS_INTERFACE_H */
