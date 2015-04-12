#ifndef __SWD_H__
#define __SWD_H__

#include <stdint.h>
#include "sw_config.h"
#include "debug_cm.h"

// DAP Transfer Response
#define DAP_TRANSFER_OK                 (1<<0)
#define DAP_TRANSFER_WAIT               (1<<1)
#define DAP_TRANSFER_FAULT              (1<<2)
#define DAP_TRANSFER_ERROR              (1<<3)
#define DAP_TRANSFER_MISMATCH           (1<<4)

// Debug Port Register Addresses
#define DP_IDCODE                       0x00    // IDCODE Register (SW Read only)
#define DP_ABORT                        0x00    // Abort Register (SW Write only)
#define DP_CTRL_STAT                    0x04    // Control & Status
#define DP_WCR                          0x04    // Wire Control Register (SW Only)
#define DP_SELECT                       0x08    // Select Register (JTAG R/W & SW W)
#define DP_RESEND                       0x08    // Resend (SW Read Only)
#define DP_RDBUFF                       0x0C    // Read Buffer (Read Only)

// DAP Transfer Request
#define DAP_TRANSFER_APnDP              (1<<0)
#define DAP_TRANSFER_RnW                (1<<1)
#define DAP_TRANSFER_A2                 (1<<2)
#define DAP_TRANSFER_A3                 (1<<3)
#define DAP_TRANSFER_MATCH_VALUE        (1<<4)
#define DAP_TRANSFER_MATCH_MASK         (1<<5)

// SWD register access
#define SWD_REG_AP        (1)
#define SWD_REG_DP        (0)
#define SWD_REG_R        (1<<1)
#define SWD_REG_W        (0<<1)
#define SWD_REG_ADR(a)    (a & 0x0c)


#define MDM_STATUS  0x01000000
#define MDM_CTRL    0x01000004     //
#define MDM_IDR     0x010000fc     // read-only identification register

#define MCU_ID      0x001c0020

// Debug Select Register definitions
#define CTRLSEL        0x00000001  // CTRLSEL (SW Only)
#define APBANKSEL      0x000000F0  // APBANKSEL Mask
#define APSEL          0xFF000000  // APSEL Mask


#ifndef BSWAP_32
#define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))
#endif

#ifndef BSWAP_16
#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif

#ifndef NULL
#define NULL    0
#endif



//!< API
uint8_t SWJ_ReadDP(uint8_t adr, uint32_t *val);
uint8_t SWJ_WriteDP(uint8_t adr, uint32_t val);
uint8_t SWJ_ReadAP(uint32_t adr, uint32_t *val);
uint8_t SWJ_WriteAP(uint32_t adr, uint32_t val);
uint8_t SWJ_InitDebug(void);
uint8_t SWJ_ReadMem32(uint32_t addr, uint32_t *val);
uint8_t SWJ_WriteMem32(uint32_t addr, uint32_t val);
uint8_t SWJ_ReadMem8(uint32_t addr, uint32_t *val);
uint8_t SWJ_WriteMem8(uint32_t addr, uint32_t val);


#endif
