#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"

#include <rl_usb.h>




/*
 *  Get Mouse Input Report -> MouseInReport
 *    Parameters:      report:
 *                               Byte0.0: 1st Button (Left)
 *                               Byte0.1: 2nd Button (Right)
 *                               Byte0.2: 3rd Button
 *                               Byte1:   Relative X Pos
 *                               Byte2:   Relative Y Pos
 *                               Byte3:   Relative Wheel Pos
 *                     size:     report size
 *    Return Value:    None
 */
void GetMouseInReport (S8 *report, U32 size) {
  U32 key;

    report[0] = 0;
    report[1] = 1;
    report[2] = 0;
    report[3] = 0;
}

int main(void)
{
    S8 report[4] = {0, 0, 0, 0};
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    #ifdef FRDM
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    #else
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    #endif
    
    printf("USB test\r\n");

    
    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */

    while(1)
    {
        GetMouseInReport(report, 4);
        usbd_hid_get_report_trigger(0, (U8 *)report, 4);
        DelayMs(10);
    }
}


