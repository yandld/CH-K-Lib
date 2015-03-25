#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "chsw.h"
#include "rl_usb.h"

uint32_t GetUID(void)
{
    
    
    
    
    return (SIM->UIDML) ^ (SIM->UIDL) ^ (SIM->UIDML) ^ (SIM->UIDMH);
        
    
}

int main(void)
{   
    uint32_t tmp, val;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 10, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);

    printf("USB test\r\n");
    printf("chip's UID:0x%08X\r\n", GetUID());
  //  if(USB_ClockInit())
    {
  //      printf("USB  Init failed, clock must be 96M or 48M\r\n");
  //      while(1);
    }
    
  //  usbd_init();                          /* USB Device Initialization          */
  //  usbd_connect(__TRUE);                 /* USB Device Connect                 */

    while(1)
    {
        printf("!!\r\n");
        GPIO_ToggleBit(HW_GPIOC, 10);
        DelayMs(50);
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










