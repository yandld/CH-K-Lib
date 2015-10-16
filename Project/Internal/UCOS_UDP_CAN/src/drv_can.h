#ifndef __CH_DRV_CAN_H__
#define __CH_DRV_CAN_H__

#include <stdint.h>

uint32_t OSCAN_Init(void);
uint32_t OSCAN_Send(uint32_t instance, uint32_t id, uint8_t *buf, uint32_t len);




#endif
