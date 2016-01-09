#ifndef __BL_CFG_H__
#define __BL_CFG_H__

#include "common.h"

/* Base address of user application */
#define APPLICATION_BASE                    0x1000UL

/****************************************************
 *
 *    Target specific attributes
 *
 ****************************************************/
/* Flash total size */
#define TARGET_FLASH_SIZE       64*1024u
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
