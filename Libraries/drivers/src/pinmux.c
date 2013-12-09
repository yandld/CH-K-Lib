#include "pinmux.h"
#include "common.h"





/**
  * @brief  PinMux config, many peripheral need to call this function to finish pinmux control
  * @param  GPIOIndex:
            @arg 0: stand for PTA
            @arg 1: stand for PTB
            @arg 2: stand for PTC
            @arg 3: stand for PTD
            @arg 4: stand for PTE
  * @param  PinIndex: 0 - 32
  * @param  MuxIndex: 0 - 7
  * @retval None
  */

void PinMuxConfig(uint8_t GPIOIndex, uint8_t pinIndex, PINMUX_Alt_Type muxIndex)
{
    switch(GPIOIndex)
    {
        case 0:
            SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
            PORTA->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);    
            PORTA->PCR[pinIndex] |= PORT_PCR_MUX(muxIndex); 
            break;
        case 1:
            SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
            PORTB->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);    
            PORTB->PCR[pinIndex] |= PORT_PCR_MUX(muxIndex); 
            break;
        case 2:
            SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
            PORTC->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);    
            PORTC->PCR[pinIndex] |= PORT_PCR_MUX(muxIndex); 
            break;
        case 3:
            SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
            PORTD->PCR[pinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTD->PCR[pinIndex]|= PORT_PCR_MUX(muxIndex); 
            break;
        case 4:
            SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
            PORTE->PCR[pinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTE->PCR[pinIndex]|= PORT_PCR_MUX(muxIndex); 
            break;
        default:
            break;
    }
}















