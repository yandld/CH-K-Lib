/**
  ******************************************************************************
  * @file    pdb.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "pdb.h"

static PDB_CallBackType PDB_CallBackTable[1] = {NULL};


void PDB_SoftwareTrigger(void)
{
    PDB0->SC |= PDB_SC_SWTRIG_MASK;
}

static void _PDB_SetCounterPeriod(uint32_t srcClock, uint32_t timeInUs)
{
    static const uint8_t MULT[4] = {1, 10, 20, 40};
    uint32_t p;
    uint32_t i,j;
    // (1<<i) 
    uint32_t fac_us = 
    p = (srcClock/1000) / 0xFFFF;
    for(i=0;i<8;i++)
    {
        for(j=0;j<4;j++)
        {
         //   fac_us = srcClock/1000000/(1<<i)/MULT[j];
            if((srcClock/1000000)*timeInUs/((1<<i)*MULT[j]) < 0xFFFF)
            {
                printf("i:%d j:%d\r\n",i, j);
            }
          //  if(> timeInUs)
            {
         //       fac_us*timeInUs*
            }
        }
    }
    
}

void PDB_Init(PDB_InitTypeDef * PDB_InitStruct)
{
	uint8_t i;
	uint32_t p;
	
    /* enable clock gate */
    SIM->SCGC6 |= SIM_SCGC6_PDB_MASK ;
    
    /* reset control register */
	PDB0->SC = 0x00;

    /* trigger source */
	PDB0->SC |= PDB_SC_TRGSEL(PDB_InitStruct->inputTrigSource);

    /* if continues mode */
    (PDB_InitStruct->isContinuesMode)?(PDB0->SC |= PDB_SC_CONT_MASK):(PDB0->SC &= ~PDB_SC_CONT_MASK);
    
    /* MUTI is lock to 1 */
    PDB0->SC &= ~PDB_SC_MULT_MASK;
    PDB0->SC |= PDB_SC_MULT(0);
    
    /* clk div */
    PDB0->SC &= ~PDB_SC_PRESCALER_MASK;
    PDB0->SC |= PDB_SC_PRESCALER(PDB_InitStruct->clkDiv);
    
    /* mod */
    PDB0->MOD = PDB_InitStruct->mod;
    
    /* */
   // PDB_InitStruct->srcClock
	//p = ((CPUInfo.BusClock)*PDB_InitStruct->PDB_Period)/65535;
	for(i=0;i<8;i++)
	{
		if(p/(1<<i) < 40) break;
	}
	if(i > 7) i = 7;

//  PDB0->SC |= PDB_SC_MULT(PDB_MULT_40);
	PDB0->SC |= PDB_SC_PRESCALER(i);

    PDB0->MOD = 10000;
    PDB0->IDLY = 10000;
//	PDB0->MOD =  ((PDB_InitStruct->PDB_Period)*(CPUInfo.BusClock/1000))/(40*(1<<i));
//	PDB0->IDLY = ((PDB_InitStruct->PDB_Period)*(CPUInfo.BusClock/1000))/(40*(1<<i));

    /* enable PDB */
	PDB0->SC |= PDB_SC_PDBEN_MASK; 

    /* enable LOCK */
	PDB0->SC |= PDB_SC_LDOK_MASK;

}


void PDB_ITDMAConfig(PDB_ITDMAConfig_Type config, bool status)
{
    /* enable clock gate */
    SIM->SCGC6 |= SIM_SCGC6_PDB_MASK;
    
    if(!status)
    {
        NVIC_DisableIRQ(PDB0_IRQn);
        return;
    }
    
    switch(config)
    {
        case kPDB_IT_CF:
            (status)?
            (PDB0->SC |= PDB_SC_PDBIE_MASK):
            (PDB0->SC &= ~PDB_SC_PDBIE_MASK);
            NVIC_EnableIRQ(PDB0_IRQn);
            break; 
        case kPDB_DMA_CF:
            (status)?
            (PDB0->SC |= PDB_SC_DMAEN_MASK):
            (PDB0->SC &= ~PDB_SC_DMAEN_MASK);   
        default:
            break;
    }
}


void PDB_CallbackInstall(PDB_CallBackType AppCBFun)
{
    /* enable clock gate */
    SIM->SCGC6 |= SIM_SCGC6_PDB_MASK;
    if(AppCBFun != NULL)
    {
        PDB_CallBackTable[0] = AppCBFun;
    }
}

void PDB0_IRQHandler(void)
{
    /* clear IT pending flags */
    PDB0->SC &= ~PDB_SC_PDBIF_MASK;
    
    if(PDB_CallBackTable[0])
    {
        PDB_CallBackTable[0]();
    }
}
 
