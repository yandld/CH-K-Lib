/**
  ******************************************************************************
  * @file    ftm.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "ftm.h"
#include "clock.h"
#include "gpio.h"
#include "math.h"

//!< Leagacy Support for Kineis Z Version(Inital Version)
#if (!defined(FTM_BASES))

    #if (defined(MK60DZ10))
        #define FTM_BASES {FTM0, FTM1, FTM2}
    #endif
    #if (defined(MK10D5))
        #define FTM_BASES {FTM0, FTM1}
    #endif
#endif

FTM_Type * const FTM_InstanceTable[] = FTM_BASES;

#if (defined(MK60DZ10) || defined(MK40D10) || defined(MK60D10)|| defined(MK10D10) || defined(MK70F12) || defined(MK70F15))
static const RegisterManipulation_Type SIM_FTMClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM0_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM1_MASK},
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FTM2_MASK},
};
#elif (defined(MK10D5))
static const RegisterManipulation_Type SIM_FTMClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM0_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM1_MASK},
};

#endif

//!< static functions declareation
static void FTM_SetMode(uint8_t instance, uint8_t chl, FTM_Mode_Type mode);

/**
 * @brief  初始化FTM模块 .在使用FTM前一定要调用此函数
 *
 * @param  FTM 初始化结构体
 * @retval None
 */
void FTM_Init(FTM_InitTypeDef* FTM_InitStruct)
{
    uint32_t pres;
    uint8_t ps;
    uint32_t i;
    uint32_t input_clk;
    uint32_t min_val = 0xFFFF;
    // enable clock gate
    uint32_t * SIM_SCGx = (void*) SIM_FTMClockGateTable[FTM_InitStruct->instance].register_addr;
    *SIM_SCGx |= SIM_FTMClockGateTable[FTM_InitStruct->instance].mask;

    //disable FTM, we must set CLKS(0) before config FTM!
    FTM_InstanceTable[FTM_InitStruct->instance]->SC = 0;
   // FTM_InstanceTable[FTM_InitStruct->instance]->SC |= FTM_SC_CLKS(0);
    
    // enable to access all register including enhancecd register(FTMEN bit control whather can access FTM enhanced function)
    FTM_InstanceTable[FTM_InitStruct->instance]->MODE |= FTM_MODE_WPDIS_MASK;
    
    // cal ps
    CLOCK_GetClockFrequency(kBusClock, &input_clk);
    pres = (input_clk/FTM_InitStruct->frequencyInHZ)/FTM_MOD_MOD_MASK;
    if((input_clk/FTM_InitStruct->frequencyInHZ)/pres > FTM_MOD_MOD_MASK)
    {
        pres++;
    }
    for(i = 0; i < 7; i++)
    {
        if((abs(pres - (1<<i))) < min_val)
        {
            ps = i;
            min_val = abs(pres - (1<<i));
        }
    }
    if(pres > (1<<ps)) ps++;
    if(ps > 7) ps = 7;
    printf("freq:%dHz\r\n", FTM_InitStruct->frequencyInHZ);
    printf("input_clk:%d\r\n", input_clk);
    printf("pres:%d\r\n", pres);

    //set CNT and CNTIN
    FTM_InstanceTable[FTM_InitStruct->instance]->CNT = 0;
    FTM_InstanceTable[FTM_InitStruct->instance]->CNTIN = 0;
    
    // set modulo
    FTM_InstanceTable[FTM_InitStruct->instance]->MOD = (input_clk/(1<<ps))/FTM_InitStruct->frequencyInHZ;

    // set LOCK bit to load MOD value
    FTM_InstanceTable[FTM_InitStruct->instance]->PWMLOAD = 0xFFFFFFFF;
    
    printf("MOD Should be:%d\r\n",  (input_clk/(1<<ps))/FTM_InitStruct->frequencyInHZ);
    printf("MOD acutall is:%d\r\n", FTM_InstanceTable[FTM_InitStruct->instance]->MOD);
    printf("ps:%d\r\n", ps);

    // set FTM clock to system clock
    FTM_InstanceTable[FTM_InitStruct->instance]->SC &= ~FTM_SC_CLKS_MASK;
    FTM_InstanceTable[FTM_InitStruct->instance]->SC |= FTM_SC_CLKS(1);
    
    // set ps, this must be done after set modulo
    FTM_InstanceTable[FTM_InitStruct->instance]->SC &= ~FTM_SC_PS_MASK;
    FTM_InstanceTable[FTM_InitStruct->instance]->SC |= FTM_SC_PS(ps); 
    
    // set FTM mode
    FTM_SetMode(FTM_InitStruct->instance, FTM_InitStruct->chl, FTM_InitStruct->mode);
}


/*combine channel control*/
/*dual capture control*/
static uint32_t get_channel_pair_index(uint8_t channel)
{
    if((channel == HW_FTM_CH0) || (channel == HW_FTM_CH1))
    {
        return 0;
    }
    else if((channel == HW_FTM_CH2) || (channel == HW_FTM_CH3)) 
    {
        return 1;
    }
    else if((channel == HW_FTM_CH4) || (channel == HW_FTM_CH5)) 
    {
        return 2;
    }
    else 
    {
        return 3;
    }
}


/*!
 * @brief enable FTM peripheral timer channel pair output combine mode.
 * @param instance The FTM peripheral instance number.
 * @param channel  The FTM peripheral channel number.
 * @param enable  true to enable channle pair to combine, false to disable.
 */
#define FTM_COMBINE_CHAN_CTRL_WIDTH  (8)
static void FTM_DualChlConfig(uint8_t instance, uint8_t chl, FTM_DualChlConfig_Type mode, FunctionalState newState)
{
    uint32_t mask;
    switch(mode)
    {
        case kFTM_Combine:
            mask = FTM_COMBINE_COMBINE0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;
        case kFTM_Complementary:
            mask = FTM_COMBINE_COMP0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;    
        case kFTM_DualEdgeCapture:
            mask = FTM_COMBINE_DECAPEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;           
        case kFTM_DeadTime:
            mask = FTM_COMBINE_DTEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;    
        case kFTM_Sync:
            mask = FTM_COMBINE_SYNCEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;   
        case kFTM_FaultControl:
            mask = FTM_COMBINE_FAULTEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;    
        default:
            break;
    } 
    (newState == ENABLE)?(FTM_InstanceTable[instance]->COMBINE |= mask):(FTM_InstanceTable[instance]->COMBINE &= ~mask);
    printf("COMBINE:0x%x\r\n", FTM_InstanceTable[instance]->COMBINE);
}

//!< 设置FTM 工作模式
static void FTM_SetMode(uint8_t instance, uint8_t chl, FTM_Mode_Type mode)
{
    switch(mode)
    {
        case kPWM_EdgeAligned:
            
            FTM_InstanceTable[instance]->MODE &= ~FTM_MODE_FTMEN_MASK;
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK; 
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= FTM_CnSC_ELSB_MASK;
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_DualChlConfig(instance, chl, kFTM_Combine, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            break;
            
        case kPWM_CenterAligned:
            FTM_InstanceTable[instance]->MODE &= ~FTM_MODE_FTMEN_MASK;
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC |= FTM_SC_CPWMS_MASK;  
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_DualChlConfig(instance, chl, kFTM_Combine, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            break;
            
        case kInputCaptureFallingEdge:   
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~(FTM_CnSC_ELSB_MASK);
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_ELSA_MASK);
            
        case kInputCaptureRisingEdge: 
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_ELSB_MASK);
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~(FTM_CnSC_ELSA_MASK);
            
        case kInputCaptureBothEdge:  
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_ELSB_MASK|FTM_CnSC_ELSA_MASK);
            /* all configuration on input capture*/
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->MODE &= ~FTM_MODE_FTMEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK; 
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_DualChlConfig(instance, chl, kFTM_Combine, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            break;
            
        case kPWM_Combine:
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK;
            
            FTM_InstanceTable[instance]->MODE |= FTM_MODE_WPDIS_MASK;
            FTM_InstanceTable[instance]->MODE |= FTM_MODE_FTMEN_MASK;
            FTM_DualChlConfig(instance, chl, kFTM_Combine, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= FTM_CnSC_ELSB_MASK;
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_InstanceTable[instance]->SYNC = FTM_SYNC_CNTMIN_MASK|FTM_SYNC_CNTMAX_MASK;
			FTM_InstanceTable[instance]->SYNC |= FTM_SYNC_SWSYNC_MASK;
            break;
            
        case kPWM_Complementary:
                FTM_InstanceTable[instance]->MODE |= FTM_MODE_WPDIS_MASK;
				FTM_InstanceTable[instance]->MODE |= FTM_MODE_FTMEN_MASK;
				FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
				FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK;
            break;
        case kQuadratureDecoder:
            break;            
        default:
            break;
    }
}


uint8_t FTM_QuickInit(uint32_t FTMxMAP, uint32_t frequencyInHZ)
{
    uint8_t i;
    QuickInit_Type * pFTMxMap = (QuickInit_Type*)&(FTMxMAP);
    FTM_InitTypeDef FTM_InitStruct1;
    FTM_InitStruct1.instance = pFTMxMap->ip_instance;
    FTM_InitStruct1.frequencyInHZ = frequencyInHZ;
    FTM_InitStruct1.mode = kPWM_EdgeAligned;
    FTM_InitStruct1.chl = pFTMxMap->channel;

    printf("pFTMxMap->ip_instance:%d\r\n", pFTMxMap->ip_instance);
    printf("pFTMxMap->io_instance:%d\r\n", pFTMxMap->io_instance);
    printf("pFTMxMap->io_base:%d\r\n", pFTMxMap->io_base);
    printf("pFTMxMap->io_offset:%d\r\n", pFTMxMap->io_offset);
    printf("pFTMxMap->mux:%d\r\n", pFTMxMap->mux);
    printf("pFTMxMap->channel:%d\r\n", pFTMxMap->channel);
    
    FTM_Init(&FTM_InitStruct1);
    // init pinmux
    for(i = 0; i < pFTMxMap->io_offset; i++)
    {
        PORT_PinMuxConfig(pFTMxMap->io_instance, pFTMxMap->io_base + i, (PORT_PinMux_Type) pFTMxMap->mux); 
    }
    return pFTMxMap->ip_instance;
}


void FTM_PWM_ChangeDuty(uint8_t instance, uint8_t chl, uint32_t pwmDuty)
{
	uint32_t cv = 0;
    cv = ((FTM_InstanceTable[instance]->MOD) * pwmDuty) / 10000;
    FTM_InstanceTable[instance]->CONTROLS[chl].CnV = cv;
    FTM_InstanceTable[instance]->CONTROLS[chl-1].CnV = cv - 400;
}


#if 0
/***********************************************************************************************
 功能：设置PWM工作模式
 形参：FTM_PWM_InitTypeDef:PWM初始化结构
 返回：0
 详解：0
************************************************************************************************/
static void FTM_PWM_SetMode(FTM_InitTypeDef *FTM_InitStruct)
{
	FTM_Type *FTMx = NULL;
	FTM_PWM_MapTypeDef *pFTM_Map = NULL;
	pFTM_Map = (FTM_PWM_MapTypeDef*)&(FTM_InitStruct->FTMxMAP);
	//找出FTM口
	switch(pFTM_Map->FTM_Index)
	{
		case 0:
			FTMx = FTM0;
			break;
		case 1:
			FTMx = FTM1;
			break;
		case 2:
			FTMx = FTM2;
			break;
		default:break;	
	}
	switch(FTM_InitStruct->FTM_Mode)
	{
		case FTM_Mode_EdgeAligned: //边沿对齐模式
			//禁止写保护
			FTMx->MODE |= FTM_MODE_WPDIS_MASK;
		  //禁止增强模式
			FTMx->MODE &= ~FTM_MODE_FTMEN_MASK;
		  //禁止正交编码模式
			FTMx->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
		  //边沿对齐
			FTMx->SC &= ~FTM_SC_CPWMS_MASK;
			FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC = 0;
			FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);	
			switch(pFTM_Map->FTM_CH_Index)
			{
				case 0:
				case 1:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN0_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE0_MASK;	
					break;
				case 2:
				case 3:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN1_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE1_MASK;					
					break;
				case 4:
				case 5:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN2_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE2_MASK;	
					break;
				case 6:
				case 7:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN3_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE3_MASK;	
					break;
				default:break;
			}
			break;
		case FTM_Mode_CenterAligned: //居中对齐模式
			//禁止写保护 可以写入所有FTM寄存器 禁止增强模式
			FTMx->MODE |= FTM_MODE_WPDIS_MASK;
			FTMx->MODE &= ~FTM_MODE_FTMEN_MASK;
			FTMx->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
			FTMx->SC |= FTM_SC_CPWMS_MASK;
			FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC = 0;
			FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);	
			switch(pFTM_Map->FTM_CH_Index)
			{
				case 0:
				case 1:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN0_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE0_MASK;	
					break;
				case 2:
				case 3:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN1_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE1_MASK;					
					break;
				case 4:
				case 5:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN2_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE2_MASK;	
					break;
				case 6:
				case 7:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN3_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_COMBINE3_MASK;	
					break;
				default:break;
			}
			break;
		case FTM_Mode_Combine:  //组合模式 Chl(n) & Chl(n+1) 组合 在Chl(n) 输出
			//禁止写保护 可以写入所有FTM寄存器 开启增强模式
			FTMx->MODE |= FTM_MODE_WPDIS_MASK;
			FTMx->MODE |= FTM_MODE_FTMEN_MASK;
			FTMx->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
			FTMx->SC &= ~FTM_SC_CPWMS_MASK;
		  //死区控制
			//FTMx->DEADTIME=FTM_DEADTIME_DTPS(2)|FTM_DEADTIME_DTVAL(10);
		  //同步控制
			FTMx->SYNC = FTM_SYNC_CNTMIN_MASK|FTM_SYNC_CNTMAX_MASK;
			FTMx->SYNC |= FTM_SYNC_SWSYNC_MASK;
		  //装入计数值
			FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC = 0;
			FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);	
			switch(pFTM_Map->FTM_CH_Index)
			{
				case 0:
				case 1:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN0_MASK;
					FTMx->COMBINE |= FTM_COMBINE_COMBINE0_MASK;	
					FTMx->COMBINE &= ~FTM_COMBINE_COMP0_MASK;
				  FTMx->COMBINE |= FTM_COMBINE_SYNCEN0_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_DTEN0_MASK;
					break;
				case 2:
				case 3:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN1_MASK;
					FTMx->COMBINE |= FTM_COMBINE_COMBINE1_MASK;	
					FTMx->COMBINE &= ~FTM_COMBINE_COMP1_MASK;
				  FTMx->COMBINE |= FTM_COMBINE_SYNCEN1_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_DTEN1_MASK;
					break;
				case 4:
				case 5:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN2_MASK;
					FTMx->COMBINE |= FTM_COMBINE_COMBINE2_MASK;	
					FTMx->COMBINE &= ~FTM_COMBINE_COMP2_MASK;
				  FTMx->COMBINE |= FTM_COMBINE_SYNCEN2_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_DTEN2_MASK;
					break;
				case 6:
				case 7:
					FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN3_MASK;
					FTMx->COMBINE |= FTM_COMBINE_COMBINE3_MASK;	
					FTMx->COMBINE &= ~FTM_COMBINE_COMP3_MASK;
				  FTMx->COMBINE |= FTM_COMBINE_SYNCEN3_MASK;
					FTMx->COMBINE &= ~FTM_COMBINE_DTEN3_MASK;
					break;
				default:break;
			}
			break;
			case FTM_Mode_Complementary:
			{
				//禁止写保护 可以写入所有FTM寄存器 开启增强模式
				FTMx->MODE |= FTM_MODE_WPDIS_MASK;
				FTMx->MODE |= FTM_MODE_FTMEN_MASK;
				FTMx->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
				FTMx->SC &= ~FTM_SC_CPWMS_MASK;
				//死区控制
				FTMx->DEADTIME=FTM_DEADTIME_DTPS(2)|FTM_DEADTIME_DTVAL(5);
				//同步控制
				FTMx->SYNC = FTM_SYNC_CNTMIN_MASK|FTM_SYNC_CNTMAX_MASK;
				FTMx->SYNC |= FTM_SYNC_SWSYNC_MASK;
				//装入计数值
				FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC = 0;
				FTMx->CONTROLS[pFTM_Map->FTM_CH_Index].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);
				switch(pFTM_Map->FTM_CH_Index)
				{
					case 0:
					case 1:
						FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN0_MASK;
						FTMx->COMBINE |= FTM_COMBINE_COMBINE0_MASK;	
						FTMx->COMBINE |= FTM_COMBINE_COMP0_MASK;
						FTMx->COMBINE |= FTM_COMBINE_SYNCEN0_MASK;
						FTMx->COMBINE |= FTM_COMBINE_DTEN0_MASK;
						break;
					case 2:
					case 3:
						FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN1_MASK;
						FTMx->COMBINE |= FTM_COMBINE_COMBINE1_MASK;	
						FTMx->COMBINE |= FTM_COMBINE_COMP1_MASK;
						FTMx->COMBINE |= FTM_COMBINE_SYNCEN1_MASK;
						FTMx->COMBINE |= FTM_COMBINE_DTEN1_MASK;
						break;
					case 4:
					case 5:
						FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN2_MASK;
						FTMx->COMBINE |= FTM_COMBINE_COMBINE2_MASK;	
						FTMx->COMBINE |= FTM_COMBINE_COMP2_MASK;
						FTMx->COMBINE |= FTM_COMBINE_SYNCEN2_MASK;
						FTMx->COMBINE |= FTM_COMBINE_DTEN2_MASK;
						break;
					case 6:
					case 7:
						FTMx->COMBINE &= ~FTM_COMBINE_DECAPEN3_MASK;
						FTMx->COMBINE |= FTM_COMBINE_COMBINE3_MASK;	
						FTMx->COMBINE |= FTM_COMBINE_COMP3_MASK;
						FTMx->COMBINE |= FTM_COMBINE_SYNCEN3_MASK;
						FTMx->COMBINE |= FTM_COMBINE_DTEN3_MASK;
						break;
					default:break;
				}
			}
			break;
			default:break;
	}
}


/***********************************************************************************************
 功能：FTM 获得正交编码数据
 形参：ftm: FTM模块号
       value: 编码计数值指针
       dir：  方向指针 0正向 1反相 
 返回：0
 详解：Added in 2013-12-12
************************************************************************************************/
void FTM_QDGetData(FTM_Type *ftm, uint32_t* value, uint8_t* dir)
{
	*dir = (((ftm->QDCTRL)>>FTM_QDCTRL_QUADIR_SHIFT)&1);
	*value = (ftm->CNT);
}

/***********************************************************************************************
 功能：FTM 正交解码器 初始化 
 形参：FTM1_QD_A12_PHA_A13_PHB: FTM1 A12-PHA  A13-PHB
       FTM1_QD_B00_PHA_B01_PHB
       FTM2_QD_B18_PHA_B19_PHB
 返回：0
 详解：Added in 2013-12-12
************************************************************************************************/
void FTM_QDInit(uint32_t FTM_QD_Maps)
{
    FTM_Type *FTMx = NULL;
    PORT_Type *FTM_PORT = NULL;
    FTM_QD_MapTypeDef *pFTM_Map = (FTM_QD_MapTypeDef*)&FTM_QD_Maps;
    // get module index
    switch(pFTM_Map->FTM_Index)
    {
        case 0:
            SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
            FTMx = FTM0;
            break;
        case 1:
            SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
            FTMx = FTM1;
            break;
        case 2:
            SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
            FTMx = FTM2;
            break;
        default:
            break;	
    }
    //get pinmux port index
    switch(pFTM_Map->FTM_GPIO_Index)
    {
        case 0:
            FTM_PORT = PORTA;
            SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
            break;
        case 1:
            FTM_PORT = PORTB;
            SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
            break;	
        case 2:
            FTM_PORT = PORTC;
            SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
            break;
        case 3:
            FTM_PORT = PORTD;
            SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
            break;
        case 4:
            FTM_PORT = PORTE;
            SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
            break;
    }
    //Config the PinMux and enable pull up
    FTM_PORT->PCR[pFTM_Map->FTM_PHA_Index] &= ~PORT_PCR_MUX_MASK;
    FTM_PORT->PCR[pFTM_Map->FTM_PHA_Index] |= PORT_PCR_MUX(pFTM_Map->FTM_Alt_Index);
    FTM_PORT->PCR[pFTM_Map->FTM_PHA_Index] |= PORT_PCR_PE_MASK;
    FTM_PORT->PCR[pFTM_Map->FTM_PHA_Index] |= PORT_PCR_PS_MASK;
		
    FTM_PORT->PCR[pFTM_Map->FTM_PHB_Index] &= ~PORT_PCR_MUX_MASK;
    FTM_PORT->PCR[pFTM_Map->FTM_PHB_Index] |= PORT_PCR_MUX(pFTM_Map->FTM_Alt_Index);
    FTM_PORT->PCR[pFTM_Map->FTM_PHB_Index] |= PORT_PCR_PE_MASK;
    FTM_PORT->PCR[pFTM_Map->FTM_PHB_Index] |= PORT_PCR_PS_MASK;
		
    FTMx->MOD = 14000; //根据需要设置
    FTMx->CNTIN = 0;
    FTMx->MODE |= FTM_MODE_WPDIS_MASK; //禁止写保护
    FTMx->MODE |= FTM_MODE_FTMEN_MASK; //FTMEN=1,关闭TPM兼容模式，开启FTM所有功能
    FTMx->QDCTRL &= ~FTM_QDCTRL_QUADMODE_MASK; //选定编码模式为A相与B相编码模式 
    FTMx->QDCTRL |= FTM_QDCTRL_QUADEN_MASK; //使能正交解码模式
    //设置时钟
    FTMx->SC |= FTM_SC_CLKS(1)|FTM_SC_PS(3);
}


/***********************************************************************************************
 功能：FTM_ITConfig
 形参：FTMx : FTM模块通道 
       @arg FTM0 : FTM0模块
			 @arg FTM1 : FTM1模块
			 @arg FTM2 : FTM2模块
			 FTM_IT ： FTM中断源
			 @arg FTM_IT_TOF          
			 @arg FTM_IT_CHF0          
			 @arg FTM_IT_CHF1          
			 @arg FTM_IT_CHF2          
			 @arg FTM_IT_CHF3          
			 @arg FTM_IT_CHF4          
			 @arg FTM_IT_CHF5          
			 @arg FTM_IT_CHF6          
			 @arg FTM_IT_CHF7         
			 NewState: 关闭或者使能
			 @arg ENABLE : 使能
			 @arg DISABLE:禁止
 返回：0
 详解：0
************************************************************************************************/
void FTM_ITConfig(FTM_Type* FTMx, uint16_t FTM_IT, FunctionalState NewState)
{
	//参数检查
	assert_param(IS_FTM_ALL_PERIPH(FTMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	
	switch(FTM_IT)
	{
		case FTM_IT_TOF:
			(ENABLE == NewState)?(FTMx->SC |= FTM_SC_TOIE_MASK):(FTMx->SC &= ~FTM_SC_TOIE_MASK);
			break;
		case FTM_IT_CHF0:
		case FTM_IT_CHF1:
		case FTM_IT_CHF2:
		case FTM_IT_CHF3:
		case FTM_IT_CHF4:
		case FTM_IT_CHF5:
		case FTM_IT_CHF6:
		case FTM_IT_CHF7:	
			(ENABLE == NewState)?(FTMx->CONTROLS[FTM_IT].CnSC |= FTM_CnSC_CHIE_MASK):(FTMx->CONTROLS[FTM_IT].CnSC &= ~FTM_CnSC_CHIE_MASK);		
			break;
		default:break;
	}
}
/***********************************************************************************************
 功能：FTM_GetITStatus 获得IT标志位
 形参：FTMx : FTM模块通道 
       @arg FTM0 : FTM0模块
			 @arg FTM1 : FTM1模块
			 @arg FTM2 : FTM2模块
			 FTM_IT ： FTM中断源
			 @arg FTM_IT_TOF          
			 @arg FTM_IT_CHF0          
			 @arg FTM_IT_CHF1          
			 @arg FTM_IT_CHF2          
			 @arg FTM_IT_CHF3          
			 @arg FTM_IT_CHF4          
			 @arg FTM_IT_CHF5          
			 @arg FTM_IT_CHF6          
			 @arg FTM_IT_CHF7         
 返回：ITStatus:标志
       @arg SET:标志位置位
       @arg RESET:标志位清0
 详解：0
************************************************************************************************/
ITStatus FTM_GetITStatus(FTM_Type* FTMx, uint16_t FTM_IT)
{
	ITStatus retval;
	//参数检查
	assert_param(IS_FTM_ALL_PERIPH(FTMx));
	assert_param(IS_FTM_IT(FTM_IT));
	

	switch(FTM_IT)
	{
		case FTM_IT_TOF:
			(FTMx->SC & FTM_SC_TOF_MASK)?(retval = SET):(retval = RESET);
		break;
		case FTM_IT_CHF0:
		case FTM_IT_CHF1:
		case FTM_IT_CHF2:
		case FTM_IT_CHF3:
		case FTM_IT_CHF4:
		case FTM_IT_CHF5:
		case FTM_IT_CHF6:
		case FTM_IT_CHF7:	
			(FTMx->CONTROLS[FTM_IT].CnSC& FTM_CnSC_CHF_MASK)?(retval = SET):(retval = RESET);
			break;
	}
	return retval;
}
/***********************************************************************************************
 功能：清IT 标志位
 形参：FTMx : FTM模块通道 
       @arg FTM0 : FTM0模块
			 @arg FTM1 : FTM1模块
			 @arg FTM2 : FTM2模块
			 FTM_IT ： FTM中断源
			 @arg FTM_IT_TOF          
			 @arg FTM_IT_CHF0          
			 @arg FTM_IT_CHF1          
			 @arg FTM_IT_CHF2          
			 @arg FTM_IT_CHF3          
			 @arg FTM_IT_CHF4          
			 @arg FTM_IT_CHF5          
			 @arg FTM_IT_CHF6          
			 @arg FTM_IT_CHF7         
 返回：0
 详解：0
************************************************************************************************/
void FTM_ClearITPendingBit(FTM_Type *FTMx,uint16_t FTM_IT)
{
	uint32_t read_value = 0;
	//参数检查
	assert_param(IS_FTM_ALL_PERIPH(FTMx));
	assert_param(IS_FTM_IT(FTM_IT));
	
	read_value = read_value;
	switch(FTM_IT)
	{
		case FTM_IT_TOF:
			read_value = FTMx->SC;
		break;
		case FTM_IT_CHF0:
		case FTM_IT_CHF1:
		case FTM_IT_CHF2:
		case FTM_IT_CHF3:
		case FTM_IT_CHF4:
		case FTM_IT_CHF5:
		case FTM_IT_CHF6:
		case FTM_IT_CHF7:	
			read_value = FTMx->CONTROLS[FTM_IT].CnSC;
			break;
	}
}


#endif


static const QuickInit_Type FTM_QuickInitTable[] =
{
    { 0, 1, 4, 12, 1, 4}, //FTM0_CH4_PB12 4
    { 0, 1, 4, 13, 1, 5}, //FTM0_CH5_PB13  4
    { 0, 0, 3,  0, 1, 5}, //FTM0_CH5_PA00  3
    { 0, 0, 3,  1, 1, 6}, //FTM0_CH6_PA01  3
    { 0, 0, 3,  2, 1, 7}, //FTM0_CH7_PA02  3
    { 0, 0, 3,  3, 1, 0}, //FTM0_CH0_PA03  3
    { 0, 0, 3,  4, 1, 1}, //FTM0_CH1_PA04  3
    { 0, 0, 3,  5, 1, 2}, //FTM0_CH2_PA05  3
    { 0, 0, 3,  6, 1, 3}, //FTM0_CH3_PA06  3
    { 0, 0, 3,  7, 1, 4}, //FTM0_CH4_PA07  3
    { 0, 2, 4,  1, 1, 0}, //FTM0_CH0_PC01  4
    { 0, 2, 4,  2, 1, 1}, //FTM0_CH1_PC02  4
    { 0, 2, 4,  3, 1, 2}, //FTM0_CH2_PC03  4
    { 0, 2, 4,  4, 1, 3}, //FTM0_CH3_PC04  4
    { 0, 3, 4,  4, 1, 4}, //FTM0_CH4_PD04  4
    { 0, 3, 4,  5, 1, 5}, //FTM0_CH5_PD05  4
    { 0, 3, 4,  6, 1, 6}, //FTM0_CH6_PD06  4
    { 0, 3, 4,  7, 1, 7}, //FTM0_CH7_PD07  4
    { 1, 1, 3, 12, 1, 0}, //FTM1_CH0_PB12  3
    { 1, 1, 3, 13, 1, 1}, //FTM1_CH1_PB13  3
    { 1, 0, 3,  8, 1, 0}, //FTM1_CH0_PA08  3
    { 1, 0, 3,  9, 1, 1}, //FTM1_CH1_PA09  3
    { 1, 0, 3, 12, 1, 0}, //FTM1_CH0_PA12  3
    { 1, 0, 3, 12, 1, 1}, //FTM1_CH1_PA13  3
    { 1, 1, 3,  0, 1, 0}, //FTM1_CH0_PB00  3
    { 1, 1, 3,  1, 1, 1}, //FTM1_CH1_PB01  3
    { 2, 0, 3, 10, 1, 0}, //FTM2_CH0_PA10  3
    { 2, 0, 3, 11, 1, 1}, //FTM2_CH1_PA11  3
    { 2, 1, 3, 18, 1, 0}, //FTM2_CH0_PB18  3
    { 2, 1, 3, 19, 1, 1}, //FTM2_CH1_PB19  3
};


