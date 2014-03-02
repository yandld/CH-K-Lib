/**
  ******************************************************************************
  * @file    flexbus.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "flexbus.h"
#include "common.h"

#if (!defined(MK10D5))

void FLEXBUS_Init(FLEXBUS_InitTypeDef* FLEXBUS_InitStruct)
{
    // enable clock gate enable seruriy mode
    SIM->SOPT2 |= SIM_SOPT2_FBSL(3);
    SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    // we must set V_MASK in CS0, because CS0.CSMR.V_MASK act as a global CS
    FB->CS[0].CSMR |= FB_CSMR_V_MASK; 
    // clear registers
    FB->CS[FLEXBUS_InitStruct->CSn].CSCR = 0;
    // base address
    FB->CS[FLEXBUS_InitStruct->CSn].CSAR = FLEXBUS_InitStruct->baseAddress;
    // address space
    FB->CS[FLEXBUS_InitStruct->CSn].CSMR = FB_CSMR_BAM(FLEXBUS_InitStruct->ADSpaceMask) | FB_CSMR_V_MASK;
    //port size
    FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= FB_CSCR_PS_MASK;
    FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_PS(FLEXBUS_InitStruct->dataWidth);
    // AutoAcknogement(AA) Config
    if(FLEXBUS_InitStruct->autoAckMode == kFLEXBUS_AutoAckEnable)
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_AA_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_AA_MASK;
    }
    // data align
    if(FLEXBUS_InitStruct->dataAlignMode == kFLEXBUS_DataLeftAligned)
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_BLS_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_BLS_MASK;
    }
    // byte enable mode
    if(FLEXBUS_InitStruct->ByteEnableMode == kFLEXBUS_BE_AssertedWrite)
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_BEM_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_BEM_MASK;
    }
    // CS Port Multiplexing Cotrol
    FB->CSPMCR = FLEXBUS_InitStruct->CSPortMultiplexingCotrol;
}

#endif



















