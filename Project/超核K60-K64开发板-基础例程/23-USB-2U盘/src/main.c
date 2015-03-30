#include "chlib_k.h"

#define UDISK_SIZE          (300*1024)
#define SECTER_SIZE         (0x000800)  /* min erase unit */
#define PROGRAM_PAGE_SIZE   (512)       /* min write unit */
#define FLASH_OFFSET        (64*1024)   /* UDisk start offset */

U8 BlockBuf[SECTER_SIZE];


/* init */
void usbd_msc_init ()
{
  USBD_MSC_MemorySize = UDISK_SIZE;
  USBD_MSC_BlockSize  = SECTER_SIZE;
  USBD_MSC_BlockGroup = 1;
  USBD_MSC_BlockCount = USBD_MSC_MemorySize / USBD_MSC_BlockSize;
  USBD_MSC_BlockBuf   = BlockBuf;

  USBD_MSC_MediaReady = __TRUE;

}

/* read */
void usbd_msc_read_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    uint8_t *p;
    uint8_t i;
    
    p = (uint8_t*)(FLASH_OFFSET + block * USBD_MSC_BlockSize);
    
    if (USBD_MSC_MediaReady)
    {
        for(i=0;i<num_of_blocks;i++)
        {
            memcpy(buf, p, num_of_blocks * USBD_MSC_BlockSize);
            p += USBD_MSC_BlockSize;
        }
    }
}

/* write */
void usbd_msc_write_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    int i,j;
    uint8_t *p;
    
    p = (uint8_t*)(FLASH_OFFSET + block * USBD_MSC_BlockSize);
    
    if (USBD_MSC_MediaReady)
    {
        for(i=0;i<num_of_blocks;i++)
        {
            EraseSector((uint32_t)p);
            
            for(j=0;j<SECTER_SIZE/PROGRAM_PAGE_SIZE;j++)
            {
                ProgramPage((uint32_t)p, PROGRAM_PAGE_SIZE, buf);
                p += PROGRAM_PAGE_SIZE;
                buf += PROGRAM_PAGE_SIZE;
            }
        }
    }
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("usbd hid msc(udisk) demo\r\n");

    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */
    while (!usbd_configured ());          /* Wait for device to configure        */
    
    /* init flash */
    Init(0x00000000, 10000000, 2); 
    
    printf("usbd configure complete\r\n");
    
    while(1)
    {
        DelayMs(500);
    }
}


