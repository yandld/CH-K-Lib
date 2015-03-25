#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "chsw.h"
#include "rl_usb.h"
#include "FlashOS.h"
#include "iflash.h"

/* disable WDOG */
void DisableWDT(void)
{
  WDOG->UNLOCK = 0xC520;
  WDOG->UNLOCK = 0xD928;
  WDOG->STCTRLH = 0;
}

/* get UID */
uint32_t GetUID(void)
{
    return (SIM->UIDML) ^ (SIM->UIDL) ^ (SIM->UIDML) ^ (SIM->UIDMH);
}


/* get  residue flash size */
#define TEST_ADDR_BEIGN  (2048*80)


static int MKP512FlashInit(void)
{
    uint32_t clock;
    uint32_t flash_clock = CLOCK_GetClockFrequency(kFlashClock, &clock);
    /* fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify) */

    /* func:Init is SSD API */    
    return Init(0x00000000, clock, 2); 
}

static void FlashTest(void)
{
    int secNo, i,err;
    uint8_t *p;
    char buf[SECTOR_SIZE];
    
    for(i=0;i<SECTOR_SIZE;i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for (secNo = 10; secNo < (DEV_SIZE/SECTOR_SIZE); secNo++)
    {
        printf("program sec:%d ", secNo);
        
        err = 0;
        err += FLASH_EraseSector(secNo);
        err += FLASH_WriteSector(secNo, SECTOR_SIZE, buf);
        if(err)
        {
            printf("issue command failed\r\n");
            return;
        }
        p = (uint8_t*)(secNo*SECTOR_SIZE);
        
        for(i=0;i<SECTOR_SIZE;i++)
        {
            if(*p++ != (i%0xFF))
            {
                err++;
                printf("[%d]:0x%02X ", i, *p);
            }
        }
        if(!err)
        {
            printf("verify OK\r\n");
        }
        else
        {
            printf("verify ERR\r\n");
        }
    }
}


int main(void)
{   
    int r;
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 10, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    
    uint32_t clock;

    printf("program for Manley\r\n");
    printf("this program manly display 3 features:\r\n");
    printf("1. read chip's uid\r\n");
    
    /* phase1: UID */
    printf("chip's UID:0x%08X\r\n", GetUID());
    
    /* phase2: falsh demo */
    printf("2. flash prgram\r\n");
    FlashTest();
//    
//    r = MKP512FlashInit();
//    if(r)
//    {
//        printf("flash init failed\r\n");
//    }
//    printf("flash test start addr:0x%08X size:0x%08X\r\n", TEST_ADDR_BEIGN, sizeof(buf));
//    
//    uint8_t *p;
//    
//    if(FLASH_EraseSector(TEST_ADDR_BEIGN/2048))
//    {
//        printf("erase error \r\n");
//   //     while(1);
//    }

//    for(i=0;i<SECTOR_SIZE;i++)
//    {
//        buf[i] = i%0xFF;
//    }
//    if(FLASH_WriteSector(TEST_ADDR_BEIGN/2048, 2048, buf))
//    {
//        printf("FLASH_WriteSector ERR\r\n");
//     //   while(1);
//    }
//    
//    /* verify */
//    p = (uint8_t*)TEST_ADDR_BEIGN;
//    for(i=0;i<SECTOR_SIZE;i++)
//    {
//        if(*p++ != (i%0xFF))
//        {
//            printf("[%d]:0x%02X ", i, *p++);
//        }
//    }
//    
//    
//    /* write test data */
//    for(i=0;i<SECTOR_SIZE;i++)
//    {
//        buf[i] = i%0xFF;
//    }

//    /* ersase and program */

//    EraseSector(TEST_ADDR_BEIGN);
//    printf("EraseSector takes %d ticks\r\n", ticks);
//    
//    /* when program flash, we must erase it first */
//    r = ProgramPage(TEST_ADDR_BEIGN, sizeof(buf), buf);
//    printf("ProgramPage takes %d ticks\r\n", ticks);
//    
//    if(r)
//    {
//        printf("program page failed\r\n");
//    }
//    
//    /* verify */
//    p = (uint8_t*)TEST_ADDR_BEIGN;
//    for(i=0;i<sizeof(buf);i++)
//    {
//        if(*p++ != (i%0xFF))
//        {
//            printf("2error:0x%02X ", *p++);
//        }
//    }
//    printf("flash test completed!\r\n");
//    
    


    
    printf("3. USB buck in and bulk out\r\n");

    if(USB_ClockInit())
    {
        printf("USB  Init failed, clock must be 96M or 48M\r\n");
        while(1);
    }
    
    usbd_init();                          /* USB Device Initialization          */
 //   usbd_connect(__TRUE);                 /* USB Device Connect                 */

    while(1)
    {
     //   printf("loop\r\n");
        GPIO_ToggleBit(HW_GPIOC, 10);
        DelayMs(500);
    }
//    CHSW_IOInit();

//    swd_init_debug();
//    
//    swd_read_idcode(&val);
//    printf("DP_IDR:0x%X\r\n", val);
//    
//
//
//    int res;
//    
//    swd_read_ap(MDM_IDR, &val);
//    printf("val:0x%X\r\n", val);
//   // res = swd_write_ap(MDM_CTRL, 0x55);
//  //  printf("res:%d\r\n", res);
//    swd_read_ap(MDM_CTRL, &val);
//    printf("val:0x%X\r\n", val);
//    
}










