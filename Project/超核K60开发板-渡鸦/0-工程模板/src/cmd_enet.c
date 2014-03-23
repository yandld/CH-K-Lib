#include "shell.h"
#include "clock.h"
#include "common.h"
#include "enet.h"

#define ENET_TYPE_ARP   {0x08, 0x06}
#define ENET_TYPE_IP    {0x08, 0x00}
uint8_t  gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
uint8_t  gCfgDest_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t gTxBuffer[1500];
uint8_t  gCfgEnet_Type[] = ENET_TYPE_ARP;

extern uint8_t gEnetFlag;

int CMD_ENET(int argc, char * const * argv)
{
    static uint32_t cnt = 0;
    uint32_t i = 0;
    uint8_t j;
    uint32_t len;
    uint32_t ret;
    uint16_t value;
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_Init(&ENET_InitStruct1);
    printf("ENET Init OK\r\n");
  //?????0~5???:????
  gTxBuffer[0]  = gCfgDest_MAC[0];
  gTxBuffer[1]  = gCfgDest_MAC[1];
  gTxBuffer[2]  = gCfgDest_MAC[2];
  gTxBuffer[3]  = gCfgDest_MAC[3];
  gTxBuffer[4]  = gCfgDest_MAC[4];
  gTxBuffer[5]  = gCfgDest_MAC[5];
  //?????1~11???:????
  gTxBuffer[6]  = gCfgLoca_MAC[0];
  gTxBuffer[7]  = gCfgLoca_MAC[1];
  gTxBuffer[8]  = gCfgLoca_MAC[2];
  gTxBuffer[9]  = gCfgLoca_MAC[3];
  gTxBuffer[10] = gCfgLoca_MAC[4];
  gTxBuffer[11] = gCfgLoca_MAC[5];
  //?????12~13???:????
  gTxBuffer[12] = gCfgEnet_Type[0];
  gTxBuffer[13] = gCfgEnet_Type[1];
  //?????14???????
  for(i=14; i<sizeof(gTxBuffer); i++)
  {
    gTxBuffer[i] = j++;  
  }
    while(1)
    {
        
        //ENET_MacSendData(gTxBuffer, sizeof(gTxBuffer));
        len = ENET_MacRecData(gTxBuffer);
        if(len)
        {
            gEnetFlag = 0;
            j = 14;
            //printf("gTxBuffer[%d]:%d\r\n", 15, gTxBuffer[15]);
            for(i=14; i<sizeof(gTxBuffer); i++)
            {
                if(gTxBuffer[i] != (j++))  
                {
                    printf("ERROR[%d]:%d %d\r\n", i, gTxBuffer[i], j);
                    while(1);
                }
            }
            printf("Enet frame received, len:%d %d\r\n", len, cnt++);
        }
        //DelayMs(5);
       // printf("Sended\r\n");
    }

    return 0;
}

const cmd_tbl_t CommandFun_ENET = 
{
    .name = "ENET",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_ENET,
    .usage = "ENET",
    .complete = NULL,
    .help = "ENET"
};
