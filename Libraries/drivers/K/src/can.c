/**
  ******************************************************************************
  * @file    can.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.4.10
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片CAN模块的底层功能函数，具体应用请查看实例程序
             此底层驱动满足基本的can通信模式，不支持先入先出功能
  ******************************************************************************
  */
  
#include "can.h"
#include "gpio.h"

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
/***********************************************************************************************
 功能：接收邮箱中断设置
 形参：can : CAN0  CAN1
 返回：0
 详解：内部参数
************************************************************************************************/
void CAN_Set_Rev_Mask(CAN_Type *can, uint32_t rxid)
{
    can->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
    	// 等待模块进入冻结模式
	while(!(CAN_MCR_FRZACK_MASK & (can->MCR))); 
    if(rxid>0x7FF)
    {	  //识别为扩展id
        can->RXMGMASK = CAN_ID_EXT(rxid); 
    }
    else
    {	//识别为标准11位id
        can->RXMGMASK = CAN_ID_STD(rxid); 
    } 
    can->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK); //取消暂停 
	while((CAN_MCR_FRZACK_MASK & (can->MCR)));   // 等待模块推出冻结模式
}
/***********************************************************************************************
 功能：启动CAN接收邮箱
 形参：can : CAN0 或者  CAN1
       boxindex：0~15
       rxid：准备接收数据源的地址
 返回：0
 详解：采用中断方式接收
************************************************************************************************/
void CAN_EnableRev(CAN_Type *can, uint8_t boxindex, uint32_t rxid)
{
	CAN_Set_Rev_Mask(can,rxid);
    can->IMASK1 |= CAN_IMASK1_BUFLM(1<<boxindex);  //开启接收中断
    // 写入 ID
    if(rxid>0x7FF)
    {	  //识别为扩展id   
        can->MB[boxindex].CS |= CAN_CS_IDE_MASK;
        can->MB[boxindex].CS &= ~CAN_CS_SRR_MASK;
        can->MB[boxindex].ID &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
        can->MB[boxindex].ID |= (rxid & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
    }
    else
    {	//识别为标准11位id
        can->MB[boxindex].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        can->MB[boxindex].ID &= ~CAN_ID_STD_MASK;
        can->MB[boxindex].ID |= CAN_ID_STD(rxid);       
    } 
    can->MB[boxindex].CS &= ~CAN_CS_CODE_MASK; // 
	can->MB[boxindex].CS |= CAN_CS_CODE(4); // 激活这个 MB 作为接受数据
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
	while(CAN_MCR_SOFTRST_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR)); 
    /* init all mb */
    for(i = 0; i < 16; i++)
	{
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].CS = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].ID = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].WORD0 = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].WORD1 = 0x00000000;
	}
	//接收邮箱屏蔽
	CAN_InstanceTable[CAN_InitStruct->instance]->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
    CAN_InstanceTable[CAN_InitStruct->instance]->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
    CAN_InstanceTable[CAN_InitStruct->instance]->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK); 
    
   //设置传输速率
	CAN_SetBaudrate(CAN_InstanceTable[CAN_InitStruct->instance], CAN_InitStruct->baudrate);
	CAN_InstanceTable[CAN_InitStruct->instance]->MCR &= ~(CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK); //取消暂停 
	while((CAN_MCR_FRZACK_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR)));   // 等待模块推出冻结模式
	while(((CAN_InstanceTable[CAN_InitStruct->instance]->MCR)&CAN_MCR_NOTRDY_MASK));    // 等待同步，见参考手册k10 1046页 

}

void CAN_QuickInit(uint32_t CANxMAP, CAN_Baudrate_Type baudrate)
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
}

uint32_t CAN_SendData2(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len)
{
	if(mb >= 16 || len >8)
	{
		return 1; //输入数据错误
	} 
}

/*
void CAN_Init2(uint32_t CANxMAP, CAN_Baudrate_Type baudrate)
{
	uint8_t i;
    CAN_Type *can;
	//记录CANdev的数据
	//开启CAN时钟 配置对应引脚
     QuickInit_Type * pq = (QuickInit_Type*)&(CANxMAP);
    
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
	if((pq->ip_instance) == HW_CAN0)
	{
		can = CAN0;
        SIM->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;	 //开启CAN0的时钟
		NVIC_EnableIRQ(CAN0_ORed_Message_buffer_IRQn);//开启中断
	}
	if((pq->ip_instance) == HW_CAN1)
	{
		can = CAN1;
        SIM->SCGC3 |= SIM_SCGC3_FLEXCAN1_MASK;	//开启can1的的时钟
		NVIC_EnableIRQ(CAN1_ORed_Message_buffer_IRQn);//开启中断
	}
	//选择为BusClock 时钟
	can->CTRL1 |=  CAN_CTRL1_CLKSRC_MASK;	
	//开启CAN通信模块 
	can->MCR &= ~CAN_MCR_MDIS_MASK;	
	//等待通信模块复位启动
	while((CAN_MCR_LPMACK_MASK & (can->MCR)));	 
	//软件复位模块 //等待软件复位完成
	can->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (can->MCR));  
    //使能冻结模式
	can->MCR |= (CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK);  
	// 等待模块进入冻结模式
	while(!(CAN_MCR_FRZACK_MASK & (can->MCR))); 
    //初始化所有邮箱
	for(i=0;i<16;i++)
	{
		can->MB[i].CS = 0x00000000;
		can->MB[i].ID = 0x00000000;
		can->MB[i].WORD0 = 0x00000000;
		can->MB[i].WORD1 = 0x00000000;
	}
	//接收邮箱屏蔽
	can->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
    can->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
    can->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK); 
   //设置传输速率
	CAN_SetBaudrate(can,baudrate);
	can->MCR &= ~(CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK); //取消暂停 
	while((CAN_MCR_FRZACK_MASK & (can->MCR)));   // 等待模块推出冻结模式
	while(((can->MCR)&CAN_MCR_NOTRDY_MASK));    // 等待同步，见参考手册k10 1046页 
}
*/
/***********************************************************************************************
 功能：CAN 设备发送数据
 形参：can : CAN0 或者  CAN1
       Data: 数据指针 
 返回：0
 详解: 发送数据最大不超过 8字节
************************************************************************************************/
uint8_t CAN_SendData(CAN_Type *can,uint8_t boxindex, uint32_t txid, uint8_t *Data, uint8_t len)
{
	uint8_t  i;
	uint32_t word[2] = {0};
    can->IMASK1 |= CAN_IMASK1_BUFLM(1<<boxindex);  //开启接收中断
	if(boxindex >= 16 || len >8)
	{
		return 1; //输入数据错误
	}
	// 转换数据格式
	for(i=0;i<len;i++)
	{
		if(i<4)
		word[0] |= (*(Data+i)<<((3-i)*8));
	   else
		word[1] |= (*(Data+i)<<((7-i)*8));
	} 
    can->MB[boxindex].WORD0 = word[0];
	can->MB[boxindex].WORD1 = word[1]; 
	//输入id处理
	if(txid>0x7FF)
    {	  //识别为扩展id
        can->MB[boxindex].ID &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);  
        can->MB[boxindex].ID |= (txid & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
        can->MB[boxindex].CS |= CAN_CS_IDE_MASK;
        can->MB[boxindex].CS &= ~CAN_CS_SRR_MASK;
        can->MB[boxindex].CS &= ~CAN_CS_DLC_MASK;
        can->MB[boxindex].CS |= CAN_CS_DLC(len);
        can->MB[boxindex].CS &= ~CAN_CS_CODE_MASK;
        can->MB[boxindex].CS |= CAN_CS_CODE(12);	 // 激活这个 MB 发送数据
    }
    else
    {	//识别为标准11位id
        can->MB[boxindex].ID &= ~CAN_ID_STD_MASK ;  
        can->MB[boxindex].ID |= CAN_ID_STD(txid);
        can->MB[boxindex].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        can->MB[boxindex].CS &= ~CAN_CS_DLC_MASK;
        can->MB[boxindex].CS |= CAN_CS_DLC(len);
        can->MB[boxindex].CS &= ~CAN_CS_CODE_MASK;
        can->MB[boxindex].CS |= CAN_CS_CODE(12);	 // 激活这个 MB 发送数据  
    } 
    return 0;
}  
/***********************************************************************************************
 功能：CAN接收数据
 形参：can : CAN0 或者  CAN1
       boxindex:邮箱编号0~15
       *data : 接收数据缓冲区
 返回：接收到的数据长度
 详解：
************************************************************************************************/
uint8_t CAN_ReadData(CAN_Type *can,uint8_t boxindex, uint8_t *Data)
{
	uint8_t len = 0;
	uint8_t code,i;
	uint32_t word[2] = {0};
    // 锁 MB
    code = CAN_get_code(can->MB[boxindex].CS);
    if(code != 0x02) //验证是否接收成功
    {
        len = 0;
        return len;
    }
    len = CAN_get_length(can->MB[boxindex].CS);  //获得接收的数据长度
    if(len <1)
    {
        return len; //接收失败
    }
	word[0] = can->MB[boxindex].WORD0;   //读取接收的数据
	word[1] = can->MB[boxindex].WORD1;   //读取接收的数据
	code = can->TIMER;    // 全局解锁 MB 操作
	//清报文缓冲区中断标志
	can->IFLAG1 = (1<<boxindex);	 //必须清除
	for(i=0;i<len;i++)
    {  
	   if(i<4)
	   (*(Data+i))=(word[0]>>((3-i)*8));
	   else									 //数据存储转换
	   (*(Data+i))=(word[1]>>((7-i)*8));
    }
    return len;
}
/***********************************************************************************************
 功能：CAN使能接收中断
 形参：can : CAN0 或者  CAN1
 返回：0
 详解：0
************************************************************************************************/
void CAN_AbleInterrupts(CAN_Type * can)
{
	switch((uint32_t)can)
	{
		case CAN0_BASE:
			NVIC_EnableIRQ(CAN0_ORed_Message_buffer_IRQn);
			break;
		case CAN1_BASE:
			NVIC_EnableIRQ(CAN1_ORed_Message_buffer_IRQn);
			break;
		default:break;
	}
}
/***********************************************************************************************
 功能：CAN禁止接收中断
 形参：can : CAN0 或者  CAN1
 返回：0
 详解：0
************************************************************************************************/
void CAN_DisableInterrupts(CAN_Type * can)
{
	switch((uint32_t)can)
	{
		case CAN0_BASE:
			NVIC_DisableIRQ(CAN0_ORed_Message_buffer_IRQn);
			break;
		case CAN1_BASE:
			NVIC_DisableIRQ(CAN1_ORed_Message_buffer_IRQn);
			break;
		default:break;
	}
}
/***********************************************************************************************
 功能：CAN清楚中断标志位
 形参：can : CAN0 或者  CAN1
 返回：0
 详解：0
************************************************************************************************/
void CAN_ClearRecFlag(CAN_Type * can, uint8_t boxindex)
{
	can->IFLAG1 |= (1<<boxindex);	 //必须清除
}

//extern uint8_t CANRXBuffer[8];
//extern uint32_t CAN_Data_Len;
/***********************************************************************************************
 功能：CAN0 接收中断
 形参：0
 返回：0
 详解：0
************************************************************************************************/
void CAN0_ORed_Message_buffer_IRQHandler(void)
{  
    CAN_ClearRecFlag(CAN0,1); //清除接收标志位
}
/***********************************************************************************************
 功能：CAN1 接收中断
 形参：0
 返回：0
 详解：0
************************************************************************************************/
void CAN1_ORed_Message_buffer_IRQHandler(void)
{  
    
   if((CAN1->IFLAG1)&(1<<CAN_DEFAULT_RXMSGBOXINDEX))
   {
//     CAN_Data_Len = CAN_ReadData(CAN1,CAN_DEFAULT_RXMSGBOXINDEX,CANRXBuffer); //接收数据
     CAN_ClearRecFlag(CAN1,CAN_DEFAULT_RXMSGBOXINDEX); //清除接收标志位
   }
   if((CAN1->IFLAG1)&(0x01<<8))
   {
    CAN_ClearRecFlag(CAN1,8); //清除接收标志位
   }
    
}


