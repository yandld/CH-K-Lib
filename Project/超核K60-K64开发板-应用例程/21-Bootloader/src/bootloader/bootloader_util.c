#include "message_manage.h"
#include "bootloader_util.h"

#pragma pack(1)
typedef struct
{
    uint8_t sof;
    uint8_t hs;
    uint8_t cipherFlg;
    uint8_t length[2];
    uint8_t conent[8192];
    uint8_t fcs;
    uint8_t end_hs;
    uint8_t eof;
} Frame_Type_t;

typedef enum
{
    INIT_STATE = 0,
    HS_STATE,
    CIPHER_STATE,
    LEN1_STATE,
    LEN2_STATE,
    CONTENT_STATE,
    FCS_STATE,
    END_HS_STATE,
    END_STATE
} CmdStateType_t;

typedef struct
{
    uint8_t cache[100];
    uint8_t offset;
    uint8_t length;
    uint8_t busyFlg;
} UartSendType_t;


UART_Type *pUARTx;
RunType_t M_Control;

static 	MessageType_t m_Msg;
static CmdStateType_t trans_state = INIT_STATE;
static uint16_t rx_loc = 0;
static uint8_t fcs;
static int length;

static Frame_Type_t m_RcvFrame;
static UartSendType_t sendCache;


/**
  * @fn: Fn_RxProcData
  * @bre: 处理接收到的数据,采用状态机的方式来实现
*/
void Fn_RxProcData(const uint8_t buf)
{

    if(M_Control.usart_timeout > 500)
    {
        trans_state = INIT_STATE;
    }

    M_Control.usart_timeout = 0;

    switch(trans_state)
    {
    case INIT_STATE:
        rx_loc = 0;
        fcs = 0;

        if(0xFF == buf)
        {
            trans_state = HS_STATE;
        }
        else
        {
            trans_state = INIT_STATE;
        }
        break;

    case HS_STATE:
        if(0xFF == buf)
        {
            trans_state = CIPHER_STATE;
        }
        break;

    case CIPHER_STATE:
        if(buf == 0 || buf == 1)
        {
            m_RcvFrame.cipherFlg = buf;
            trans_state = LEN1_STATE;
        }
        else
        {
            trans_state = INIT_STATE;
        }
        break;

    case LEN1_STATE:
        m_RcvFrame.length[0] = buf;
        trans_state = LEN2_STATE;
        break;

    case LEN2_STATE:
        m_RcvFrame.length[1] = buf;
        length = buf;
        length <<= 8;
        length |= m_RcvFrame.length[0];

        if(length > 0 && length <= 8192)
        {
            rx_loc = 0;
            trans_state = CONTENT_STATE;
        }
        else
        {
            trans_state = INIT_STATE;
        }
        break;

    case CONTENT_STATE:
        fcs += buf;

        m_RcvFrame.conent[rx_loc++] = buf;

        if(rx_loc >= length)
        {
            trans_state = FCS_STATE;
        }

        break;

    case FCS_STATE:
        if(fcs == buf)
        {
            trans_state = END_HS_STATE;
        }
        else
        {
            trans_state = INIT_STATE;
        }
        break;

    case END_HS_STATE:
        if(0xFF == buf)
        {
            trans_state = END_STATE;
        }
        else
        {
            trans_state = INIT_STATE;
        }
        break;

    case END_STATE:
        if(0xFE == buf)
        {
            m_Msg.m_Command = CMD_SERIALPORT;
            m_Msg.pMessage = m_RcvFrame.conent;
            m_Msg.m_MsgLen = length;
            fn_msg_push(m_Msg);
						#ifdef BOOTLOADER_DEBUG
						printf("接收到的消息长度%d\r\n",m_Msg.m_MsgLen);
						#endif
        }
        trans_state = INIT_STATE;
        break;
    }

}


/**
  * @fn:    Fn_SendResponse
  * @bref:  发送回应函数
  */
extern uint8_t ModeSelect;
void Fn_SendResponse(uint8_t* content, uint8_t cipherFlg,  uint16_t len)
{
	uint16_t NumOfFrame;
	uint16_t Frame_Remain;
	uint16_t Cnt;
    uint8_t header[5] = {0xFF, 0xFF, 0, 0, 0};
    uint8_t footer[3] = {0, 0xFF, 0xFE};
    uint8_t fcs = 0;
    int i, offset = 0;



    header[3] = (uint8_t)(len&0xFF);
    header[4] = (uint8_t)((len>>8)&0xFF);

    if(cipherFlg == 1)
    {
        //加入加密代码，未实现
    }

    for(i=0; i<len; i++)
    {
        fcs += content[i];
    }
    footer[0] = fcs;

    //等待发送完
    while(sendCache.busyFlg);

    for(i=0; i<5; i++)
    {
        sendCache.cache[offset++] = header[i];
    }

    for(i=0; i<len; i++)
    {
        sendCache.cache[offset++] = content[i];
    }
    for(i=0; i<3; i++)
    {
        sendCache.cache[offset++] = footer[i];
    }
    sendCache.offset = 0;
    sendCache.length = offset;
    sendCache.busyFlg = 1;
		//USB模拟串口发送
		#ifdef BOOTLOADER_DEBUG
		printf("需要发送的数据大小: %d\r\n",sendCache.length);
		#endif
    NumOfFrame = sendCache.length / 30;
    Frame_Remain = sendCache.length % 30;
    //发送过程1
    uint8_t *p = sendCache.cache;
    for(i=0; i<sendCache.length; i++)
    {
        UART_WriteByte(0, *p++);
    }
    sendCache.busyFlg = 0;
}


