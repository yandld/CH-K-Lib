#include <string.h>

#include "virtual_eep.h"
#include "flash.h"



extern int Image$$ER_IROM1$$RO$$Limit;

#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))


    
static uint32_t StartAddr;
    
void veep_init(void)
{
    uint32_t sector_size;
    
    sector_size = FLASH_GetSectorSize();
    StartAddr = RT_ALIGN(((uint32_t)&Image$$ER_IROM1$$RO$$Limit + sector_size), sector_size);
}
    

void veep_write(uint8_t* buf, uint32_t len)
{
    FLASH_EraseSector(StartAddr);
    FLASH_WriteSector(StartAddr, buf, len);
}

void veep_read(uint8_t *buf, uint32_t len)
{
    memcpy(buf, (void*)StartAddr, len);
}



