#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"
#include "flash.h"
#include "rl_usb.h"


#define UDISK_SIZE          (32*1024)
#define UDISK_START_ADDR    (32*1024)


static uint32_t StartAddr;

U8 BlockBuf[4096];

/* init */
void usbd_msc_init ()
{
    USBD_MSC_MemorySize = UDISK_SIZE;
    USBD_MSC_BlockSize  = FLASH_GetSectorSize();
    USBD_MSC_BlockGroup = 1;
    USBD_MSC_BlockCount = USBD_MSC_MemorySize / USBD_MSC_BlockSize;
    USBD_MSC_BlockBuf   = BlockBuf;

    USBD_MSC_MediaReady = __TRUE;
    StartAddr = UDISK_START_ADDR;
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
            memcpy(buf, p, num_of_blocks * USBD_MSC_BlockSize);
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
            FLASH_WriteSector((uint32_t)p, buf, FLASH_GetSectorSize());
        }
    }
}

int cmd_usb(int argc, char * const argv[])
{
    printf("usbd hid msc(udisk) demo\r\n");

    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */
    while (!usbd_configured ());          /* Wait for device to configure        */
    
    printf("usbd configure complete\r\n");
    return 0;
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    printf("9-USB demo\r\n");

    cmd_usb(0, NULL);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(500);
    }
}




