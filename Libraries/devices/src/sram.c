

#include "sram.h"


void SRAM_Init(void)
{
    // set SRAM pinMux
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);
    // flexbus clock output(optional, use for debug)
    PORTC->PCR[3] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_CLKOUT
    //control signals
    PORTB->PCR[19] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_OE
    PORTC->PCR[11] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_RW
    PORTD->PCR[0] =  PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // CS1
    PORTC->PCR[16] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_BE_23_16
    PORTC->PCR[17] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_BE_31-24
    // address signal
    PORTD->PCR[10] = PORT_PCR_MUX(6)|PORT_PCR_DSE_MASK;         //  FB_A18 
    PORTD->PCR[9] = PORT_PCR_MUX(6)|PORT_PCR_DSE_MASK;          //  FB_A17 
    PORTD->PCR[8] = PORT_PCR_MUX(6)|PORT_PCR_DSE_MASK;          //  FB_A16
    PORTB->PCR[18] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A15
    PORTC->PCR[0]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A14
    PORTC->PCR[1]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A13
    PORTC->PCR[2]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A12
    PORTC->PCR[4]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A11
    PORTC->PCR[5]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A10
    PORTC->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A9
    PORTC->PCR[7]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A8
    PORTC->PCR[8]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A7
    PORTC->PCR[9]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A6
    PORTC->PCR[10] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A5
    PORTD->PCR[2]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A4
    PORTD->PCR[3]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A3
    PORTD->PCR[4]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A2
    PORTD->PCR[5]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_A1
    // data signal
    PORTB->PCR[17]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD16
    PORTB->PCR[16]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD17
    PORTB->PCR[11]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD18
    PORTB->PCR[10]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD19
    PORTB->PCR[9]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_AD20
    PORTB->PCR[8]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_AD21
    PORTB->PCR[7]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_AD22
    PORTB->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;         //  FB_AD23
    PORTC->PCR[15]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD24
    PORTC->PCR[14]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD25
    PORTC->PCR[13]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD26
    PORTC->PCR[12]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD27
    PORTB->PCR[23]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD28
    PORTB->PCR[22]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD29
    PORTB->PCR[21]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD30
    PORTB->PCR[20]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;        //  FB_AD31 
    // enable flexbus
    FLEXBUS_InitTypeDef FLEXBUS_InitStruct;
    FLEXBUS_InitStruct.ADSpaceMask = kFLEXBUS_ADSpace_512KByte;
    FLEXBUS_InitStruct.autoAckMode = kFLEXBUS_AutoAckEnable;
    FLEXBUS_InitStruct.CSn = kFLEXBUS_CS1;
    FLEXBUS_InitStruct.dataAlignMode = kFLEXBUS_DataLeftAligned;
    FLEXBUS_InitStruct.dataWidth = kFLEXBUS_PortSize_16Bit;
    FLEXBUS_InitStruct.baseAddress = SDRAM_ADDRESS_BASE;
    FLEXBUS_InitStruct.ByteEnableMode = kFLEXBUS_BE_AssertedReadWrite;
    FLEXBUS_InitStruct.CSPortMultiplexingCotrol = FB_CSPMCR_GROUP3(kFLEXBUS_CSPMCR_GROUP3_BE_23_16) | FB_CSPMCR_GROUP2(kFLEXBUS_CSPMCR_GROUP2_BE_31_24) | (FB_CSPMCR_GROUP1(kFLEXBUS_CSPMCR_GROUP1_CS1));
    FLEXBUS_Init(&FLEXBUS_InitStruct);
}



uint32_t SRAM_SelfTest(void)
{
    uint32_t i;
    uint32_t err_cnt = 0;
    char * SRAM_START_ADDR = SRAM_START_ADDRESS;
    for(i = 0; i < SRAM_SIZE; i++)
    {
        SRAM_START_ADDR[i] = i%0xFF;
        if((SRAM_START_ADDR[i]) != (i%0xFF))
        {
            err_cnt++;
        }
    }
    return  err_cnt;
}





