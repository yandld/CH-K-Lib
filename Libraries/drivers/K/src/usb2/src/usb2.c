#include "usb.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "message_manage.h"

/* BDT RAM */
ALIGN(512) tBDT tBDTtable[16];


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

void USB_WaitDeviceEnumed(void)
{
  while(guint8_tUSB_State != uENUMERATED);//等待USB设备被枚举
}


uint32_t USB_GetEPNum(void)
{
    return ARRAY_SIZE(USB0->ENDPOINT);
}

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
  
  
  if(guint8_tUSBClearFlags & (1<<kUSB_IN)) //如果guint8_tUSBClearFlags = 1
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
      BIT_CLR(kUSB_IN,guint8_tUSBClearFlags);//将guint8_tUSBClearFlags清零
  }
  else
  { 
      uint8_tEPSize = guint8_tIN_Counter;      //如果小于
      guint8_tIN_Counter=0;            
      BIT_SET(kUSB_IN,guint8_tUSBClearFlags);//将guint8_tUSBClearFlags置一
  }
  /*将用户缓冲的区的值复制到EP 缓冲区中准备发送*/
  tBDTtable[uint8_tEP].Cnt=(uint8_tEPSize);    //复制所要发送的数据长度
  while(uint8_tEPSize--)
       *puint8_tEPBuffer++=*puint8_tIN_DataPointer++; //将用户的数据赋值给EP存储区     

 
  if(guint8_tUSB_Toogle_flags & (1<<uint8_tEndPointFlag))  //如果相应端点的guint8_tUSB_Toogle_flags == 1
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


uint16_t USB_EP_OUT_SizeCheck(uint8_t uint8_tEP)
{
  uint8_t uint8_tEPSize; 
   /* 读取缓冲区的长度 */
  uint8_tEPSize = tBDTtable[uint8_tEP<<2].Cnt;
  return(uint8_tEPSize & 0x03FF);
}

void hw_usb_ep0_stall(void)
{
	// 产生一个STALL包  
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("send a stall packet\r\n"));
    
	BIT_SET(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
	//ENDPT0_EP_STALL = 1;                      
	tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA0; 
	tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;       
}


void USB_EnableInterface(void)
{
    uint32_t i, ep_num;
    
    ep_num = USB_GetEPNum();
    for(i = 0; i < ep_num;i++)
    {
        USB0->ENDPOINT[i].ENDPT |= (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK);
    }

    // 设置 1 BDT 设置
    // 把控制权交给MCU 
    //tBDTtable[bEP1IN_ODD].Stat._byte= kUDATA1;                //发送DATA1数据包
    //tBDTtable[bEP1IN_ODD].Cnt = 0x00;                         //计数器清空
    //tBDTtable[bEP1IN_ODD].Addr =(uint32_t)guint8_tEP1_IN_ODD_Buffer;    //地址指向对应的缓冲区

    // 设置 2 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].bufAddr =(uint32_t  )guint8_tEP2_IN_ODD_Buffer;            

    // 设置 3 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].bufAddr =(uint32_t)guint8_tEP3_OUT_ODD_Buffer;            
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
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("device desc\r\n"));
        
			USB_EP_IN_Transfer(EP0,(uint8_t*)Device_Descriptor,sizeof(Device_Descriptor));//发送设备描述符
			break;
		case CONFIGURATION_DESCRIPTOR:
			USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("config desc\r\n"));
        
			USB_EP_IN_Transfer(EP0,(uint8_t*)Configuration_Descriptor,sizeof(Configuration_Descriptor)); //发送配置描述符
			break;
		case STRING_DESCRIPTOR:

			switch(Setup_Pkt->wValue_l)  //根据wValue的低字节（索引值）散转
			{
				case 0:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: language\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 1:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 2:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender string\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 3:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender serail number\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				default: 
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: unknow:%d\r\n", Setup_Pkt->wValue_l));
					break; 
			}
			break;
				case REPORT_DESCRIPTOR:
					USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report desc\r\n"));
                
					USB_EP_IN_Transfer(EP0,(uint8_t*)Report_Descriptor,sizeof(Report_Descriptor));
					break;
		default:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("desc:unknow%d\r\n", Setup_Pkt->wValue_h));
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
    int i;
	uint8_t *p =(uint8_t*)Setup_Pkt;
	// 从DATA0 开始传输。
	BIT_CLR(0,guint8_tUSB_Toogle_flags);
    
    for(i=0;i<8;i++)
    {
        //USB_DEBUG_LOG(USB_DEBUG_EP0, ("0x%X ", *p++));
    }
    //USB_DEBUG_LOG(USB_DEBUG_EP0, ("\r\n"));
    
	if((Setup_Pkt->bmRequestType & 0x80) == 0x80)
	{
		//根据bmRequestType的D6-5位散转，D6-5位表示请求的类型
		//0为标准请求，1为类请求，2为厂商请求
        switch((Setup_Pkt->bmRequestType>>5) & 0x03)
        {
			case 0:
                
				//USB协议定义了几个标准输入请求，实现这些标准输入请求即可
				//请求的代码在bRequest中，对不同的请求代码进行散转
				switch(Setup_Pkt->bRequest)
				{
					case mGET_CONFIG:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get config\r\n"));
						break;	
					case mGET_DESC:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get desc - "));
						USB_GetDescHandler(); //执行获取描述符
						break;
					case mGET_INTF:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get interface\r\n"));
						break;
					case mGET_STATUS:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get status\r\n"));
						break;
					case mSYNC_FRAME:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: sync frame\r\n"));
						break;
                    default:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: unknown\r\n"));
						break;
				}
				break;
			case 1:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb request\r\n"));
				break;
			case 2:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb vender request\r\n"));
				break;
			default:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("unknown request\r\n"));
				break;
		}	
	}
	else
	{
		switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				switch(Setup_Pkt->bRequest)
				{
					case mCLR_FEATURE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: clear feature\r\n"));
						break;
					case mSET_ADDRESS:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: get addr:%d\r\n", Setup_Pkt->wValue_l));
						guint8_tUSB_State=uADDRESS;
						USB_EP_IN_Transfer(EP0,0,0); //为什么？？？
						break;
					case mSET_CONFIG:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set config\r\n"));
                        if(Setup_Pkt->wValue_h+Setup_Pkt->wValue_l) 
                        {
                            //使能1 、2 、3 端点 
							USB_EnableInterface();
							USB_EP_IN_Transfer(EP0,0,0);
							guint8_tUSB_State=uENUMERATED;
                        }
						break;
					case  mSET_DESC:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set desc\r\n"));
						break;
					case mSET_INTF:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set interface\r\n"));
						break;
					default:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: unknown\r\n"));
					break;
				}
				break;
				case 1:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: usb request\r\n"));
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
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: usb vender request\r\n"));
					break;
				default:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: unknown request\r\n"));
				break;
		}
	}
	tBDTtable[bEP0OUT_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL); // 为0 时: SIE 继续处理令牌
}


uint8_t USB_Init(void)
{
    Setup_Pkt=(tUSB_Setup*)BufferPointer[bEP0OUT_ODD];                       //将Setup数据结构体指向对应的缓冲区
    guint8_tUSB_State = uPOWER;
    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    MPU->CESR=0;

    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK){};
        
	//设置BDT基址寄存器
	//( 低9 位是默认512 字节的偏移) 512 = 16 * 4 * 8 。
	//8 位表示: 4 个字节的控制状态，4 个字节的缓冲区地址 。
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);

    
    USB0->USBTRC0 = 0x40; 

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
    
	//初始化消息队列
	fn_queue_init(); //初始化队列
	return 0;
}


void USB_BusResetHandler(void)
{
    uint32_t i, ep;
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb reset\r\n"));
	//清标志
	guint8_tUSBClearFlags=0xFF;
	guint8_tUSB_Toogle_flags=0;
    
    ep = USB_GetEPNum();
    
    for(i=1;i<ep;i++)
    {
        USB0->ENDPOINT[i].ENDPT=0x00;
    }

    /*端点0 BDT 启动端点设置*/
    tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;   // EP0 OUT BDT 设置
    tBDTtable[bEP0OUT_ODD].bufAddr =(uint32_t)guint8_tEP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;//USB-FS(硬件USB模块) 有专有权访问 BD
                                              //使能USB-FS去扮演数据翻转同步
                                              //定义DATA1允许发送或者接收        
   
    tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE; // EP0 OUT BDT 设置
    tBDTtable[bEP0OUT_EVEN].bufAddr =(uint32_t)guint8_tEP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;//USB-FS(硬件USB模块) 有专有权访问 BD
                                               //使能USB-FS去扮演数据翻转同步
                                               //定义DATA1允许发送或者接收       
   
    tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;   // EP0 IN BDT 设置     
    tBDTtable[bEP0IN_ODD].bufAddr =(uint32_t)guint8_tEP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;//USB-FS(硬件USB模块) 有专有权访问 BD
                                             //使能USB-FS去扮演数据翻转同步
                                             //定义DATA0允许发送或者接收 
   
    tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE;  // EP0 IN BDT 设置            
    tBDTtable[bEP0IN_EVEN].bufAddr =(uint32_t)guint8_tEP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;//USB-FS(硬件USB模块) 有专有权访问 BD
                                              //使能USB-FS去扮演数据翻转同步
                                              //定义DATA0允许发送或者接收  

    /* reset all BDT to even */
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    USB0->ENDPOINT[0].ENDPT=0x0D; // 使能 EP0 开启发送 接受时能
    
    /* enable and clear all error states */
    USB0->ERRSTAT=0xFF;
    USB0->ERREN=0xFF;
    
    /* reset USB moudle addr */
    USB0->ADDR=0x00;
    
    /* clear all interrupts status */
    USB0->ISTAT=0xFF;
    
    /* enable USB module interrupts */
    USB0->INTEN |= USB_INTEN_TOKDNEEN_MASK;
    //USB0->INTEN |= USB_INTEN_SOFTOKEN_MASK;
    USB0->INTEN |= USB_INTEN_ERROREN_MASK;
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;  
}


void USB_EP0_IN_Handler(void)
{
    if(guint8_tUSB_State==uADDRESS)
    {
        USB0->ADDR = Setup_Pkt->wValue_l; //写入地址
        guint8_tUSB_State=uREADY;              //变为Ready状态
        BIT_SET(kUSB_IN, guint8_tUSBClearFlags);
        USB_DEBUG_LOG(USB_DEBUG_EP0, ("new addr:%d\r\n", USB0->ADDR));
    }
    USB_EP_IN_Transfer(EP0,0,0); 
}


static  MessageType_t m_Msg;
void USB_EP2_IN_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = kUSB_IN;
	fn_msg_push(m_Msg); //发送一个消息
	// tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL);
}


void USB_EP2_OUT_Handler(void)
{
	
}


void USB_EP3_IN_Handler(void)
{
	
}

void USB_EP3_OUT_Handler(void)
{
    m_Msg.m_Command = USB_DEVICE_CLASS;
    m_Msg.m_MessageType = kUSB_OUT;
    m_Msg.m_MsgLen = USB_EP_OUT_SizeCheck(EP3);
    m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
    fn_msg_push(m_Msg); //发送一个消息
    tBDTtable[EP3<<2].Stat._byte= kSIE;
    tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
}


void USB_Handler(void)
{
    uint8_t ep;
    uint8_t uint8_tIN;
    uint8_tIN = USB0->STAT & 0x08;
    
    /* get endpoint direction */
    
    /* get endpoint num */
    ep = (USB0->STAT >> 4);
    
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("EP%d-DIR:%d - ", ep, uint8_tIN));
    
	if(ep == 0)
	{
		if(uint8_tIN)
		{
			USB_EP0_IN_Handler();
		}
		else
		{
			USB_EP0_OUT_Handler();
		}
	}
	if(ep == 2)
	{
		if(uint8_tIN)
		{
			USB_EP2_IN_Handler();
		}
		else
		{
			USB_EP2_OUT_Handler();
		}	
	}
	if(ep == 3)
	{
		if(uint8_tIN)
		{
			USB_EP3_IN_Handler();
		}
		else
		{
			USB_EP3_OUT_Handler();
		}	
	}
}


void USB0_IRQHandler(void)
{
	uint8_t v1 = USB0->ISTAT;
	uint8_t v2 = USB0->INTEN;
    uint8_t status = (v1 & v2);
    
    if(status & USB_ISTAT_USBRST_MASK)
    {
        USB_BusResetHandler();
    }

	if(status & USB_ISTAT_SOFTOK_MASK) 
	{
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb sof\r\n"));
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;   
	}
	
	if(status & USB_ISTAT_STALL_MASK)
	{
		USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb stall\r\n"));
        
        if(USB0->ENDPOINT[0].ENDPT & USB_ENDPT_EPSTALL_MASK)
        {
            USB0->ENDPOINT[0].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
        }
    
        USB0->ISTAT |= USB_ISTAT_STALL_MASK;
	}

	if(status & USB_ISTAT_TOKDNE_MASK) 
	{
        
		USB_Handler();

        USB0->ISTAT |= USB_ISTAT_TOKDNE_MASK;
	}

	if(status & USB_ISTAT_SLEEP_MASK) 
	{
		USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb sleep\r\n"));
        USB0->ISTAT |= USB_ISTAT_SLEEP_MASK;      
	}

	if(status & USB_ISTAT_ERROR_MASK)
	{
        uint8_t err = USB0->ERRSTAT;
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb error:%0x%X\r\n", err));
        
        /* clear */
        USB0->ISTAT |= USB_ISTAT_ERROR_MASK;
        USB0->ERRSTAT = 0xFF;
	}
}




