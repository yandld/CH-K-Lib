/**
  ******************************************************************************
  * @file    spi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片IIC模块的底层功能函数
  ******************************************************************************
  */
#include "spi.h"
#include "common.h"

SPI_Type * const SPI_InstanceTable[] = SPI_BASES;
static SPI_CallBackType SPI_CallBackTable[ARRAY_SIZE(SPI_InstanceTable)] = {NULL};
static const struct reg_ops SIM_SPIClockGateTable[] =
{
    {(void*)&(SIM->SCGC4), SIM_SCGC4_SPI0_MASK},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_SPI1_MASK},
};


/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_HAL_SetBaud
 * This function takes in the desired bitsPerSec (baud rate) and calculates the nearest
 * possible baud rate without exceeding the desired baud rate, and  returns the calculated
 * baud rate in bits-per-second. It requires that the caller also provide the frequency of the
 * module source clock (in Hertz).
 *
 *END**************************************************************************/
static uint32_t SPI_HAL_SetBaud(uint32_t instance, uint32_t bitsPerSec, uint32_t sourceClockInHz)
{
    uint32_t prescaler, bestPrescaler;
    uint32_t rateDivisor, bestDivisor;
    uint32_t rateDivisorValue;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t baudrate = bitsPerSec;

    /* find combination of prescaler and scaler resulting in baudrate closest to the
     * requested value
     */
    min_diff = 0xFFFFFFFFU;
    bestPrescaler = 0;
    bestDivisor = 0;
    bestBaudrate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (prescaler = 0; (prescaler <= 7) && min_diff; prescaler++)
    {
        rateDivisorValue = 2U;  /* Initialize to div-by-2 */

        for (rateDivisor = 0; (rateDivisor <= 8U) && min_diff; rateDivisor++)
        {
            /* calculate actual baud rate, note need to add 1 to prescaler */
            realBaudrate = ((sourceClockInHz) /
                            ((prescaler + 1) * rateDivisorValue));

            /* calculate the baud rate difference based on the conditional statement*/
            /* that states that the calculated baud rate must not exceed the desired baud rate*/
            if (baudrate >= realBaudrate)
            {
                diff = baudrate-realBaudrate;
                if (min_diff > diff)
                {
                    /* a better match found */
                    min_diff = diff;
                    bestPrescaler = prescaler; /* Prescale divisor SPIx_BR register bit setting */
                    bestDivisor = rateDivisor; /* baud rate divisor SPIx_BR register bit setting */
                    bestBaudrate = realBaudrate;
                }
            }
            /* Multiply by 2 for each iteration, possible divisor values: 2, 4, 8, 16, ... 512 */
            rateDivisorValue *= 2U;
        }
    }

    /* write the best prescalar and baud rate scalar */
    SPI_InstanceTable[instance]->BR &= ~SPI_BR_SPR_MASK;
    SPI_InstanceTable[instance]->BR &= ~SPI_BR_SPPR_MASK;
    SPI_InstanceTable[instance]->BR |= SPI_BR_SPR(bestDivisor);
    SPI_InstanceTable[instance]->BR |= SPI_BR_SPPR(bestPrescaler);

    /* return the actual calculated baud rate*/
    return bestBaudrate;
}

void SPI_Init(SPI_InitTypeDef* SPI_InitStruct)
{
    uint32_t clock;
    /* enable clock gate */
    *(uint32_t*)SIM_SPIClockGateTable[SPI_InitStruct->instance].addr |= SIM_SPIClockGateTable[SPI_InitStruct->instance].mask;
    
    /* stop SPI */
    SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_SPE_MASK;
    
    /* master or slave */
    switch(SPI_InitStruct->mode)
    {
        case kSPI_Master:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 |= SPI_C1_MSTR_MASK;
            break;
        case kSPI_Slave:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_MSTR_MASK;
            break;
        default:
            break;
    }
    
    /* bit order */
    switch(SPI_InitStruct->bitOrder)
    {
        case kSPI_MSBFirst:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_LSBFE_MASK;
            break;
        case kSPI_LSBFirst:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 |= SPI_C1_LSBFE_MASK;
            break;
        default:
            break;
    }
    
    /* clock parity */
    switch(SPI_InitStruct->frameFormat)
    {
        case kSPI_CPOL0_CPHA0:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_CPHA_MASK;
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_CPOL_MASK;
            break;
        case kSPI_CPOL0_CPHA1:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 |=  SPI_C1_CPHA_MASK;
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_CPOL_MASK;
            break;
        case kSPI_CPOL1_CPHA0:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 &= ~SPI_C1_CPHA_MASK;
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 |=  SPI_C1_CPOL_MASK;
            break;
        case kSPI_CPOL1_CPHA1:
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 |= SPI_C1_CPHA_MASK;
            SPI_InstanceTable[SPI_InitStruct->instance]->C1 |= SPI_C1_CPOL_MASK;
            break;
        default:
            break;
    }
    
    /* config SS as PCS0 output */
    SPI_InstanceTable[SPI_InitStruct->instance]->C2 |= SPI_C2_MODFEN_MASK;
    SPI_InstanceTable[SPI_InitStruct->instance]->C1 |= SPI_C1_SSOE_MASK;
    
    /*baudrate */
    CLOCK_GetClockFrequency(kBusClock, &clock);
    SPI_HAL_SetBaud(SPI_InitStruct->instance, SPI_InitStruct->baudrate, clock);
    
    /* enable SPI */
    SPI_InstanceTable[SPI_InitStruct->instance]->C1 |= SPI_C1_SPE_MASK;
}

 /**
 * @brief  快速初始化SPI模块
 * @param  MAP: SPI快速初始化选择项，详见spi.h文件
 *         @arg SPI1_SCK_PE02_MOSI_PE01_MISO_PE00
 *         @arg         ...
 * @param  baudrate :通信速度
 * @retval spi模块号
 */
uint32_t SPI_QuickInit(uint32_t MAP, SPI_FrameFormat_Type frameFormat, uint32_t baudrate)
{
    SPI_InitTypeDef SPI_InitStruct1;
    SPI_InitStruct1.baudrate = baudrate;
    SPI_InitStruct1.frameFormat = frameFormat;
    SPI_InitStruct1.dataSize = 8;
    SPI_InitStruct1.bitOrder = kSPI_MSBFirst;
    SPI_InitStruct1.mode = kSPI_Master;
    SPI_Init(&SPI_InitStruct1);
}

#if 0
uint16_t SPI_ReadWriteByte(SPI_Type *SPIx, uint16_t Data)
{
  uint8_t temp;
  while(!(SPIx->S & SPI_S_SPTEF_MASK)); /* write finish? */
	#ifdef MKL46Z4_H_
	SPIx->DL = Data;
	#endif
	#ifdef MKL25Z4_H_
	SPIx->D = Data;
	#endif
 	while(!(SPIx->S & SPI_S_SPTEF_MASK)){};     //等待发送完成
  while(!(SPIx->S & SPI_S_SPRF_MASK)); /* read finish? */
	#ifdef MKL46Z4_H_
	temp = SPIx->DL;
	#endif
	#ifdef MKL25Z4_H_
	temp = SPIx->D;
	#endif
  
  return temp;
}


void SPI_Cmd(SPI_Type *SPIx, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    SPIx->C1 |= SPI_C1_SPE_MASK; 
  }
  else
  {
    SPIx->C1 &= ~SPI_C1_SPE_MASK;
  }
}


void SPI_DMACmd(SPI_Type *SPIx, uint16_t SPI_DMAReq, FunctionalState NewState)
{
  switch(SPI_DMAReq)
  {
    case SPI_DMAReq_Tx:
      (ENABLE == NewState)?(SPIx->C2 |= SPI_C2_TXDMAE_MASK):(SPIx->C2 &= ~SPI_C2_RXDMAE_MASK);
    break;
    case SPI_DMAReq_Rx:
      (ENABLE == NewState)?(SPIx->C2 |= SPI_C2_RXDMAE_MASK):(SPIx->C2 &= ~SPI_C2_RXDMAE_MASK);
    break;
    default:break;
  }
}


#endif


/*
static const QuickInit_Type SPI_QuickInitTable[] =
{
    { 1, 4, 2, 0, 3, 0}, //SPI1_SCK_PE02_MOSI_PE01_MISO_PE00
    { 0, 4, 2,17, 3, 0}, //SPI0_SCK_PE17_MOSI_PE18_MISO_PE19
    { 0, 0, 2,15, 3, 0}, //SPI0_SCK_PA15_MOSI_PA16_MISO_PA17
    { 0, 2, 2, 5, 3, 0}, //SPI0_SCK_PC05_MOSI_PC06_MISO_PC07
    { 0, 3, 2, 1, 3, 0}, //SPI0_SCK_PD01_MOSI_PD02_MISO_PD03
    { 1, 3, 2, 5, 3, 0}, //SPI1_SCK_PD05_MOSI_PD06_MISO_PD07
};
*/

