#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "usb.h"
#include "usb_hid.h"

static int USB_SetClockDiv(uint32_t srcClock)
{
    uint8_t frac,div;
#ifdef SIM_CLKDIV2_USBDIV
    /* clear all divivder */
    SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBDIV_MASK;
    SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBFRAC_MASK;
    
    for(frac = 0; frac < 2; frac++)
    {
        for(div = 0; div < 8; div++)
        {
            if(((srcClock*(frac+1))/(div+1)) > 47000000 && ((srcClock*(frac+1))/(div+1)) < 49000000)
            {
                SIM->CLKDIV2 |= SIM_CLKDIV2_USBDIV(div);
                (frac)?(SIM->CLKDIV2 |= SIM_CLKDIV2_USBFRAC_MASK):(SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBFRAC_MASK);
                LIB_TRACE("USB clock OK src:%d frac:%d div:%d 0x%08X\r\n", srcClock, frac, div, SIM->CLKDIV2);
                return 0;
            }
        }
    }
#else
    return 0;
#endif
    return 1;
}

uint8_t USB_ClockInit(void)
{
    /* open clock gate */
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK;
    
    /* clock config */
    uint32_t clock;
    clock = GetClock(kMCGOutClock);
    if(USB_SetClockDiv(clock))
    {
        LIB_TRACE("USB clock setup fail\r\n");
        return 1;
    }

    /* which MCG generator is to be used */
    SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
#ifdef SIM_SOPT2_PLLFLLSEL
    (MCG->C6 & MCG_C6_PLLS_MASK)?
    (SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL(1)):   /* PLL */
    (SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL(0));  /* FLL */
#else
    (MCG->C6 & MCG_C6_PLLS_MASK)?
    (SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK):   /* PLL */
    (SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK);  /* FLL */
#endif
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
    USB_Init();
    NVIC_EnableIRQ(USB0_IRQn);
    USBD_HID_Init();
    
      int8_t report[4] = {0, 0, 0, 0};
      
      report[0] = 1;
      report[1] = 1;
      report[2] = 1;
      report[3] = 1;
    while(1)
    {
     //   USB_HID_SetMouse(1,0,0,0);
     //   HID_Proc();
    }
}


