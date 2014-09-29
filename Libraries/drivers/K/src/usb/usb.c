#include "usb.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "message_manage.h"

//BDT缓冲区，理论上只要申请512字节，但是由于缓冲区地址的低9位必须为0（不然USB模块无法使用）
//所以申请1024从中找出地址低9位为0的缓冲区地址作为首地址。
#ifdef __ICCARM__ 
#pragma data_alignment = 512
tBDT tBDTtable[16];
#endif

#ifdef __CC_ARM
__align(512) tBDT tBDTtable[16];													//内部SRAM内存池
#endif


//各个端点的数据缓冲区
uint8_t guint8_tEP0_OUT_ODD_Buffer[EP0_SIZE];
uint8_t guint8_tEP0_OUT_EVEN_Buffer[EP0_SIZE];
uint8_t guint8_tEP0_IN_ODD_Buffer[EP0_SIZE];
uint8_t guint8_tEP0_IN_EVEN_Buffer[EP0_SIZE];
uint8_t guint8_tEP1_OUT_ODD_Buffer[EP1_SIZE];
uint8_t guint8_tEP1_OUT_EVEN_Buffer[EP1_SIZE];
uint8_t guint8_tEP1_IN_ODD_Buffer[EP1_SIZE];
uint8_t guint8_tEP1_IN_EVEN_Buffer[EP1_SIZE];
uint8_t guint8_tEP2_OUT_ODD_Buffer[EP2_SIZE];
uint8_t guint8_tEP2_OUT_EVEN_Buffer[EP2_SIZE];
uint8_t guint8_tEP2_IN_ODD_Buffer[EP2_SIZE];
uint8_t guint8_tEP2_IN_EVEN_Buffer[EP2_SIZE];
uint8_t guint8_tEP3_OUT_ODD_Buffer[EP3_SIZE];
uint8_t guint8_tEP3_OUT_EVEN_Buffer[EP3_SIZE];
uint8_t guint8_tEP3_IN_ODD_Buffer[EP3_SIZE];
uint8_t guint8_tEP3_IN_EVEN_Buffer[EP3_SIZE];
//指向各个缓冲区的地址指针
uint8_t *BufferPointer[]=
{
    guint8_tEP0_OUT_ODD_Buffer,
    guint8_tEP0_OUT_EVEN_Buffer,
    guint8_tEP0_IN_ODD_Buffer,
    guint8_tEP0_IN_EVEN_Buffer,
    guint8_tEP1_OUT_ODD_Buffer,
    guint8_tEP1_OUT_EVEN_Buffer,
    guint8_tEP1_IN_ODD_Buffer,
    guint8_tEP1_IN_EVEN_Buffer,
    guint8_tEP2_OUT_ODD_Buffer,
    guint8_tEP2_OUT_EVEN_Buffer,
    guint8_tEP2_IN_ODD_Buffer,
    guint8_tEP2_IN_EVEN_Buffer,
    guint8_tEP3_OUT_ODD_Buffer,
    guint8_tEP3_OUT_EVEN_Buffer,
    guint8_tEP3_IN_ODD_Buffer,
    guint8_tEP3_IN_EVEN_Buffer
};
//每个缓冲区大小
const uint8_t cEP_Size[]=
{
    EP0_SIZE,    
    EP0_SIZE,    
    EP0_SIZE,    
    EP0_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP3_SIZE,
    EP3_SIZE,
    EP3_SIZE,
    EP3_SIZE
};
//StringDesc描述符指针
const uint8_t* String_Table[4]=
{
    String_Descriptor0,
    String_Descriptor1,
    String_Descriptor2,
    String_Descriptor3
};
//SETUP包后面只能跟DATA0
//端点IN(2)状态
uint8_t vEP2State = kUDATA1;
//端点OUT(3)状态
uint8_t vEP3State = kUDATA0;

//USB模块内部全局变量
uint8_t guint8_tUSBClearFlags;            //内部使用
uint8_t *puint8_tIN_DataPointer;          //内部使用
uint8_t guint8_tIN_Counter;               //内部使用
uint8_t guint8_tUSB_Toogle_flags;         //内部使用
uint8_t guint8_tUSB_State;                //保存USB端点0的状态
tUSB_Setup *Setup_Pkt;             //指向端点0OUT数据首地址


/***********************************************************************************************
 功能：等待USB模块被枚举成功 
 形参：0          
 返回：0
 详解：当初始化USB设备后，调用此函数，则程序会一直等待 USB枚举成功
************************************************************************************************/
void USB_WaitDeviceEnumed(void)
{
  while(guint8_tUSB_State != uENUMERATED);//等待USB设备被枚举
}
/***********************************************************************************************
 功能：判断USB模块时候被成功枚举
 形参：0          
 返回：0 枚举已经成功  1 枚举失败 或者 还未枚举完成
 详解：
************************************************************************************************/
uint8_t USB_IsDeviceEnumed(void)
{
	if(guint8_tUSB_State == uENUMERATED) 
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
/***********************************************************************************************
 功能：端点输入中断发送数据函数
 形参：uint8_tEP: 端点号
       puint8_tDataPointer: 发送数据缓冲区
       uint8_tDataSize: 需要发送的数据长度
 返回：0 
 详解：
*   端点IN传送，在不同USB模式下IN的含义不同
*     在USB主机模式下（USB host）  IN表示  USB Host接收 USB Device的数据包
*     在USB设备模式下（USB Device）IN表示  USB Host向 USB Device发送数据包
*   当前在USB Device模式下，该函数的作用是Device设备发送数据包
************************************************************************************************/
void USB_EP_IN_Transfer(uint8_t uint8_tEP,uint8_t *puint8_tDataPointer,uint8_t uint8_tDataSize)
{
  uint8_t *puint8_tEPBuffer;
  uint8_t uint8_tEPSize;     //端点的数据长度
  uint16_t uint16_tLenght=0;    
  uint8_t uint8_tEndPointFlag;    
  /*调整当前缓冲区的位置*/
  uint8_tEndPointFlag=uint8_tEP;
  if(uint8_tEP)   //如果不是端点0,
    uint8_tEP=(uint8_t)(uint8_tEP<<2);
  uint8_tEP+=2; //EP值加2
  puint8_tEPBuffer=BufferPointer[uint8_tEP];   //将新EP的BUFFER地址给puint8_tEPBuffer
  
  if(BIT_CHK(fIN,guint8_tUSBClearFlags)) //如果guint8_tUSBClearFlags = 1
  {
      puint8_tIN_DataPointer = puint8_tDataPointer; //将用户数据存储区的首地址给puint8_tIN_DataPointer
      guint8_tIN_Counter = uint8_tDataSize;         //将用户数据长度给guint8_tIN_Counter

      uint16_tLenght=(Setup_Pkt->wLength_h<<8)+Setup_Pkt->wLength_l ;//将setup数据的长度给uint16_tLenght
      if((uint16_tLenght < uint8_tDataSize) && (uint8_tEP==2)) //如果所发送的数据长度大于setup所设定的长度 同时 端点值 == 2
      {
          guint8_tIN_Counter=Setup_Pkt->wLength_l; //只发送setup中的低8位长度
      }
  }
  /*检查发送长度*/
  if(guint8_tIN_Counter > cEP_Size[uint8_tEP]) //如果发送数据包的长度 大于32字节时
  {
      uint8_tEPSize = cEP_Size[uint8_tEP];     //将此时端点的长度限制在端点的默认长度
      guint8_tIN_Counter-=cEP_Size[uint8_tEP]; //将数据包的长度减少EP_Size
      BIT_CLR(fIN,guint8_tUSBClearFlags);//将guint8_tUSBClearFlags清零
  }
  else
  { 
      uint8_tEPSize = guint8_tIN_Counter;      //如果小于
      guint8_tIN_Counter=0;            
      BIT_SET(fIN,guint8_tUSBClearFlags);//将guint8_tUSBClearFlags置一
  }
  /*将用户缓冲的区的值复制到EP 缓冲区中准备发送*/
  tBDTtable[uint8_tEP].Cnt=(uint8_tEPSize);    //复制所要发送的数据长度
  while(uint8_tEPSize--)
       *puint8_tEPBuffer++=*puint8_tIN_DataPointer++; //将用户的数据赋值给EP存储区                                                        
  if(BIT_CHK(uint8_tEndPointFlag,guint8_tUSB_Toogle_flags)) //如果相应端点的guint8_tUSB_Toogle_flags == 1
  {
      tBDTtable[uint8_tEP].Stat._byte= kUDATA0;         
      BIT_CLR(uint8_tEndPointFlag,guint8_tUSB_Toogle_flags);//清零相应端点的guint8_tUSB_Toogle_flags
  } 
  else
  {
      tBDTtable[uint8_tEP].Stat._byte= kUDATA1;          
      BIT_SET(uint8_tEndPointFlag,guint8_tUSB_Toogle_flags);//置位guint8_tUSB_Toogle_flags
  }
}
/***********************************************************************************************
 功能：检查OUT端点接收到的数据量
 形参：uint8_tEP: 端点号
 返回：接收到的数据长度
 详解：检查OUT端点接收到的数据量
************************************************************************************************/
uint16_t USB_EP_OUT_SizeCheck(uint8_t uint8_tEP)
{
  uint8_t uint8_tEPSize; 
   /* 读取缓冲区的长度 */
  uint8_tEPSize = tBDTtable[uint8_tEP<<2].Cnt;
  return(uint8_tEPSize & 0x03FF);
}
//=========================================================================
//函数名: hw_usb_ep0_stall
//功  能: 端点0产生一个STALL包
//参  数: 无  
//返  回: 无
//=========================================================================

void hw_usb_ep0_stall(void)
{
	// 产生一个STALL包  
	#if (DEBUG_PRINT == 1)
	UART_printf("发送一个stall包\r\n");
	#endif
	BIT_SET(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
	//ENDPT0_EP_STALL = 1;                      
	tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA0; 
	tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;       
}
/***********************************************************************************************
 功能：设置SIE(K60USB串行接口引擎)非EP0 外的端点属性
 形参：0
 返回：0
 详解：0
************************************************************************************************/
void USB_EnableInterface(void)
{
    // 使能端点
    // 端点 Register 设置
//	USB0->ENDPOINT[1].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;              
	USB0->ENDPOINT[2].ENDPT=_EP_IN  | USB_ENDPT_EPHSHK_MASK;    //使能RX 接受
	USB0->ENDPOINT[3].ENDPT=_EP_OUT | USB_ENDPT_EPHSHK_MASK;    //使能TX 发送
	//USB0->ENDPOINT[4].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;   
	//USB0->ENDPOINT[5].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;   
	//USB0->ENDPOINT[6].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;   
	                   
  
    // 设置 1 BDT 设置
    // 把控制权交给MCU 
    //tBDTtable[bEP1IN_ODD].Stat._byte= kUDATA1;                //发送DATA1数据包
    //tBDTtable[bEP1IN_ODD].Cnt = 0x00;                         //计数器清空
    //tBDTtable[bEP1IN_ODD].Addr =(uint32_t)guint8_tEP1_IN_ODD_Buffer;    //地址指向对应的缓冲区

    // 设置 2 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].Addr =(uint32_t  )guint8_tEP2_IN_ODD_Buffer;            

    // 设置 3 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].Addr =(uint32_t)guint8_tEP3_OUT_ODD_Buffer;            
}

/***********************************************************************************************
 功能：USB中断散转-EP0_OUT散转-Host获取描述符中断处理
 形参：0
 返回：0
 详解：0
************************************************************************************************/
void USB_GetDescHandler(void)
{
	switch((Setup_Pkt->wValue_h) & 0xFF)
	{
		case DEVICE_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			UART_printf("设备描述符\r\n");
			#endif
			USB_EP_IN_Transfer(EP0,(uint8_t*)Device_Descriptor,sizeof(Device_Descriptor));//发送设备描述符
			break;
		case CONFIGURATION_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			UART_printf("配置描述符\r\n");
			#endif
			USB_EP_IN_Transfer(EP0,(uint8_t*)Configuration_Descriptor,sizeof(Configuration_Descriptor)); //发送配置描述符
			break;
		case STRING_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			UART_printf("字符串描述符-%x ",Setup_Pkt->wValue_l);
			#endif
			switch(Setup_Pkt->wValue_l)  //根据wValue的低字节（索引值）散转
			{
				case 0:  //获取语言ID
					#if (DEBUG_PRINT == 1)
					UART_printf("-语言ID\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 1:  //厂商字符串的索引值为1，所以这里为厂商字符串
					#if (DEBUG_PRINT == 1)
					UART_printf("-厂商字符串\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 2:  //产品字符串的索引值为2，所以这里为产品字符串
					#if (DEBUG_PRINT == 1)
					UART_printf("-产品字符串\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 3:  //产品序列号的索引值为3，所以这里为序列号
					#if (DEBUG_PRINT == 1)
					UART_printf("-序列号\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				default: 
					#if (DEBUG_PRINT == 1)
					UART_printf("-位置的索引值\r\n");
					#endif
					break; 
			}
			break;
				case REPORT_DESCRIPTOR:
					#if (DEBUG_PRINT == 1)
					UART_printf("-报告描述符\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)Report_Descriptor,sizeof(Report_Descriptor));
					break;
		default:
			#if (DEBUG_PRINT == 1)
			UART_printf("其他描述符,描述符代码:0x%x\r\n",Setup_Pkt->wValue_h);
			#endif
			break;
	}
}

/***********************************************************************************************
 功能：USB中断散转-EP0输出请求中断
 形参：0
 返回：0
 详解：对SETUP包解包和处理
		(1)只有接收到SETUP包才调用该函数
		(2)SETUP包中8字节数据
			bmRequestType:1
			bRequest:1
			wValue.H:1 :描述符的类型
			wValue.L:1 :描述符的索引
			wIndex:2
			wLength:2
************************************************************************************************/
void USB_EP0_OUT_Handler(void)
{
	uint8_t *p =(uint8_t*)Setup_Pkt;
	#if (DEBUG_PRINT == 1)
	uint8_t i;
	#endif

	*p = *p;
	// 从DATA0 开始传输。
	BIT_CLR(0,guint8_tUSB_Toogle_flags);
	//打印接收到的数据
	#if (DEBUG_PRINT == 1)
	for(i=0;i<8;i++,p++) UART_printf("0x%x ",*p); 	UART_printf("\r\n");
	#endif
	if((Setup_Pkt->bmRequestType & 0x80) == 0x80)
	{
		//根据bmRequestType的D6-5位散转，D6-5位表示请求的类型
		//0为标准请求，1为类请求，2为厂商请求
	  switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB标准输入请求-");
				#endif
				//USB协议定义了几个标准输入请求，实现这些标准输入请求即可
				//请求的代码在bRequest中，对不同的请求代码进行散转
				switch(Setup_Pkt->bRequest)
				{
					case mGET_CONFIG:
						#if (DEBUG_PRINT == 1)
						UART_printf("获取配置\r\n");
						#endif
						break;	
					case mGET_DESC:
						#if (DEBUG_PRINT == 1)
						UART_printf("获取描述符-");
						#endif
						USB_GetDescHandler(); //执行获取描述符
						break;
					case mGET_INTF:
						#if (DEBUG_PRINT == 1)
						UART_printf("获取接口\r\n");
						#endif
						break;
					case mGET_STATUS:
						#if (DEBUG_PRINT == 1)
						UART_printf("获取状态\r\n");
						#endif
						break;
					case mSYNC_FRAME:
						#if (DEBUG_PRINT == 1)
						UART_printf("同步帧\r\n");
						#endif
						break;
						default:
						#if (DEBUG_PRINT == 1)
						UART_printf("错误：未定义的标准输入请求\r\n");
						#endif
						break;
				}
				break;
			case 1:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB类输入请求\r\n");
				#endif
				break;
			case 2:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB厂商输入请求\r\n");
				#endif
				break;
			default:
				#if (DEBUG_PRINT == 1)
				UART_printf("错误：未定义的输入请求\r\n");
				#endif
				break;
		}	
	}
	else
	{
		//根据bmRequestType的D6-5位散转,D6-5位表示请求的类型
		//0为标准请求，1为类型求 2为厂商请求
		switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB 标准输出请求-");
				#endif
				switch(Setup_Pkt->bRequest)
				{
					case mCLR_FEATURE:
						#if (DEBUG_PRINT == 1)
						UART_printf("清除特性\r\n");
						#endif
						break;
					case mSET_ADDRESS:
						#if (DEBUG_PRINT == 1)
						UART_printf("设置地址 地址:%x\r\n",Setup_Pkt->wValue_l);
						#endif
						guint8_tUSB_State=uADDRESS;
						USB_EP_IN_Transfer(EP0,0,0); //为什么？？？
						break;
					case mSET_CONFIG:
						#if (DEBUG_PRINT == 1)
						UART_printf("设置配置\r\n");
						#endif
            if(Setup_Pkt->wValue_h+Setup_Pkt->wValue_l) 
            {
                //使能1 、2 、3 端点 
							USB_EnableInterface();
							USB_EP_IN_Transfer(EP0,0,0);
							guint8_tUSB_State=uENUMERATED;
            }
						break;
					case  mSET_DESC:
						#if (DEBUG_PRINT == 1)
						UART_printf("设置描述符\r\n");
						#endif
						break;
					case mSET_INTF:
						#if (DEBUG_PRINT == 1)
						UART_printf("设置接口\r\n");
						#endif
						break;
					default:
						#if (DEBUG_PRINT == 1)
						UART_printf("错误：未定义的标准输出请求\r\n");
						#endif
					break;
				}
				break;
				case 1:
					#if (DEBUG_PRINT == 1)
					UART_printf("USB类输出请求-\r\n");
					#endif
				/*
					switch(Setup_Pkt->bRequest)
					{
						case 0x0A:
							USB_EP_IN_Transfer(EP0,0,0); //等待，发送0数据包即可
							UART_printf("设置空闲\r\n");
							break;
						default:
							UART_printf("未知的请求\r\n");
							break;
					}
				*/
					break;
				case 2:
					#if (DEBUG_PRINT == 1)
					UART_printf("USB厂商输出请求r\n");
					#endif
					break;
				default:
					#if (DEBUG_PRINT == 1)
					UART_printf("错误：未定义的输出请求\r\n");
					#endif
				break;
		}
	}
	tBDTtable[bEP0OUT_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL); // 为0 时: SIE 继续处理令牌
}


static int USB_SetClockDiv(uint32_t srcClock)
{
    uint8_t usbfrac_max, usbdiv_max,frac,div;
    usbfrac_max = 1;
    usbdiv_max = 7;
    
    /* clear all divivder */
    SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBDIV_MASK;
    SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBFRAC_MASK;
    
    for(frac=0;frac<usbfrac_max;frac++)
    {
        for(div=0;div<usbdiv_max;div++)
        {
            if((srcClock*(frac+1))/(div+1) == 48000000)
            {
                SIM->CLKDIV2 |= SIM_CLKDIV2_USBDIV(div);
                (frac)?(SIM->CLKDIV2 |= SIM_CLKDIV2_USBFRAC_MASK):(SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBFRAC_MASK);
                LIB_TRACE("USB clock OK src:%d frac:%d div:%d 0x%08X\r\n", srcClock, frac, div, SIM->CLKDIV2);
                return 0;
            }
        }
    }
    return 1;
}

uint8_t USB_Init(void)
{
    Setup_Pkt=(tUSB_Setup*)BufferPointer[bEP0OUT_ODD];
    guint8_tUSB_State=uPOWER;      

    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    MPU->CESR=0;
    
    /* clock config */
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK;
    uint32_t clock;
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    if(USB_SetClockDiv(clock))
    {
        LIB_TRACE("USB clock setup fail\r\n");
    }
    (MCG->C6 & MCG_C6_PLLS_MASK)?
    (SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK):   /* PLL */
    (SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK);  /* FLL */

    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    USB0->USBTRC0 = 0x40; 

	//设置BDT基址寄存器
	//( 低9 位是默认512 字节的偏移) 512 = 16 * 4 * 8 。
	//8 位表示: 4 个字节的控制状态，4 个字节的缓冲区地址
        
#if 0
    
    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(BIT_CHK(USB_USBTRC0_USBRESET_SHIFT,USB0->USBTRC0)){};
    
    /* set BDT table address */
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);
        
    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;       //D-  D+ 下拉
	USB0->USBTRC0 |= 0x40;                      //强制设置第6位为1  真是纠结，DS上就这么写的

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
#endif
    NVIC_EnableIRQ(USB0_IRQn); //使能USB模块IRQ中断
	return 0;
}


void USB_DisConnect(void)
{
	USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void USB_Connect(void)
{
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
}


void USB_ResetHandler(void)
{
    //清标志
    guint8_tUSBClearFlags=0xFF;
    guint8_tUSB_Toogle_flags=0;
    //禁止所有端点 0端点除外
    USB0->ENDPOINT[1].ENDPT=0x00;
	USB0->ENDPOINT[2].ENDPT=0x00;
	USB0->ENDPOINT[3].ENDPT=0x00;
	USB0->ENDPOINT[4].ENDPT=0x00;
	USB0->ENDPOINT[5].ENDPT=0x00;
	USB0->ENDPOINT[6].ENDPT=0x00;
    /*端点0 BDT 启动端点设置*/
    tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;   // EP0 OUT BDT 设置
    tBDTtable[bEP0OUT_ODD].Addr =(uint32_t)guint8_tEP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;//USB-FS(硬件USB模块) 有专有权访问 BD
                                              //使能USB-FS去扮演数据翻转同步
                                              //定义DATA1允许发送或者接收        
   
    tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE; // EP0 OUT BDT 设置
    tBDTtable[bEP0OUT_EVEN].Addr =(uint32_t)guint8_tEP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;//USB-FS(硬件USB模块) 有专有权访问 BD
                                               //使能USB-FS去扮演数据翻转同步
                                               //定义DATA1允许发送或者接收       
   
    tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;   // EP0 IN BDT 设置     
    tBDTtable[bEP0IN_ODD].Addr =(uint32_t)guint8_tEP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;//USB-FS(硬件USB模块) 有专有权访问 BD
                                             //使能USB-FS去扮演数据翻转同步
                                             //定义DATA0允许发送或者接收 
   
    tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE;  // EP0 IN BDT 设置            
    tBDTtable[bEP0IN_EVEN].Addr =(uint32_t)guint8_tEP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;//USB-FS(硬件USB模块) 有专有权访问 BD
                                              //使能USB-FS去扮演数据翻转同步
                                              //定义DATA0允许发送或者接收          
    USB0->ENDPOINT[0].ENDPT=0x0D; // 使能 EP0 开启发送 接受时能
    USB0->ERRSTAT=0xFF;           // 清除所有的错误
	USB0->ISTAT=0xFF;             // 清除所有的中断标志
	USB0->ADDR=0x00;  					  // USB枚举时的默认设备地址0
	USB0->ERREN=0xFF;             // 使能所有的错误中断
	// USB模块中断使能
	BIT_SET(USB_INTEN_TOKDNEEN_SHIFT,USB0->INTEN);
	BIT_SET(USB_INTEN_SOFTOKEN_SHIFT,USB0->INTEN);
	BIT_SET(USB_INTEN_ERROREN_SHIFT,USB0->INTEN); 
	BIT_SET(USB_INTEN_USBRSTEN_SHIFT,USB0->INTEN);    
}

/***********************************************************************************************
 功能：USB中断散转-EP0输入中断请求
 形参：0          
 返回：0 
 详解：
************************************************************************************************/
void USB_EP0_IN_Handler(void)
{
    if(guint8_tUSB_State==uADDRESS)
    {
        USB0->ADDR = Setup_Pkt->wValue_l; //写入地址
        guint8_tUSB_State=uREADY;              //变为Ready状态
        BIT_SET(fIN,guint8_tUSBClearFlags);   //
				#if (DEBUG_PRINT == 1)
				UART_printf("新地址：%d\r\n",USB0->ADDR);
				#endif
    }
		USB_EP_IN_Transfer(EP0,0,0); 
}

/***********************************************************************************************
 功能：USB中断散转-EP2输入中断请求
 形参：0          
 返回：0 
 详解：
************************************************************************************************/
static  MessageType_t m_Msg;
void USB_EP2_IN_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = fIN;
	fn_msg_push(m_Msg); //发送一个消息
	// tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL);
}
/***********************************************************************************************
 功能：USB中断散转-EP2输出中断请求
 形参：0          
 返回：0 
 详解：
************************************************************************************************/
void USB_EP2_OUT_Handler(void)
{
	
}
/***********************************************************************************************
 功能：USB中断散转-EP3输入中断请求
 形参：0          
 返回：0 
 详解：
************************************************************************************************/
void USB_EP3_IN_Handler(void)
{
	
}
/***********************************************************************************************
 功能：USB中断散转-EP3输出中断请求
 形参：0          
 返回：0 
 详解：
************************************************************************************************/
void USB_EP3_OUT_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = fOUT;
	m_Msg.m_MsgLen = USB_EP_OUT_SizeCheck(EP3);
	m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
	fn_msg_push(m_Msg); //发送一个消息
    tBDTtable[EP3<<2].Stat._byte= kSIE;
    tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
}
/***********************************************************************************************
 功能：从USB0_IRQHandler 硬件中断散转而来，继续散转
 形参：0
 返回：0
 详解：从USB0_IRQHandler 硬件中断散转而来，继续进行散转
************************************************************************************************/
void USB_Handler(void)
{
	uint8_t uint8_tEndPoint;
	uint8_t uint8_tIN;
    uint8_tIN = USB0->STAT & 0x08;    //获得当前的传输状态，1发送；0接收
    uint8_tEndPoint = USB0->STAT >> 4;//获得当前接收令牌的端点地址
	if(uint8_tEndPoint == 0) //端点0
	{
		if(uint8_tIN) //IN
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("端点0输入中断处理-");
			#endif
			USB_EP0_IN_Handler(); //端点0发送包
		}
		else    //OUT
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("端点0输出中断处理-");
			#endif
			USB_EP0_OUT_Handler();
		}
	}
	if(uint8_tEndPoint == 2)//端点2
	{
		if(uint8_tIN) //IN
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("端点2输入中断处理\r\n");
			#endif
			USB_EP2_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("端点2输出中断处理\r\n");
			#endif
			USB_EP2_OUT_Handler();
		}	
	}
	if(uint8_tEndPoint == 3)//端点3
	{
		if(uint8_tIN) //IN
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("端点3输入中断处理\r\n");
			#endif
			USB_EP3_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("端点3输入中断处理\r\n");
			#endif
			USB_EP3_OUT_Handler();
		}	
	}
}


void USB0_IRQHandler2(void)
{
	uint8_t err = 0;
	err = err;
    if(USB0->ISTAT & USB_ISTAT_USBRST_MASK)
    {
    	USB0->ISTAT = USB_ISTAT_USBRST_MASK;

        /* Handle RESET Interrupt */
        //USB_Bus_Reset_Handler();

        /* Notify Device Layer of RESET Event */
        //(void)USB_Device_Call_Service(USB_SERVICE_BUS_RESET, &event);

        /* Clearing this bit allows the SIE to continue token processing and clear suspend condition */
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
        LIB_TRACE("bus USBRST\r\n");
        /* No need to process other interrupts */
        return;
    }
    if(USB0->ISTAT & USB_ISTAT_ERROR_MASK)
    {
        LIB_TRACE("bus ERROR\r\n");
    }
    if(USB0->ISTAT & USB_ISTAT_SOFTOK_MASK)
    {
        LIB_TRACE("bus SOFTOK\r\n");
    }
    if(USB0->ISTAT & USB_ISTAT_TOKDNE_MASK)
    {
        LIB_TRACE("bus TOKDNE\r\n");
    }
    if(USB0->ISTAT & USB_ISTAT_SLEEP_MASK)
    {
        /* Clear RESUME Interrupt if Pending */
    	USB0->ISTAT = USB_ISTAT_RESUME_MASK;

        /* Clear SLEEP Interrupt */
    	USB0->ISTAT = USB_ISTAT_SLEEP_MASK;

        /* Notify Device Layer of SLEEP Event */
        //(void)USB_Device_Call_Service(USB_SERVICE_SLEEP, &event);

        /* Set Low Power RESUME enable */
        USB0->USBTRC0 |= USB_USBTRC0_USBRESMEN_MASK;

        /* Set SUSP Bit in USB_CTRL */
        USB0->USBCTRL |= USB_USBCTRL_SUSP_MASK;

        /* Enable RESUME Interrupt */
        USB0->INTEN |= USB_INTEN_RESUMEEN_MASK;
    }
    if(USB0->ISTAT & USB_ISTAT_RESUME_MASK)
    {
        LIB_TRACE("bus RESUME\r\n");
    } 
    if(USB0->ISTAT & USB_ISTAT_ATTACH_MASK)
    {
        LIB_TRACE("bus ATTACH\r\n");
    } 
    if(USB0->ISTAT & USB_ISTAT_STALL_MASK)
    {
        LIB_TRACE("bus STALL\r\n");
    } 
    
    
    
    #if 0
    
	//检测USB 模块是否解析到有效的复位。
	if(BIT_CHK(USB_ISTAT_USBRST_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		UART_printf("USB总线复位\r\n");
		#endif
		USB_ResetHandler();
	}
	//收到SOF包
	if(BIT_CHK(USB_ISTAT_SOFTOK_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		//UART_printf("收到SOF包\r\n");
		#endif
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;   
	}
	//收到STALL包
	if(BIT_CHK(USB_ISTAT_STALL_SHIFT,USB0->ISTAT))
	{
		#if (DEBUG_PRINT == 1)
		UART_printf("收到STALL包\r\n");
		#endif
		if(BIT_CHK(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT))
		BIT_CLR(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
    BIT_SET(USB_ISTAT_STALL_SHIFT,USB0->ISTAT);
	}
	//令牌完成中断
	if(BIT_CHK(USB_ISTAT_TOKDNE_SHIFT,USB0->ISTAT)) 
	{
		BIT_SET(USB_CTL_ODDRST_SHIFT,USB0->CTL);//指向BDT EVEN 组
		USB_Handler(); //调用USB Handler
		//清除令牌完成中断
		BIT_SET(USB_ISTAT_TOKDNE_SHIFT,USB0->ISTAT);
	}
	//SLEEP 
	if(BIT_CHK(USB_ISTAT_SLEEP_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		UART_printf("SLEEP中断\r\n");
		#endif
		BIT_SET(USB_ISTAT_SLEEP_SHIFT,USB0->ISTAT);        
	}
	// 错误
	if(BIT_CHK(USB_ISTAT_ERROR_SHIFT,USB0->ISTAT))
	{
		err = USB0->ERRSTAT;
		#if (DEBUG_PRINT == 1)
		UART_printf("错误 错误代码:%d\r\n",err);
		#endif
		BIT_SET(USB_ISTAT_ERROR_SHIFT,USB0->ISTAT);
		USB0->ERRSTAT=0xFF; //清除所有中断错误
	}
    #endif
}



