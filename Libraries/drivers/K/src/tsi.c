/**
  ******************************************************************************
  * @file    tsi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
 #include "tsi.h"
 #include "common.h"
 #include "gpio.h"
 
 static  QuickInit_Type TSI_QuickInitTable[] =
{
    { 0, 1, 0, 0, 1, 0},    //TSI0_CH0_PB00
    { 0, 0, 0, 0, 1, 1},    //TSI0_CH1_PA00
    { 0, 0, 0, 1, 1, 2},    //TSI0_CH2_PA01
    { 0, 0, 0, 2, 1, 3},    //TSI0_CH3_PA02
    { 0, 0, 0, 3, 1, 4},    //TSI0_CH4_PA03
    { 0, 0, 0, 4, 1, 5},    //TSI0_CH5_PA04
    { 0, 1, 0, 1, 1, 6},    //TSI0_CH6_PB01
    { 0, 1, 0, 2, 1, 7},    //TSI0_CH7_PB02
    { 0, 1, 0, 3, 1, 8},    //TSI0_CH8_PB03
    { 0, 1, 0,16, 1, 9},    //TSI0_CH9_PB16
    { 0, 1, 0,17, 1,10},    //TSI0_CH10_PB17
    { 0, 1, 0,18, 1,11},    //TSI0_CH11_PB18
    { 0, 1, 0,19, 1,12},    //TSI0_CH12_PB19
    { 0, 2, 0, 0, 1,13},    //TSI0_CH13_PC00
    { 0, 2, 0, 1, 1,14},    //TSI0_CH14_PC01
    { 0, 2, 0, 2, 1,15},    //TSI0_CH15_PC02
};

void TSI_Init(TSI_InitTypeDef* TSI_InitStruct)
{
	SIM->SCGC5 |= (SIM_SCGC5_TSI_MASK); 
	//每个电极连续扫描11次，prescaler分频8，软件触发
	TSI0->GENCS |= ((TSI_GENCS_NSCN(0x18))|(TSI_GENCS_PS(7))); 
	//配置TSI扫描电流和模式
	TSI0->SCANC &= ~TSI_SCANC_EXTCHRG_MASK; //外部振荡器充电电流 0-31
	TSI0->SCANC |=  TSI_SCANC_EXTCHRG(31);//
	TSI0->SCANC &= ~TSI_SCANC_REFCHRG_MASK; //参考时钟充电电路   0-31
	TSI0->SCANC |=  TSI_SCANC_REFCHRG(31);  //
	TSI0->SCANC &= ~TSI_SCANC_DELVOL_MASK;  //增量电压设定选择
	TSI0->SCANC |=  TSI_SCANC_DELVOL(7);
	//间断扫描模式，输入时钟为Busclock/128
	TSI0->SCANC |= (TSI_SCANC_SMOD(0))|(TSI_SCANC_AMPSC(0));
	//时能对应的TSI 端口 使能时必须先禁止TSI
	TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK; 
	while((TSI0->GENCS & TSI_GENCS_SCNIP_MASK) == 0x01);
	TSI0->PEN |= ((1 << (TSI_InitStruct->chl)));				
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;  
    /* config trigger mode */
    switch(TSI_InitStruct->triggerMode)
    {
        case kTSI_TriggerSoftware:
            TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
            break;
        case kTSI_TriggerPeriodicalScan:
            TSI0->GENCS |= TSI_GENCS_STM_MASK;
            break;
        default:
            break;
    }
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK; //时能TSI
}

uint32_t TSI_QuickInit(uint32_t UARTxMAP)
{
    uint32_t i;
    QuickInit_Type * pTSIxMAP;
    pTSIxMAP = (QuickInit_Type*)&UARTxMAP;
    #if DEBUG
    printf("pTSIxMAP->channel:%d\r\n", pTSIxMAP->channel);
    printf("pTSIxMAP->io_base:%d\r\n", pTSIxMAP->io_base);
    printf("pTSIxMAP->io_instance:%d\r\n", pTSIxMAP->io_instance);
    #endif
    // init pinmux
    for(i = 0; i < pTSIxMAP->io_offset; i++)
    {
        PORT_PinMuxConfig(pTSIxMAP->io_instance, pTSIxMAP->io_base + i, (PORT_PinMux_Type) pTSIxMAP->mux); 
    }
    return pTSIxMAP->ip_instance;
}

