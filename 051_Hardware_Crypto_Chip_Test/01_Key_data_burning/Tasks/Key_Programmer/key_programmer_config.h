/**
 * @file key_programmer_config.h
 * @brief Application key/IV and EEPROM settings for the teacher provisioning flow.
 *
 * This flow requires a KEROS model with user EEPROM. CK02AT has no user EEPROM;
 * enabling the flow does not add that hardware capability.
 */
#ifndef KEY_PROGRAMMER_CONFIG_H
#define KEY_PROGRAMMER_CONFIG_H

/* 0: authenticate, program and verify EEPROM; 1: retained read-only diagnosis. */
#ifndef KEY_PROGRAMMER_DIAGNOSTIC_MODE
#define KEY_PROGRAMMER_DIAGNOSTIC_MODE 0
#endif

/* Keep page 0 and its default password compatible with the teacher Boot code. */
#define KEY_PROGRAMMER_EEPROM_PAGE       0U
#define KEY_PROGRAMMER_EEPROM_PASSWORD   0x00000000U
#define KEY_PROGRAMMER_POWER_DELAY_MS   2U
#define KEY_PROGRAMMER_WRITE_DELAY_MS   5U

/* Teacher demonstration values: 16-byte IV and 32-byte application AES key.
 * These are separate from the factory authentication material in the SDK.
 */
#define KEY_PROGRAMMER_IV_BYTES { \
    0x31, 0x32, 0x31, 0x32, 0x31, 0x32, 0x31, 0x32, \
    0x31, 0x32, 0x31, 0x32, 0x31, 0x32, 0x31, 0x32  \
}

#define KEY_PROGRAMMER_AES_KEY_BYTES { \
    0x31, 0x32, 0x31, 0x32, 0x31, 0x32, 0x31, 0x32, \
    0x31, 0x32, 0x31, 0x32, 0x31, 0x32, 0x31, 0x32, \
    0x31, 0x32, 0x31, 0x32, 0x31, 0x32, 0x31, 0x32, \
    0x31, 0x32, 0x31, 0x32, 0x31, 0x32, 0x31, 0x32  \
}

#endif /* KEY_PROGRAMMER_CONFIG_H */
