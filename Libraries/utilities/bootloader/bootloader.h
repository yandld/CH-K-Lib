#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include <stdint.h>

#define BL_FLASH_OK     (0x00)
#define BL_FLASH_ERR    (0x01)

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
    
    /* settings */
    uint32_t TimeOut;
    const char *name;
}Boot_t;

//!< API 
uint32_t BootloaderInit(Boot_t* boot);
void BootloaderProc(void);
void GetData(uint8_t data);

#endif
