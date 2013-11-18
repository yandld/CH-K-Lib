/**
  ******************************************************************************
  * @file    sys.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   超核K60固件库 系统级API函数
  ******************************************************************************
  */
#include "sys.h"
#include "libconfig.h"
//! @brief struct CPUInfo contain various CPU information.
SYS_CPUInfoTypeDef CPUInfo; //处理器运行信息结构体

//! @defgroup CH_Periph_Driver
//! @{
	
//! @defgroup SYS
//! @brief SYS driver modules
//! @{
	

//! @defgroup SYS_Exported_Functions
//! @{

/**
  * @brief  GetCPUInfo, this function mainly for Fwlib interaly use. but user can also call this funtion.
						this function caluate many CPU information and store in public struct CPUInfo.
  * @param  None			
  * @retval None
  */
void GetCPUInfo(void)
{
    //ResetStateCode
    if (MC->SRSH & MC_SRSH_SW_MASK)     CPUInfo.m_ResetStateCode = kSoftwareReset;
    if (MC->SRSH & MC_SRSH_LOCKUP_MASK) CPUInfo.m_ResetStateCode = kCoreLockupEventReset;
    if (MC->SRSH & MC_SRSH_JTAG_MASK)   CPUInfo.m_ResetStateCode = kJTAGReset;
    if (MC->SRSL & MC_SRSL_POR_MASK)    CPUInfo.m_ResetStateCode = kPOReset;
    if (MC->SRSL & MC_SRSL_PIN_MASK)  	CPUInfo.m_ResetStateCode = kExternalPinReset;
    if (MC->SRSL & MC_SRSL_COP_MASK)    CPUInfo.m_ResetStateCode = kWdogReset;
    if (MC->SRSL & MC_SRSL_LOC_MASK)    CPUInfo.m_ResetStateCode = kLossOfClockReset;
    if (MC->SRSL & MC_SRSL_LVD_MASK)    CPUInfo.m_ResetStateCode = kLowVoltageDetectReset;
    if (MC->SRSL & MC_SRSL_WAKEUP_MASK) CPUInfo.m_ResetStateCode = kLLWUReset;
    //FaimlyTypeCode
    switch((SIM->SDID & SIM_SDID_FAMID(0x7))>>SIM_SDID_FAMID_SHIFT) 
    {  
        case 0x0:
            CPUInfo.m_FamilyTypeCode = kK10;
            break;
        case 0x1: 
            CPUInfo.m_FamilyTypeCode = kK20; 
            break;
        case 0x2: 
            CPUInfo.m_FamilyTypeCode = kK30;
            break;
        case 0x3: 
            CPUInfo.m_FamilyTypeCode = kK40; 
            break;
        case 0x4: 
            CPUInfo.m_FamilyTypeCode = kK60; 
            break;
        case 0x5: 
            CPUInfo.m_FamilyTypeCode = kK70; 
            break;
        case 0x6:
            CPUInfo.m_FamilyTypeCode = kK50;
            break;
        case 0x7: 
            CPUInfo.m_FamilyTypeCode = kK53; 
            break;
        default: 
            CPUInfo.m_FamilyTypeCode = kUnrecognizedFamilyType; 
            break;
    }
    //Pin package information
    switch((SIM->SDID & SIM_SDID_PINID(0xF))>>SIM_SDID_PINID_SHIFT) 
    {  
        case 0x02: 
            CPUInfo.m_PinNumberCode = k32Pin; 
            break;
        case 0x04: 
            CPUInfo.m_PinNumberCode = k48Pin; 
            break;
        case 0x05:
            CPUInfo.m_PinNumberCode = k64Pin;
            break;
        case 0x06: 
            CPUInfo.m_PinNumberCode = k80Pin;
            break;
        case 0x07: 
            CPUInfo.m_PinNumberCode = k81Pin; 
            break;
        case 0x08:
            CPUInfo.m_PinNumberCode = k100Pin;
            break;
        case 0x09:
            CPUInfo.m_PinNumberCode = k104Pin; 
            break;
        case 0x0A: 
					CPUInfo.m_PinNumberCode = k144Pin; 
            break;
        case 0x0C: 
            CPUInfo.m_PinNumberCode = k196Pin; 
            break;
        case 0x0E: 
            CPUInfo.m_PinNumberCode = k256Pin;
            break;
        default:  
            CPUInfo.m_PinNumberCode = kUnrecognizedPin;  
            break;	
    }  
    //PFlash Size
    switch((SIM->FCFG1 & SIM_FCFG1_PFSIZE(0xF))>>SIM_FCFG1_PFSIZE_SHIFT)
    {
        case 0x7:
            CPUInfo.m_PFlashSizeInKB = 128;
            break;
        case 0x9:
            CPUInfo.m_PFlashSizeInKB = 256;
            break;
        case 0xB:
            CPUInfo.m_PFlashSizeInKB = 512;
            break;
        case 0xF:
            CPUInfo.m_PFlashSizeInKB = 512;
            break;
        default:
            CPUInfo.m_PFlashSizeInKB = 0;
        break;
    }
    //FlexNVM Size
    if (SIM->FCFG2 & SIM_FCFG2_PFLSH_MASK) 
    {
        CPUInfo.m_FlexNVMSizeInKB = 0;
    }
    else
    {
        switch((SIM->FCFG1 & SIM_FCFG1_NVMSIZE(0xF))>>SIM_FCFG1_NVMSIZE_SHIFT)
        {
            case 0x0:
                CPUInfo.m_FlexNVMSizeInKB = 0;
                break;
            case 0x7: 
                CPUInfo.m_FlexNVMSizeInKB = 128;
                break;
            case 0x9: 
                CPUInfo.m_FlexNVMSizeInKB = 256;
                break;
            case 0xF: 
                CPUInfo.m_FlexNVMSizeInKB = 256;
                break;
            default:  
                CPUInfo.m_FlexNVMSizeInKB = 0;
                break; 		
        }
    }
    //RAM Size
    switch((SIM->SOPT1 & SIM_SOPT1_RAMSIZE(0xF))>>SIM_SOPT1_RAMSIZE_SHIFT)
    {
        case 0x5: 
            CPUInfo.m_RAMSizeInKB = 32; 
            break;
        case 0x7: 
            CPUInfo.m_RAMSizeInKB = 64;  
            break;
        case 0x8: 
            CPUInfo.m_RAMSizeInKB = 96;  
            break;
        case 0x9: 
            CPUInfo.m_RAMSizeInKB = 128;
            break;
        default: 
            CPUInfo.m_RAMSizeInKB = 0; 
            break;	
    }
    //Clock 
    CPUInfo.m_CoreClockInHz = SystemCoreClock;
    CPUInfo.m_BusClockInHz =(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT)+1));     
    CPUInfo.m_FlexBusClockHz =(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV3_MASK)>>SIM_CLKDIV1_OUTDIV3_SHIFT)+1)); 
    CPUInfo.m_FlashClockHz =(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT)+1));
}

/**
  * @brief  SystemClockSetup, change coreclock freq.other clock freq is also changed 
            according to there max limit.
            Not all core clock can be reached in any clock source option.
            We advised that use DisplayCPUInfo to check out.
  * @param  SYS_ClockSourceSelect_TypeDef 
            @arg kClockSource_IRC  : interal clock is used, run in FLL mode
            @arg kClockSource_EX8M : external 8M OSC is used,run in PLL mode
            @arg kClockSource_EX50M: external 50M OSC is used,run in PLL mode
  * @param  SYS_CoreClockSelect_TypeDef
            @arg kCoreClock_48M    : coreclock = 48MHz
            @arg kCoreClock_64M    : coreclock = 64MHz
            @arg kCoreClock_72M    : coreclock = 72MHz
            @arg kCoreClock_96M    : coreclock = 96MHz
            @arg kCoreClock_100M    : coreclock = 100MHz		
            @arg kCoreClock_200M    : coreclock = 200MHz					
  * @retval None
  */
void SystemClockSetup(SYS_ClockSourceSelect_TypeDef clockSource, SYS_CoreClockSelect_TypeDef coreClock)
{
    //SIM_CLKDIV1_OUTDIV1(0) CORE     CLOCK    UP TO 100M  
    //SIM_CLKDIV1_OUTDIV2(1) BUS      CLOCK    UP TO 50M 
    //SIM_CLKDIV1_OUTDIV3(1) FlexBus  ClOCK    UP TO 50M 
    //SIM_CLKDIV1_OUTDIV4(3) Flash    ClOCK    UP TO 25M 
    if(clockSource == kClockSource_IRC)
    {
        SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu; //set all clock to lowest level
        // FEI mode
        MCG->C1 = (uint8_t)0x06u;
        MCG->C2 = (uint8_t)0x00u;
        MCG->C4 &= ~((1<<6)|(1<<7)|(1<<5));
        switch(coreClock)
        {
            case kCoreClock_96M:
                MCG->C4|= (1<<6)|(1<<7)|(1<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));
                SystemCoreClock	= 96000000;
                break;
            case kCoreClock_72M:
                MCG->C4|= (1<<6)|(1<<7)|(0<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(2));	
                SystemCoreClock	= 72000000;
                break;
            case kCoreClock_48M:
                MCG->C4|= (0<<6)|(1<<7)|(1<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));	
                SystemCoreClock	= 48000000;
                break;
            default: //48M
                MCG->C4|= (0<<6)|(1<<7)|(1<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));	
                SystemCoreClock	= 48000000;
                break;
        }
        MCG->C5 = (uint8_t)0x00u;
		    MCG->C6 = (uint8_t)0x00u; 
	    	while((MCG->S & MCG_S_IREFST_MASK) == 0u);  //check FLL's clock is come from IRClock
	    	while((MCG->S & 0x0Cu) != 0x00u);           //wait for FLL to be locked
    }
    if(clockSource == kClockSource_EX8M || clockSource == kClockSource_EX50M)
    {
        SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu; //set all clock to lowest level
        //FBE mode
        OSC->CR = (uint8_t)0x00u;
        SIM->SOPT2 &= (uint8_t)~(uint8_t)0x01u;
        MCG->C2 = (uint8_t)0x24u;
        MCG->C1 = (uint8_t)0x9Au;
        MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
        MCG->C5 = (uint8_t)0x03u; 
        MCG->C6 = (uint8_t)0x00u;
        while((MCG->S & MCG_S_OSCINIT_MASK) == 0u);//check FLL's clock is come from IRClock
        while((MCG->S & MCG_S_IREFST_MASK) != 0u); 
        while((MCG->S & 0x0Cu) != 0x08u);
        if(clockSource == kClockSource_EX8M)
        {
            MCG->C5 = (uint8_t)MCG_C5_PRDIV(3);        //8/4 = 2M
        }
        else if (clockSource == kClockSource_EX50M)
        {
            MCG->C5 = (uint8_t)MCG_C5_PRDIV(24);        //50/25 = 2M
        }
        switch(coreClock)
        {
            case kCoreClock_48M:
				        MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(0));
			        	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));	
			        	SystemCoreClock	= 48000000;
			        	break;		
            case kCoreClock_64M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(8));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(2));
                SystemCoreClock	= 64000000;
                break;		
            case kCoreClock_72M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(12));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(2));
                SystemCoreClock	= 72000000;
                break;			
            case kCoreClock_96M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(24));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));		
                SystemCoreClock	= 96000000;
                break;		
            case kCoreClock_100M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(26));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));	
                SystemCoreClock	= 100000000;
                break;	
            case kCoreClock_200M:
                if(clockSource == kClockSource_EX8M)
                {
                    MCG->C5 = (uint8_t)MCG_C5_PRDIV(1);        //8/2 = 4M
                    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(26));
                    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));	
                }
                else if (clockSource == kClockSource_EX50M)
                {
                    MCG->C5 = (uint8_t)MCG_C5_PRDIV(12);
                    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(28));
                    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));	
                }
                SystemCoreClock	= 200000000;
                break;			
            default:
			        	MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(0));
			        	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));
		        		SystemCoreClock	= 48000000;
			        	break;
		    } 
        while((MCG->S & MCG_S_PLLST_MASK) == 0u);   //wait for PLL
        while((MCG->S & MCG_S_LOCK_MASK) == 0u);    
	    	//turn in to PBE
	    	MCG->C1 = (uint8_t)0x1Au;
	    	while((MCG->S & 0x0Cu) != 0x0Cu);
        while((MCG->S & MCG_S_LOCK_MASK) == 0u);  
    }
		// recalculate all clock
    GetCPUInfo();
}

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
void PinMuxConfig(uint8_t GPIOIndex, uint8_t PinIndex, uint8_t MuxIndex)
{
    switch(GPIOIndex)
    {
        case 0:
            SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
            PORTA->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTA->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
            break;
        case 1:
            SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
            PORTB->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTB->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
            break;
        case 2:
            SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
            PORTC->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTC->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
            break;
        case 3:
            SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
            PORTD->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTD->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
            break;
        case 4:
            SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
            PORTE->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
            PORTE->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
            break;
        default:
            break;
    }
}

/**
  * @brief  NVIC_Init, Set a interrupt priority. This function base on Cortex-M core. Independed from peripheral
  * @param  IRQn : IRQ number, located in MKxxxx.h
            PriorityGroup:
						@arg NVIC_PriorityGroup_0 : Group0
						@arg NVIC_PriorityGroup_1 : Group1
						@arg NVIC_PriorityGroup_2 : Group2
						@arg NVIC_PriorityGroup_3 : Group3
						@arg NVIC_PriorityGroup_4 : Group4
  * @param   PreemptPriority： Preemptive priority value (starting from 0).
  * @param   SubPriority:      Subpriority value (starting from 0).
  * @retval None
  */
void NVIC_Init(IRQn_Type IRQn,uint32_t PriorityGroup,uint32_t PreemptPriority,uint32_t SubPriority)
{
    //param check
    assert_param(IS_NVIC_PRIORITY_GROUP(PriorityGroup));
    assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));  
    assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
    //NVIC priority grouping
    NVIC_SetPriorityGrouping(PriorityGroup);
    //Set NVIC interrupt priority
    NVIC_SetPriority(IRQn,NVIC_EncodePriority(PriorityGroup,PreemptPriority,SubPriority));
}
/**
 * @brief  Reboot
 * @param  none
 * @retval none
 */
void SystemSoftReset(void)
{   
    NVIC_SystemReset();
} 
/**
 * @brief  Enable all interrupts
 * @param  none
 * @retval none
 */
void EnableInterrupts(void)
{
    __enable_irq();
}
/**
 * @brief  Disable all interrupts
 * @param  none
 * @retval none
 */
void DisableInterrupts(void)
{
    __disable_irq();
}
/**
 * @brief  Set interrupt table offset
 * @param  offset
 * @retval none
 */
void SetVectorTable(uint32_t VectorOffset)
{
    //param check
    assert_param(IS_VECTOR_OFFSET(VectorOffset));
    SCB->VTOR = VectorOffset;  // 更改中断向量表偏移地址
}
/**
 * @brief  Get Fwlib Version
 * @param  None
 * @retval FwLib Version code
 */
uint16_t GetFWVersion(void)
{
    return(FW_VERSION);
}

//! @}

//! @}

//! @}

