#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "chsw.h"
#include "rl_usb.h"
#include "FlashOS.h"
#include "iflash.h"

#include <stdio.h>

int main(void)
{   
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 10, kGPIO_Mode_OPP);
    
    #ifdef MK22F25612
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    #elif  MK20D5
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    #endif
    

    printf("program for Manley\r\n");
    printf("this program manly display 3 features:\r\n");


    uint32_t val;
    
    swd_io_init();

    swd_init_debug();
    
    swd_read_idcode(&val);
    printf("DP_IDR:0x%X\r\n", val);
    
    while(1)
    {
     //   printf("loop\r\n");
        GPIO_ToggleBit(HW_GPIOC, 10);
        DelayMs(500);
    }
    
    

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










