/**
  ******************************************************************************
  * @file    can.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.4.10
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "can.h"
#include "gpio.h"

#define CAN_MB_MAX      (16)
#if (!defined(CAN_BASES))

    #if (defined(MK60DZ10))
        #define CAN_BASES {CAN0, CAN1}
    #endif
#endif
/* global vars */
CAN_Type * const CAN_InstanceTable[] = CAN_BASES;
static const struct reg_ops SIM_CANClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FLEXCAN0_MASK},
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FLEXCAN1_MASK},
}; 
static const IRQn_Type CAN_IRQnTable[] = 
{
    CAN0_ORed_Message_buffer_IRQn,
    CAN1_ORed_Message_buffer_IRQn,
};
/* callback function slot */
static CAN_CallBackType CAN_CallBackTable[ARRAY_SIZE(CAN_InstanceTable)] = {NULL};

#define CAN_GET_MB_CODE(cs)         (((cs) & CAN_CS_CODE_MASK)>>CAN_CS_CODE_SHIFT)
#define CAN_GET_FRAME_LEN(cs)       (((cs) & CAN_CS_DLC_MASK)>>CAN_CS_DLC_SHIFT)

typedef enum
{
    kFlexCanTX_Inactive  = 0x08, /*!< MB is not active.*/
    kFlexCanTX_Abort     = 0x09, /*!< MB is aborted.*/
    kFlexCanTX_Data      = 0x0C, /*!< MB is a TX Data Frame(MB RTR must be 0).*/
    kFlexCanTX_Remote    = 0x1C, /*!< MB is a TX Remote Request Frame (MB RTR must be 1).*/
    kFlexCanTX_Tanswer   = 0x0E, /*!< MB is a TX Response Request Frame from.*/
                                 /*!  an incoming Remote Request Frame.*/
    kFlexCanTX_NotUsed   = 0xF,  /*!< Not used*/
    kFlexCanRX_Inactive  = 0x0, /*!< MB is not active.*/
    kFlexCanRX_Full      = 0x2, /*!< MB is full.*/
    kFlexCanRX_Empty     = 0x4, /*!< MB is active and empty.*/
    kFlexCanRX_Overrun   = 0x6, /*!< MB is overwritten into a full buffer.*/
    //kFlexCanRX_Busy      = 0x8, /*!< FlexCAN is updating the contents of the MB.*/
                                /*!  The CPU must not access the MB.*/
    kFlexCanRX_Ranswer   = 0xA, /*!< A frame was configured to recognize a Remote Request Frame*/
                                /*!  and transmit a Response Frame in return.*/
    kFlexCanRX_NotUsed   = 0xF, /*!< Not used*/
}CAN_MBCode_Type;


/**
 * @brief  设置CAN通讯速率
 * @note   内部函数 BucClock必须是50M  否则通信速度不准确
 * @param  can           :串口通讯速率设置
 * @param  baudrate      :CAN模块的通信速度
 *         @arg CAN_SPEED_20K  :设置通信速度为20K  传输距离3300m
 *         @arg CAN_SPEED_50K  :设置通信速度为50K  
 *         @arg CAN_SPEED_100K :设置通信速度为100K  
 *         @arg CAN_SPEED_125K :设置通信速度为125K  
 *         @arg CAN_SPEED_250K :设置通信速度为250K  
 *         @arg CAN_SPEED_500K :设置通信速度为500K  
 *         @arg CAN_SPEED_1M   :设置通信速度为1M   传输距离40m
 * @retval 0:成功；1:失败
 */
/***********************************************************************************************
 功能：
 形参：can : CAN0 或者  CAN1
			CAN_BAUDRATE_SELECT baudrate:
					
					,
					,
					,
					,
					,
					,  
 返回：0成功  1失败
 详解
************************************************************************************************/
static uint32_t CAN_SetBaudrate(CAN_Type *can, CAN_Baudrate_Type baudrate)
{
    switch(baudrate)
    {
        case kCAN_Baudrate_25K:
			 // 50M/125 = 400k sclock, 16Tq
			 // PROPSEG = 5, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 5, PSEG2 = 5,PRESDIV = 125
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(4) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(4) 
							 | CAN_CTRL1_PSEG2(4)
							 | CAN_CTRL1_PRESDIV(124));
            break;
		case kCAN_Baudrate_50K:
			 // 50M/100= 500K sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 100
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(99));	
            break;
		case kCAN_Baudrate_100K:
			 // 50M/50= 1M sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 50
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(49));	
            break;
		case kCAN_Baudrate_125K:
			 // 50M/25 = 2000k sclock, 16Tq
			 // PROPSEG = 5, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 5, PSEG2 = 5,PRESDIV = 25
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(4) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(4) 
							 | CAN_CTRL1_PSEG2(4)
							 | CAN_CTRL1_PRESDIV(24));	
            break;
		case kCAN_Baudrate_250K:
			 // 50M/20= 2500K sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 20
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(19));			
            break;
		case kCAN_Baudrate_500K:
			 // 50M/10= 5000K sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 20
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(9));				
            break;
		case kCAN_Baudrate_1000K:
			 // 50M/5= 10000K sclock, 10Tq
			 // PROPSEG = 2, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 2, PSEG1 = 3, PSEG2 = 3, PRESDIV = 20
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(4));	
            break;
		default: 
            return 1;
	}
	return 0;
}

void CAN_SetReceiveMask(uint32_t instance, uint32_t mb, uint32_t mask)
{
    CAN_InstanceTable[instance]->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CAN_InstanceTable[instance]->MCR))) {}; 
    if(mask > 0x7FF)
    {	 
        CAN_InstanceTable[instance]->RXIMR[mb] = CAN_ID_EXT(mask); 
    }
    else
    {
        CAN_InstanceTable[instance]->RXIMR[mb] = CAN_ID_STD(mask); 
    } 
    /* enable module */
    CAN_InstanceTable[instance]->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CAN_InstanceTable[instance]->MCR)));
}


void CAN_SetReceiveMB(uint32_t instance, uint32_t mb, uint32_t id)
{
    CAN_SetReceiveMask(instance , mb, id);
    if(id > 0x7FF)
    {	 
        CAN_InstanceTable[instance]->MB[mb].CS |= CAN_CS_IDE_MASK;
        CAN_InstanceTable[instance]->MB[mb].CS &= ~CAN_CS_SRR_MASK;
        /* id settings */
        CAN_InstanceTable[instance]->MB[mb].ID &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
        CAN_InstanceTable[instance]->MB[mb].ID |= (id & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
    }
    else
    {
        CAN_InstanceTable[instance]->MB[mb].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
         /* id settings */
        CAN_InstanceTable[instance]->MB[mb].ID &= ~CAN_ID_STD_MASK;
        CAN_InstanceTable[instance]->MB[mb].ID |= CAN_ID_STD(id);       
    }
    /* set code */
    CAN_InstanceTable[instance]->MB[mb].CS &= ~CAN_CS_CODE_MASK; 
	CAN_InstanceTable[instance]->MB[mb].CS |= CAN_CS_CODE(kFlexCanRX_Empty);
}

void CAN_Init(CAN_InitTypeDef* CAN_InitStruct)
{
    uint32_t i;
    /* enable clock gate */
    *((uint32_t*) SIM_CANClockGateTable[CAN_InitStruct->instance].addr) |= SIM_CANClockGateTable[CAN_InitStruct->instance].mask; 
  
    /* set clock source is bus clock */
    CAN_InstanceTable[CAN_InitStruct->instance]->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

    /* enable module */
    CAN_InstanceTable[CAN_InitStruct->instance]->MCR &= ~CAN_MCR_MDIS_MASK;
    
    /* software reset */
	CAN_InstanceTable[CAN_InitStruct->instance]->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR)) {}; 
        
    /* halt mode */
    CAN_InstanceTable[CAN_InitStruct->instance]->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR))) {}; 
        
    /* init all mb */
    for(i = 0; i < CAN_MB_MAX; i++)
	{
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].CS = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].ID = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].WORD0 = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].WORD1 = 0x00000000;
        /* indviual mask*/
        CAN_InstanceTable[CAN_InitStruct->instance]->RXIMR[i] = CAN_ID_EXT(CAN_RXIMR_MI_MASK);
	}
	/* set all masks */
	//CAN_InstanceTable[CAN_InitStruct->instance]->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
   // CAN_InstanceTable[CAN_InitStruct->instance]->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
   // CAN_InstanceTable[CAN_InitStruct->instance]->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK);
    /* use indviual mask, do not use RXMGMASK, RX14MASK and RX15MASK */
    CAN_InstanceTable[CAN_InitStruct->instance]->MCR |= CAN_MCR_IRMQ_MASK;
    
    /* setting baudrate */
	CAN_SetBaudrate(CAN_InstanceTable[CAN_InitStruct->instance], CAN_InitStruct->baudrate);
	CAN_InstanceTable[CAN_InitStruct->instance]->MCR &= ~(CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK);
    
    /* enable module */
    CAN_InstanceTable[CAN_InitStruct->instance]->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR)));
	while(((CAN_InstanceTable[CAN_InitStruct->instance]->MCR)&CAN_MCR_NOTRDY_MASK));
}

uint32_t CAN_QuickInit(uint32_t CANxMAP, CAN_Baudrate_Type baudrate)
{
	uint32_t i;
    QuickInit_Type * pq = (QuickInit_Type*)&(CANxMAP); 
    CAN_InitTypeDef CAN_InitSturct1;
    CAN_InitSturct1.instance = pq->ip_instance;
    CAN_InitSturct1.baudrate = baudrate;
    CAN_Init(&CAN_InitSturct1);
    /* init pinmux */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
    return pq->ip_instance;
}

uint32_t CAN_IsMessageBoxBusy(uint32_t instance, uint32_t mb)
{
    uint32_t code;
    code = CAN_GET_MB_CODE(CAN_InstanceTable[instance]->MB[mb].CS);
    if((code == kFlexCanTX_Inactive) || (code == kFlexCanRX_Inactive))
    {
        return 0;
    }
    return 1;
}

uint32_t CAN_WriteData(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len)
{
    uint32_t i;
	uint32_t word[2] = {0};
	if(mb >= CAN_MB_MAX || len > 8)
	{
		return 1;
	}
    if(CAN_IsMessageBoxBusy(instance, mb))
    {
        return 2;
    }
    /* clear IT pending bit */
    CAN_InstanceTable[instance]->IFLAG1 |= (1 << mb);
    /* setting data */
	for(i = 0; i < len; i++)
	{
        if(i<4)
        {
            word[0] |= (*(buf+i)<<((3-i)*8));
        }
        else
        {
            word[1] |= (*(buf+i)<<((7-i)*8));  
        }
	}
    CAN_InstanceTable[instance]->MB[mb].WORD0 = word[0];
    CAN_InstanceTable[instance]->MB[mb].WORD1 = word[1];
    /* DLC field */
    CAN_InstanceTable[instance]->MB[mb].CS &= ~CAN_CS_DLC_MASK;
    CAN_InstanceTable[instance]->MB[mb].CS |= CAN_CS_DLC(len);
    /* clear RTR */
    CAN_InstanceTable[instance]->MB[mb].CS &= ~CAN_CS_RTR_MASK;
    /* ID and IDE */
    if(id > 0x7FF)
    {
        /* SRR must set to 1 */
        CAN_InstanceTable[instance]->MB[mb].CS |= CAN_CS_SRR_MASK;
        CAN_InstanceTable[instance]->MB[mb].ID |= (id & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
        CAN_InstanceTable[instance]->MB[mb].CS |= CAN_CS_IDE_MASK;
    }
    else
    {
        CAN_InstanceTable[instance]->MB[mb].ID |= CAN_ID_STD(id);
        CAN_InstanceTable[instance]->MB[mb].CS &= ~CAN_CS_IDE_MASK;
    }
    /* start transfer */
    CAN_InstanceTable[instance]->MB[mb].CS &= ~CAN_CS_CODE_MASK;
    CAN_InstanceTable[instance]->MB[mb].CS |= CAN_CS_CODE(kFlexCanTX_Data);
    return 0;
}

void CAN_CallbackInstall(uint32_t instance, CAN_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        CAN_CallBackTable[instance] = AppCBFun;
    }
}

void CAN_ITDMAConfig(uint32_t instance, uint32_t mb, CAN_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kCAN_IT_Tx_Disable:
            CAN_InstanceTable[instance]->IMASK1 &= ~CAN_IMASK1_BUFLM(1 << mb);
            break;
        case kCAN_IT_Tx:
            CAN_InstanceTable[instance]->IMASK1 |= CAN_IMASK1_BUFLM(1 << mb);
            NVIC_EnableIRQ(CAN_IRQnTable[instance]);
            break;
        case kCAN_IT_Rx_Disable:
            CAN_InstanceTable[instance]->IMASK1 &= ~CAN_IMASK1_BUFLM(1 << mb);
            break;
        case kCAN_IT_RX:
            CAN_InstanceTable[instance]->IMASK1 |= CAN_IMASK1_BUFLM(1 << mb);
            NVIC_EnableIRQ(CAN_IRQnTable[instance]);
            break;
        default:
            break;
    }
}


uint32_t CAN_ReadData(uint32_t instance, uint32_t mb, uint8_t *buf, uint8_t *len)
{
	uint32_t code,i;
    uint8_t len1;
	uint32_t word[2] = {0};
    code = CAN_GET_MB_CODE(CAN_InstanceTable[instance]->MB[mb].CS);
    if((code & 0x01))
    { 
        /* MB is busy and controlled by hardware */
        return 2;
    }
    /* if reviced data */
    if(CAN_InstanceTable[instance]->IFLAG1 & (1<<mb))
    {
        /* clear IT pending bit */
        CAN_InstanceTable[instance]->IFLAG1 |= (1 << mb);
        /* read content */
        len1 = CAN_GET_FRAME_LEN(CAN_InstanceTable[instance]->MB[mb].CS);
        word[0] = CAN_InstanceTable[instance]->MB[mb].WORD0;
        word[1] = CAN_InstanceTable[instance]->MB[mb].WORD1;
        for(i = 0; i < len1; i++)
        {  
           if(i<4)
           (*(buf+i))=(word[0]>>((3-i)*8));
           else							
           (*(buf+i))=(word[1]>>((7-i)*8));
        }
        *len = len1;
        i = CAN_InstanceTable[instance]->TIMER; /* unlock MB */
        return 0;
    }
    i = CAN_InstanceTable[instance]->TIMER; /* unlock MB */
    return 1;
}


void CAN0_ORed_Message_buffer_IRQHandler(void)
{
    uint32_t temp;
    if(CAN_CallBackTable[HW_CAN0])
    {
        CAN_CallBackTable[HW_CAN0]();
    }
    /* make sure clear IT pending bit according to IT enable reg */
    temp = CAN_InstanceTable[HW_CAN0]->IMASK1;
    CAN_InstanceTable[HW_CAN0]->IFLAG1 |= temp;
}

void CAN1_ORed_Message_buffer_IRQHandler(void)
{  
    uint32_t temp;
    if(CAN_CallBackTable[HW_CAN1])
    {
        CAN_CallBackTable[HW_CAN1]();
    } 
    /* make sure clear IT pending bit according to IT enable reg */
    temp = CAN_InstanceTable[HW_CAN1]->IMASK1;
    CAN_InstanceTable[HW_CAN1]->IFLAG1 |= temp;
}


