#include "usb.h"
#include "usb_desc.h"
#include "usb_desc_type.h"
#include "usb_hid.h"
#include "message_manage.h"

/* alloc BDT */
ALIGN(512) tBDT tBDTtable[NUMBER_OF_PHYSICAL_ENDPOINTS];

static CONTROL_TRANSFER transfer;
static USB_DEVICE device;

//各个端点的数据缓冲区
uint8_t EP0_OUT_ODD_Buffer[EP0_SIZE];
uint8_t EP0_OUT_EVEN_Buffer[EP0_SIZE];
uint8_t EP0_IN_ODD_Buffer[EP0_SIZE];
uint8_t EP0_IN_EVEN_Buffer[EP0_SIZE];
uint8_t EP1_OUT_ODD_Buffer[EP1_SIZE];
uint8_t EP1_OUT_EVEN_Buffer[EP1_SIZE];
uint8_t EP1_IN_ODD_Buffer[EP1_SIZE];
uint8_t EP1_IN_EVEN_Buffer[EP1_SIZE];
uint8_t EP2_OUT_ODD_Buffer[EP2_SIZE];
uint8_t EP2_OUT_EVEN_Buffer[EP2_SIZE];
uint8_t EP2_IN_ODD_Buffer[EP2_SIZE];
uint8_t EP2_IN_EVEN_Buffer[EP2_SIZE];
uint8_t EP3_OUT_ODD_Buffer[EP3_SIZE];
uint8_t EP3_OUT_EVEN_Buffer[EP3_SIZE];
uint8_t EP3_IN_ODD_Buffer[EP3_SIZE];
uint8_t EP3_IN_EVEN_Buffer[EP3_SIZE];
//指向各个缓冲区的地址指针
uint8_t *BufferPointer[]=
{
    EP0_OUT_ODD_Buffer,
    EP0_OUT_EVEN_Buffer,
    EP0_IN_ODD_Buffer,
    EP0_IN_EVEN_Buffer,
    EP1_OUT_ODD_Buffer,
    EP1_OUT_EVEN_Buffer,
    EP1_IN_ODD_Buffer,
    EP1_IN_EVEN_Buffer,
    EP2_OUT_ODD_Buffer,
    EP2_OUT_EVEN_Buffer,
    EP2_IN_ODD_Buffer,
    EP2_IN_EVEN_Buffer,
    EP3_OUT_ODD_Buffer,
    EP3_OUT_EVEN_Buffer,
    EP3_IN_ODD_Buffer,
    EP3_IN_EVEN_Buffer
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

const uint8_t* String_Table[4]=
{
    String_Descriptor0,
    String_Descriptor1,
    String_Descriptor2,
    String_Descriptor3
};

//SETUP包后面只能跟DATA0

void USBD_DecodeSetupPacket(uint8_t *data, SETUP_PACKET *packet)
{
    /* Fill in the elements of a SETUP_PACKET structure from raw data */
    packet->bmRequestType.dataTransferDirection = (data[0] & 0x80) >> 7;
    packet->bmRequestType.Type = (data[0] & 0x60) >> 5;
    packet->bmRequestType.Recipient = data[0] & 0x1f;
    packet->bRequest = data[1];
    packet->wValue = (data[2] | (uint16_t)data[3] << 8);
    packet->wIndex = (data[4] | (uint16_t)data[5] << 8);
    packet->wLength = (data[6] | (uint16_t)data[7] << 8);
}


uint32_t USB_GetPhyEPNumber(void)
{
    return ARRAY_SIZE(USB0->ENDPOINT);
}

void USB_EP_IN_Transfer(uint8_t ep, uint8_t *buf, uint8_t len)
{
    uint8_t *p;
    uint8_t transfer_size;     //端点的数据长度 
    static uint8_t *pData;
    static uint8_t remain;
    static bool new_transfer = true;
    
    transfer_size = 0;
    
    ep *= 4;
    ep += 2; //EP值加2
    p = BufferPointer[ep];
  
    /* a new transfer */
    if(new_transfer)
    {
        pData = buf;
        remain = len;

        if((device.setup_pkt.wLength < len) && (ep == 2))
        {
            remain = device.setup_pkt.wLength;
        }
    }
    
    if(remain > cEP_Size[ep])
    {
        transfer_size = cEP_Size[ep];
        remain -= cEP_Size[ep];
        new_transfer = false;
    }
    else
    {
        transfer_size = remain;
        remain = 0;            
        new_transfer = true;
    }
    
    tBDTtable[ep].Cnt=(transfer_size);
    while(transfer_size--)
    {
        *p++ = *pData++; //将用户的数据赋值给EP存储区   
    }
    
    /* DATA0 DATA1 transfer */
    static uint8_t data1 = kUDATA0;
    data1 ^= 0x40;
    tBDTtable[ep].Stat._byte = data1;
}


uint16_t USB_GetPacketSize(uint8_t ep)
{
    uint8_t size; 
    size = tBDTtable[ep << 2].Cnt;
    return(size & 0x03FF);
}



void USB_EnableInterface(void)
{
    uint32_t i, ep_num;
    
    /* enable all endpoint */
    ep_num = USB_GetPhyEPNumber();
    for(i = 0; i < ep_num;i++)
    {
        USB0->ENDPOINT[i].ENDPT |= (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK);
    }

    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].bufAddr =(uint32_t  )EP2_IN_ODD_Buffer;            

    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].bufAddr =(uint32_t)EP3_OUT_ODD_Buffer;            
}

//USBD_GetDesc
uint8_t* USBD_GetStdDesc(SETUP_PACKET *packet, uint32_t *len)
{
    uint8_t *p;
    
    *len = 0;
    p = 0;
    
    switch(DESCRIPTOR_TYPE(packet->wValue))
	{
		case DEVICE_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("device desc\r\n"));
            p = (uint8_t*)Device_Descriptor;
            *len = sizeof(Device_Descriptor);
			break;
        
		case CONFIGURATION_DESCRIPTOR:
			USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("config desc\r\n"));
            p = (uint8_t*)Configuration_Descriptor;
            *len = sizeof(Configuration_Descriptor);
			break;
        
		case STRING_DESCRIPTOR:
            switch (DESCRIPTOR_INDEX(packet->wValue))
			{
				case STRING_OFFSET_LANGID:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: language\r\n"));
                    p = (uint8_t*)String_Table[STRING_OFFSET_LANGID];
                    *len = sizeof(String_Table[STRING_OFFSET_LANGID]);
					break;
				case STRING_OFFSET_IMANUFACTURER:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender\r\n"));
                    p = (uint8_t*)String_Table[STRING_OFFSET_IMANUFACTURER];
                    *len = sizeof(String_Table[STRING_OFFSET_IMANUFACTURER]);
					break;
				case STRING_OFFSET_IPRODUCT:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender string\r\n"));
                    p = (uint8_t*)String_Table[STRING_OFFSET_IPRODUCT];
                    *len = sizeof(String_Table[STRING_OFFSET_IPRODUCT]);
					break;
				case STRING_OFFSET_ISERIAL:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender serail number\r\n"));
                    p = (uint8_t*)String_Table[STRING_OFFSET_ISERIAL];
                    *len = sizeof(String_Table[STRING_OFFSET_ISERIAL]);
					break;
				case STRING_OFFSET_ICONFIGURATION:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: congiuration\r\n"));
					break;
				case STRING_OFFSET_IINTERFACE:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: interface\r\n"));
					break;
				default: 
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: unknow:%d\r\n",DESCRIPTOR_INDEX(packet->wValue)));
					break; 
			}
			break;
        case REPORT_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report desc\r\n"));
            p = (uint8_t*)Report_Descriptor;
            *len = sizeof(Report_Descriptor);
            break;
		default:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("desc:unknow%d\r\n",  DESCRIPTOR_TYPE(packet->wValue)));
			break;
    }
    return p;
}


void USB_EP0_OUT_Handler(SETUP_PACKET *packet)
{
    uint32_t i, size;
    uint8_t *p;
    for(i=0;i<8;i++)
    {
        //USB_DEBUG_LOG(USB_DEBUG_EP0, ("0x%X ", *p++));
    }
    //USB_DEBUG_LOG(USB_DEBUG_EP0, ("~~0x%X\r\n", packet->bmRequestType));
    
	if(packet->bmRequestType.dataTransferDirection == DEVICE_TO_HOST)
	{
        printf("DEVICE_TO_HOST\r\n");
        switch(packet->bmRequestType.Type)
        {
			case STANDARD_TYPE:
				switch(packet->bRequest)
				{
					case GET_CONFIGURATION:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get config\r\n"));
						break;	
					case GET_DESCRIPTOR:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get desc - "));
                        p = USBD_GetStdDesc(packet, &size);
                        if(size)
                        {
                            USB_EP_IN_Transfer(EP0, p, size);
                        }
						break;
					case GET_INTERFACE:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get interface\r\n"));
						break;
					case GET_STATUS:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get status\r\n"));
						break;
                    default:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: unknown\r\n"));
						break;
				}
				break;
			case CLASS_TYPE:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb class request\r\n"));
				break;
			case VENDOR_TYPE:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb vender request\r\n"));
				break;
			default:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("unknown request\r\n"));
				break;
		}	
	}
	else
	{
        printf("HOST_TO_DEIVCE\r\n");
		switch(packet->bmRequestType.Type)
		{
			case STANDARD_TYPE:
				switch(packet->bRequest)
				{
					case CLEAR_FEATURE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: clear feature\r\n"));
						break;
					case SET_ADDRESS:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: get addr:%d\r\n", packet->wValue));
                        device.state = ADDRESS;
						USB_EP_IN_Transfer(EP0, 0, 0);
						break;
					case SET_CONFIGURATION:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set config\r\n"));
                        USB_EnableInterface();
                        USB_EP_IN_Transfer(EP0, 0, 0);
                        device.state = CONFIGURED;
						break;
					case SET_DESCRIPTOR:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set desc\r\n"));
						break;
					case SET_INTERFACE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set interface\r\n"));
						break;
					default:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: unknown\r\n"));
					break;
				}
				break;
				case CLASS_TYPE:
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
				case VENDOR_TYPE:
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
    device.state = DEFAULT;
    
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

    /* BDT adddress */
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);

    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;
	USB0->USBTRC0 |= 0x40;

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
    
	fn_queue_init();
    
	return 0;
}


void USB_BusResetHandler(void)
{
    uint32_t i, phy_ep;
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb reset\r\n"));

    phy_ep = USB_GetPhyEPNumber();
    
    for(i = 1; i < phy_ep; i++)
    {
        USB0->ENDPOINT[i].ENDPT=0x00;
    }

    /*端点0 BDT 启动端点设置*/
    tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;
    tBDTtable[bEP0OUT_ODD].bufAddr =(uint32_t)EP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;   
   
    tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE;
    tBDTtable[bEP0OUT_EVEN].bufAddr =(uint32_t)EP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;     
   
    tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;
    tBDTtable[bEP0IN_ODD].bufAddr =(uint32_t)EP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;
   
    tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE; 
    tBDTtable[bEP0IN_EVEN].bufAddr =(uint32_t)EP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;

    /* reset all BDT to even */
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    USB0->ENDPOINT[0].ENDPT = 0x0D;
    
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



void USB_EP0_IN_Handler(SETUP_PACKET *packet)
{
    if(device.state == ADDRESS)
    {
        USB0->ADDR = packet->wValue & 0xFF;
        device.state = CONFIGURED;
        USB_DEBUG_LOG(USB_DEBUG_EP0, ("new addr:%d\r\n", USB0->ADDR));
    }
    USB_EP_IN_Transfer(EP0, 0, 0); 
}


static  MessageType_t m_Msg;
void USB_EP2_IN_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = kUSB_IN;
	fn_msg_push(m_Msg); //发送一个消息
	// tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;

    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
}

void USB_EPCallback(uint8_t ep, uint8_t dir)
{
    if((ep == 2) && (dir == kUSB_IN))
    {
        m_Msg.m_Command = USB_DEVICE_CLASS;
        m_Msg.m_MessageType = kUSB_IN;
        fn_msg_push(m_Msg); //发送一个消息
        // tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    }
    if((ep == 3) && (dir == kUSB_OUT))
    {
        m_Msg.m_Command = USB_DEVICE_CLASS;
        m_Msg.m_MessageType = kUSB_OUT;
        m_Msg.m_MsgLen = USB_GetPacketSize(EP3);
        m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
        fn_msg_push(m_Msg); //发送一个消息
        tBDTtable[EP3<<2].Stat._byte= kSIE;
        tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
    }
    
}

void USB_Handler(void)
{
    uint8_t ep;
    uint8_t dir;

    /* get endpoint direction and ep number */
    (USB0->STAT & USB_STAT_TX_MASK)?(dir = kUSB_IN):(dir = kUSB_OUT);
    ep = ((USB0->STAT & USB_STAT_ENDP_MASK )>> USB_STAT_ENDP_SHIFT);
    
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("EP%d-DIR:%d - ", ep, dir));
    
	if(ep == 0)
	{
        SETUP_PACKET packet;
        
        //printf("!%d\r\n", tBDTtable[0].Cnt);
        USBD_DecodeSetupPacket((uint8_t*)BufferPointer[bEP0OUT_ODD], &packet);
        device.setup_pkt = packet;
        
		if(dir == kUSB_IN)
		{
			USB_EP0_IN_Handler(&packet);
		}
		else
		{
			USB_EP0_OUT_Handler(&packet);
		}
	}
    
    /* other ep callback */
    USB_EPCallback(ep, dir);
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


