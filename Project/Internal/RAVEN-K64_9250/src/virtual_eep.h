#ifndef _VEEP_H_
#define _VEEP_H_

#include <stdint.h>



void veep_init(void);
void veep_write(uint8_t *buf, uint32_t len);
void veep_read(uint8_t *buf, uint32_t len);


#endif

