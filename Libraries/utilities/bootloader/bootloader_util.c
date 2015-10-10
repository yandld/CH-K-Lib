#include "mq.h"
#include "bootloader_util.h"
#include "bootloader.h"

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
}States_t;



RunType_t M_Control;
static 	msg_t m_Msg;

/**
  * @fn: GetData
  * @bre: 处理接收到的数据,采用状态机的方式来实现
*/
void GetData(uint8_t data)
{
    static uint8_t RevBuf[4096+30];
    static int i, length;
    static uint8_t fcs;
    static States_t States = INIT_STATE;
    if(M_Control.timeout > 500)
    {
        States = INIT_STATE;
    }

    M_Control.timeout = 0;

    switch(States)
    {
        case INIT_STATE:
            i = 0;
            fcs = 0;
            if(0xFF == data)
            {
                States = HS_STATE;
            }
            else
            {
                States = INIT_STATE;
            }
            break;

        case HS_STATE:
            if(0xFF == data)
            {
                States = CIPHER_STATE;
            }
            break;

        case CIPHER_STATE:
            if(data == 0 || data == 1)
            {
                /*  CIPHER state */
                States = LEN1_STATE;
            }
            else
            {
                States = INIT_STATE;
            }
            break;

        case LEN1_STATE:
            length = data;
            States = LEN2_STATE;
            break;

        case LEN2_STATE:
            length |= (data<<8);
            if(length > 0 && length <= 8192)
            {
                i = 0;
                States = CONTENT_STATE;
            }
            else
            {
                States = INIT_STATE;
            }
            break;

        case CONTENT_STATE:
            fcs += data;

            RevBuf[i++] = data;

            if(i >= length)
            {
                States = FCS_STATE;
            }

            break;

        case FCS_STATE:
            if(fcs == data)
            {
                States = END_HS_STATE;
            }
            else
            {
                States = INIT_STATE;
            }
            break;

        case END_HS_STATE:
            if(0xFF == data)
            {
                States = END_STATE;
            }
            else
            {
                States = INIT_STATE;
            }
            break;

        case END_STATE:
            if(0xFE == data)
            {
                m_Msg.cmd = CMD_SERIALPORT;
                m_Msg.pMessage = RevBuf;
                m_Msg.len = length;
                mq_push(m_Msg);
            }
            States = INIT_STATE;
            break;
    }

}

extern Boot_t Bootloader;

void SendResp(uint8_t* content, uint8_t cipherFlg,  uint16_t len)
{
    uint8_t header[5] = {0xFF, 0xFF, 0, 0, 0};
    uint8_t footer[3] = {0, 0xFF, 0xFE};
    uint8_t fcs = 0;
    int i;

    header[3] = (uint8_t)(len & 0xFF);
    header[4] = (uint8_t)((len>>8) & 0xFF);

    /* cipter */
    if(cipherFlg == 1)
    {

    }

    /* make checksum */
    for(i=0; i<len; i++)
    {
        fcs += content[i];
    }
    footer[0] = fcs;
    
    Bootloader.send(header, sizeof(header));
    Bootloader.send(content, len);
    Bootloader.send(footer, sizeof(footer));
}


