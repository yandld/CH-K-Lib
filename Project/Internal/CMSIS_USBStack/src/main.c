#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"


uint8_t USB_Init(void)
{
    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    MPU->CESR=0;

    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK){};

    /* BDT adddress */

    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;
	USB0->USBTRC0 |= 0x40;

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
    
    
	return 0;
}

int main(void)
{
    uint8_t buf[4];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    #ifdef FRDM
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    #else
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    #endif
    
    printf("HelloWorld\r\n");
    if(USB_ClockInit())
    {
        printf("USB  Init failed, clock must be 96M or 48M\r\n");
        while(1);
    }
 
    while(1)
    {
    //    USB_HID_SetMouse(1,0,0,0);
    //    HID_Proc();
    }
}


