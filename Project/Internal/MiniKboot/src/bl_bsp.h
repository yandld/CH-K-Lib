#ifndef __BL_BSP_H__
#define __BL_BSP_H__

#include <stdint.h>
#include <stdbool.h>


void bl_hw_init(void);
void bl_hw_deinit(void);
void bl_hw_interface_init(void);
void bl_hw_interface_deinit(void);

void bl_hw_if_write(const uint8_t *buffer, uint32_t length);

bool stay_in_bootloader(void);




#endif // __BL_BSP_H__
