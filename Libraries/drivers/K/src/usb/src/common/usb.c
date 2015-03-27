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

uint8_t USB_Init(void)
{
    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    MPU->CESR=0;
    
    /* clock config */
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK;
    uint32_t clock;
    CLOCK_GetClockFrequency(kMCGOutClock, &clock);
    if(USB_SetClockDiv(clock))
    {
        LIB_TRACE("USB clock setup fail\r\n");
        return 1;
    }

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
    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    USB0->USBTRC0 = 0x40; 
    /* enable USB reset IT */
	//设置BDT基址寄存器
	//( 低9 位是默认512 字节的偏移) 512 = 16 * 4 * 8 。
	//8 位表示: 4 个字节的控制状态，4 个字节的缓冲区地址

    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK){};
    
    /* set BDT table address */
	//USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	//USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	//USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);
        
    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;       //D-  D+ 下拉
	USB0->USBTRC0 |= 0x40;                      //强制设置第6位为1  真是纠结，DS上就这么写的

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;


	return 0;
}


void USB_DisConnect(void)
{
	USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void USB_Connect(void)
{
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
}
