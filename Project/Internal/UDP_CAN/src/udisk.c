#include "flash.h"
#include "rl_usb.h"

extern int Image$$ER_IROM1$$RO$$Limit;

#ifndef RT_ALIGN
#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#endif

static uint32_t StartAddr;

#define UDISK_SIZE          (100*1024)


U8 BlockBuf[4096];

/* init */
void usbd_msc_init ()
{
    USBD_MSC_MemorySize = UDISK_SIZE;
    USBD_MSC_BlockSize  = FLASH_GetSectorSize();
    USBD_MSC_BlockGroup = sizeof(BlockBuf) / USBD_MSC_BlockSize;
    USBD_MSC_BlockCount = USBD_MSC_MemorySize / USBD_MSC_BlockSize;
    USBD_MSC_BlockBuf   = BlockBuf;

    USBD_MSC_MediaReady = __TRUE;
    StartAddr = RT_ALIGN(((uint32_t)&Image$$ER_IROM1$$RO$$Limit + USBD_MSC_BlockSize), USBD_MSC_BlockSize);
}

/* read */
void usbd_msc_read_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    uint8_t *p;
    uint8_t i;
    
    p = (uint8_t*)(StartAddr + block * USBD_MSC_BlockSize);
    
    if (USBD_MSC_MediaReady)
    {
        for(i=0;i<num_of_blocks;i++)
        {
            memcpy(buf, p, USBD_MSC_BlockSize);
            p += USBD_MSC_BlockSize;
        }
    }
}

/* write */
void usbd_msc_write_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    int i;
    uint8_t *p;
    
    p = (uint8_t*)(StartAddr + block * USBD_MSC_BlockSize);
    
    if (USBD_MSC_MediaReady)
    {
        for(i=0;i<num_of_blocks;i++)
        {
            FLASH_EraseSector((uint32_t)p);
            FLASH_WriteSector((uint32_t)p, buf, USBD_MSC_BlockSize);
            p += USBD_MSC_BlockSize;
        }
    }
}
