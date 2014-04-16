
#include "flexbus.h"
#include "shell.h"
#include "ili9320.h"
#include "sram.h"
#include "common.h"

int CMD_FLEXBUS(int argc, char * const * argv)
{
    shell_printf("FLEXBUS Test CMD\r\n");
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED FEATURE\r\n");
    
#else
    
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
    PORTD->PCR[1]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // CS0
    //WORK AS LCD RS
    PORTA->PCR[26] = PORT_PCR_MUX(6)|PORT_PCR_DSE_MASK;          // A27
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
    // enable flexbus for SRAM
    FLEXBUS_InitTypeDef FLEXBUS_InitStruct;
    FLEXBUS_InitStruct.ADSpaceMask = kFLEXBUS_ADSpace_512KByte;
    FLEXBUS_InitStruct.autoAckMode = kFLEXBUS_AutoAckEnable;
    FLEXBUS_InitStruct.CSn = kFLEXBUS_CS1;
    FLEXBUS_InitStruct.dataAlignMode = kFLEXBUS_DataLeftAligned;
    FLEXBUS_InitStruct.dataWidth = kFLEXBUS_PortSize_16Bit;
    FLEXBUS_InitStruct.baseAddress = SDRAM_ADDRESS_BASE;
    FLEXBUS_InitStruct.ByteEnableMode = kFLEXBUS_BE_AssertedReadWrite;
    FLEXBUS_Init(&FLEXBUS_InitStruct);
    // enable flexbus for LCD
    FLEXBUS_InitStruct.ADSpaceMask = 0x800;
    FLEXBUS_InitStruct.autoAckMode = kFLEXBUS_AutoAckEnable;
    FLEXBUS_InitStruct.CSn = kFLEXBUS_CS0;
    FLEXBUS_InitStruct.dataAlignMode = kFLEXBUS_DataLeftAligned;
    FLEXBUS_InitStruct.dataWidth = kFLEXBUS_PortSize_16Bit;
    FLEXBUS_InitStruct.baseAddress = ILI9320_BASE;
    FLEXBUS_InitStruct.ByteEnableMode = kFLEXBUS_BE_AssertedWrite;
    FLEXBUS_Init(&FLEXBUS_InitStruct);
    /* config Flexbus SRAM pinmux */
    FLEXBUS_PortMuxConfig(kFLEXBUS_CSPMCR_Group3, kFLEXBUS_CSPMCR_GROUP3_BE_23_16);
    FLEXBUS_PortMuxConfig(kFLEXBUS_CSPMCR_Group2, kFLEXBUS_CSPMCR_GROUP2_BE_31_24);
    FLEXBUS_PortMuxConfig(kFLEXBUS_CSPMCR_Group1, kFLEXBUS_CSPMCR_GROUP1_CS1);
#endif

    return 0;
}



const cmd_tbl_t CommandFun_FLEXBUS = 
{
    .name = "FLEXBUS",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_FLEXBUS,
    .usage = "FLEXBUS TEST",
    .complete = NULL,
    .help = "\r\n"
};
