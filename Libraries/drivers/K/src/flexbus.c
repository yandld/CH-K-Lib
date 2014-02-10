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

void FLEXBUS_Init(FLEXBUS_InitTypeDef* FLEXBUS_InitStruct)
{
    uint32_t i;
    //enable clock gate enable seruriy mode
    SIM->SOPT2 |= SIM_SOPT2_FBSL(3);
    SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    //AutoAck(AA) Config
    if(FLEXBUS_InitStruct->autoAckMode == kFLEXBUS_AutoAckEnable)
    {
        FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSCR |= FB_CSCR_AA_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSCR &= ~FB_CSCR_AA_MASK;
    }
    //address space
    i = (FLEXBUS_InitStruct->addressSpaceInByte - 1 - 0xFFFF)>>16;
    FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSMR = FB_CSMR_BAM(i);
    //port size
    FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSCR &= FB_CSCR_PS_MASK;
    FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSCR |= FB_CSCR_PS(FLEXBUS_InitStruct->portSize);
    // data align
    if(FLEXBUS_InitStruct->dataAlignMode == kFLEXBUS_DataLeftAligned)
    {
        FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSCR &= ~FB_CSCR_BLS_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->chipSelectChl].CSCR |= FB_CSCR_BLS_MASK;
    }
    printf("i:%d\r\n" ,i);
    /*
  //???????????????
  addr_space = (addr_space-1)>>16;
  FB->CS[Fbx].CSMR = FB_CSMR_BAM(addr_space)  //????????128K???? =2^n(n=BAM+16)
                | FB_CSMR_V_MASK;    //??CS??
                */
    
}





















