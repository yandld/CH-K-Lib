#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "dma.h"
#include "pit.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "adc.h"
#include "flash.h"


#include "board.h"

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

static uint8_t buf[1024*4];
int main(void)
{
    uint32_t i, val, time;
    DelayInit();
    GPIO_Init(HW_GPIOC, PIN3, kGPIO_Mode_OPP);

    UART_Init(BOARD_UART_DEBUG_MAP, 115200);
    
    PIT_Init();
    PIT_SetIntMode(0, true);
    PIT_SetTime(0, 1000);
    
    
    printf("HelloWorld\r\n");
  //  FLASH_Test(6*1024, 130*1024);
    printf("flash test OK!\r\n");
//    if(USB_ClockInit())
//    {
//        printf("USB  Init failed, clock must be 96M or 48M\r\n");
//        while(1);
//    }
//    USB_Init();
//    NVIC_EnableIRQ(USB0_IRQn);
//    USBD_HID_Init();
 //   memset(buf, '\r', sizeof(buf));
    ADC_Init(ADC0_SE3A_PE22, kADC_SpeedLow);
    //ADC_SetIntMode(HW_ADC0, true);
    ADC_SetTrigMode(HW_ADC0, TrigSoft);
    ADC_SetAveMode(HW_ADC0, kADC_Ave32);
    while(1)
    {
        PIT_SetTime(0, 500*1000);
        time = PIT_GetCnt(0);
        val = ADC_SoftRead(HW_ADC0, 3);
        time = time - PIT_GetCnt(0);
        printf("val:%d %d\r\n", val, time);
        DelayMs(30);
       // UART_DMASend(HW_UART0, 0, "12345678", 8);
       // while(UART_DMAGetRemain(HW_UART0) != 0);
       // printf("Succ!\r\n");
    }
}

void ADC0_IRQHandler(void)
{
    uint32_t val;
    val = ADC0->R[0];
   // printf("!\r\n");
}

void PIT_IRQHandler(void)
{
    if(PIT->CHANNEL[0].TFLG)
    {
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
       // printf("!!\r\n");
    }
}

