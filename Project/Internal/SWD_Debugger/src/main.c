#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "swd.h"

#include <stdio.h>
#include <string.h>
#include "target_flash.h"

#include "target_image.h"

#define SIM_SDID_BASE       (0x40048024)


uint8_t buf[4*1024];




int main(void)
{   
    uint32_t i, id;
    uint8_t err, DP_ID;
    
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
    
    SWJ_ReadDP(DP_IDCODE, &id);
    printf("DP-IDR:0x%X\r\n", id);

    SWJ_ReadAP(MDM_IDR, &id);
    printf("AHB_AP_IDR:0x%X\r\n", id);
    
    SWJ_ReadAP(0x000000FC, &id);
    printf("APB_AP_IDR:0x%X\r\n", id);
    
    err = 0;
    for(i=0;i<sizeof(buf);i++)
    {
        buf[i] = i & 0xFF;
    }
    
    SWJ_WriteMem(0x20000011, buf, sizeof(buf));
    memset(buf, sizeof(buf), 0);
    SWJ_ReadMem(0x20000011, buf, sizeof(buf));

    for(i=0;i<sizeof(buf);i++)
    {
        if(buf[i] != (i & 0xFF))
        {
            printf("buf[%d]:%d\r\n", i, buf[i]);
        }
    }
    
    SWJ_SetTargetState(RESET_PROGRAM);
    
    printf("test complete\r\n");
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 10);
        DelayMs(500);
    }
    
}



