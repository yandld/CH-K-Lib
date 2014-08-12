#include "shell.h"
#include "gpio.h"

/*
000 FlexBus CLKOUT
001 Reserved
010 Flash clock
011 LPO clock (1 kHz)
100 MCGIRCLK
101 RTC 32.768kHz clock
110 OSCERCLK0
111 IRC 48 MHz clock
*/

int DoCLKOUT(int argc, char * const argv[])
{
    int i;
    SIM->SOPT2 &= ~SIM_SOPT2_CLKOUTSEL_MASK;
    SIM->SOPT2 |= SIM_SOPT2_CLKOUTSEL(6);
    
    PORT_PinMuxConfig(HW_GPIOC, 3, kPinAlt5);
    
    MCG->C2 &= ~MCG_C2_EREFS0_MASK;
    OSC->CR |= OSC_CR_ERCLKEN_MASK;
}

SHELL_EXPORT_CMD(DoCLKOUT, clkout , clkout test)
    

