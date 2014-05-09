/**
  ******************************************************************************
  * @file    sram.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "sram.h"
#include "flexbus.h"
#include "common.h"

//SRAM初始化配置
void SRAM_Init(void)
{
    /* set SRAM pinMux */
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);
    /* flexbus clock output(optional, use for debug) */
    PORTC->PCR[3] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_CLKOUT
    /*control signals */
    PORTB->PCR[19] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_OE
    PORTC->PCR[11] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_RW
    PORTD->PCR[0] =  PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // CS1
    PORTC->PCR[16] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_BE_23_16
    PORTC->PCR[17] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_BE_31-24
    /* address signal */
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
    /* data signal */
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
    /* enable flexbus */
    FLEXBUS_InitTypeDef FLEXBUS_InitStruct;
    FLEXBUS_InitStruct.ADSpaceMask = kFLEXBUS_ADSpace_512KByte; /* 内存地址范围 512K */
    FLEXBUS_InitStruct.autoAckMode = kFLEXBUS_AutoAckEnable; /*启动自动应答 */
    FLEXBUS_InitStruct.CSn = kFLEXBUS_CS1; /*使用CS1片选信号 */
    FLEXBUS_InitStruct.dataAlignMode = kFLEXBUS_DataLeftAligned; /*数据左对齐 */
    FLEXBUS_InitStruct.dataWidth = kFLEXBUS_PortSize_16Bit; /*数据位宽 16位 */
    FLEXBUS_InitStruct.baseAddress = SDRAM_ADDRESS_BASE; /* 基地址 */
    FLEXBUS_InitStruct.ByteEnableMode = kFLEXBUS_BE_AssertedReadWrite; /* 在读写操作的时候都插入 位使能信号 */
    FLEXBUS_InitStruct.div = 0;
    FLEXBUS_Init(&FLEXBUS_InitStruct);
    /* config Flexbus SRAM pinmux */
    FLEXBUS_PortMuxConfig(kFLEXBUS_CSPMCR_Group3, kFLEXBUS_CSPMCR_GROUP3_BE_23_16);
    FLEXBUS_PortMuxConfig(kFLEXBUS_CSPMCR_Group2, kFLEXBUS_CSPMCR_GROUP2_BE_31_24);
    FLEXBUS_PortMuxConfig(kFLEXBUS_CSPMCR_Group1, kFLEXBUS_CSPMCR_GROUP1_CS1);
}

//自测试程序，向sram中写数据，然后读出数据，验证数据的正确性
uint32_t SRAM_SelfTest(void)
{
    uint32_t i;
    uint32_t err_cnt = 0;
    volatile uint8_t * SRAM_START_ADDR = SRAM_START_ADDRESS;
    for(i = 0; i < SRAM_SIZE; i++)
    {
        SRAM_START_ADDR[i] = i%0xFF; //向SRAM指定地址写数据
        if((SRAM_START_ADDR[i]) != (i%0xFF))  //读取SRAM中的指定数据
        {
            err_cnt++;
        }
    }
    return  err_cnt;
}





