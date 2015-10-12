#ifndef __BOOTLOADER_UTIL_H__
#define __BOOTLOADER_UTIL_H__

#include <stdint.h>
#include <stdbool.h>

#define CMD_SERIALPORT      (0xFD)
#define CMD_CHIPINFO        (0x80)
#define CMD_APP_INFO        (0x81)
#define CMD_TRANS_DATA      (0x82)
#define CMD_VERIFICATION    (0x83)
#define CMD_APP_CHECK       (0x84)
#define CMD_SOFT_RESET      (0x85)


/* 运行参数 */
typedef struct
{
    uint32_t write_addr;
    volatile uint32_t timeout;
    uint8_t  currentPkgNo;
    uint8_t  op_state;
    bool  IsBootMode;
    uint8_t  retryCnt;
} RunType_t;


extern RunType_t MainControl;

//!< API functions
void SendResp(uint8_t* content, uint8_t cipherFlg,  uint16_t len);


#endif
