#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "swd.h"

#include <stdio.h>
#include <string.h>
#include "flash_kinetis.h"

#include "target_image.h"


uint8_t buf[1024];
uint8_t buf2[1024];

static int ShowID(void)
{
    uint32_t id;
    SWJ_ReadDP(DP_IDCODE, &id);
    printf("DP-IDR:0x%X\r\n", id);
    
    SWJ_ReadAP(0x000000FC, &id);
    printf("AHB_AP_IDR:0x%X\r\n", id);

    printf("MEMID:0x%X\r\n", TFlashGetMemID());
  //  printf("SDID:0x%X\r\n", TFlashGetSDID());
    
    return 0;
}


extern const TARGET_FLASH flash;
    
int main(void)
{   
    uint32_t i, err,val;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 10, kGPIO_Mode_OPP);
    
    #ifdef MK22F25612
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    #elif  MK20D5
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    #endif
    
    printf("program for Manley\r\n");

    swd_io_init();
    
    SWJ_InitDebug();
    printf("ShowID\r\n");
    ShowID();
    
    for(i=0;i<sizeof(buf);i++)
    {
        buf[i] = i & 0xFF;
    }

    ShowID();
    

    for(i=0;i<30;i++)
    {
        TFlash_UnlockSequence();
    }
    
    DelayMs(500);
    
    SWJ_WriteAP(0x01000004, 0x00);

    SWJ_SetTargetState(RESET_RUN);
    SWJ_SetTargetState(RESET_PROGRAM);
    
    SWJ_ReadAP(0x01000000, &val);
    
    while(val & (1<<2))
    {
        printf("system in sec!\r\n");
        while(1);
    }
    
    
    
    err = TFlash_Init(&flash);
    printf("TFlash_Init %d\r\n", err);

    uint32_t offset;
    offset = 0;
    
    for(i=0;i<7;i++)
    {
        printf("0x%X\r\n", offset);
        err = target_flash_program_page(&flash, 0x00000000+offset, FileArray, sizeof(buf));
        offset += sizeof(buf);
        if(err)
        {
            printf("target_flash_program_page %d\r\n", err);
        }
    }

    /* release core */
    SWJ_WriteAP(0x01000004, 0x00);
    val = 0;
    SWJ_WriteMem(DBG_EMCR, (uint8_t*)&val, sizeof(val));
    SWJ_SetTargetState(RESET_RUN);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 10);
        DelayMs(500);
    }
}



