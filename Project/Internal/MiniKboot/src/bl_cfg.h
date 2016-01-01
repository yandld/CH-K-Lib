#ifndef __BL_CFG_H__
#define __BL_CFG_H__

#include "common.h"

/* Base address of user application */
#define APPLICATION_BASE                    0x5000UL
/* UART Buadrate */
#define BL_UART_BAUDRATE                    115200

/****************************************************
 *
 *    Target specific attributes
 *
 ****************************************************/
/* Flash sector size */
#define TARGET_FLASH_SECTOR_SIZE    4096
/* Flash total size */
#define TARGET_FLASH_SIZE       1024*1024u
/* SRAM start address */
#define TARGET_RAM_START        0x1FFF0000
/* SRAM total size */
#define TARGET_RAM_SIZE         0x40000


/****************************************************
 *
 *    Bootloader features
 *
 ****************************************************/
#define BL_FEATURE_VERIFY       0
#define BL_FEATURE_APP_CHECK    0
#define BL_FEATURE_FLASH_SECURITY_DISABLE   0

#endif // __BL_CFG_H__
