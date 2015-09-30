#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include <stdint.h>
#include "uart.h"
#include "common.h"

typedef struct
{
    /* Flash */
    uint32_t FlashSize;
    uint32_t FlashPageSize;
    uint32_t AppStartAddr;
    uint32_t (*flash_erase)(uint32_t addr);
    uint32_t (*flash_write)(uint32_t addr, const uint8_t *buf, uint32_t len);
    
    /* transmit */
    uint32_t (*send)(uint8_t *buf, uint32_t len);
    uint32_t (*receive)(uint8_t *buf, uint32_t len);
    
    /* settings */
    uint32_t TimeOut;
}BLParam_t;

//!< API 
uint32_t BootloaderInit(uint32_t timeOut);
void BootloaderProc(void);


#endif
