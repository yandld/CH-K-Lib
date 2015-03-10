#include "usb.h"
#include "common.h"

static int USB_SetClockDiv(uint32_t srcClock)
{
    uint8_t frac,div;
    
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
    return 1;
}


uint8_t USB_ClockInit(void)
{
    /* open clock gate */
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK;
    
    /* clock config */
    uint32_t clock;
    CLOCK_GetClockFrequency(kMCGOutClock, &clock);
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


