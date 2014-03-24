#include "shell.h"
#include "clock.h"
#include "common.h"
#include "enet.h"
#include "gpio.h"

#define ENET_TYPE_ARP   {0x08, 0x06}
#define ENET_TYPE_IP    {0x08, 0x00}
uint8_t  gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
uint8_t  gCfgDest_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t gTxBuffer[1500];
uint8_t  gCfgEnet_Type[] = ENET_TYPE_ARP;


void ENET_ISR(void)
{
    uint32_t i = 0;
    uint8_t j;
    static uint32_t cnt = 0;
    uint32_t len;
    len = ENET_MacReceiveData(gTxBuffer);
    if(len)
    {
        j = 14;
        for(i=14; i<sizeof(gTxBuffer); i++)
        {
            if(gTxBuffer[i] != (j++))  
            {
                printf("len:%d ERROR[%d]:%d %d\r\n", len, i, gTxBuffer[i], j);
                while(1);
            }
        }
        printf("Enet frame received, len:%d %d\r\n", len, cnt++);
    }
}

int CMD_ENET(int argc, char * const * argv)
{

    uint32_t len;
    uint16_t value;
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_Init(&ENET_InitStruct1);
    //Enable PinMux
    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    ENET_CallbackRxInstall(ENET_ISR);
    ENET_ITDMAConfig(kENET_IT_RXF);
    printf("ENET Init OK\r\n");
    gTxBuffer[0]  = gCfgDest_MAC[0];
    gTxBuffer[1]  = gCfgDest_MAC[1];
    gTxBuffer[2]  = gCfgDest_MAC[2];
    gTxBuffer[3]  = gCfgDest_MAC[3];
    gTxBuffer[4]  = gCfgDest_MAC[4];
    gTxBuffer[5]  = gCfgDest_MAC[5];
    
    gTxBuffer[6]  = gCfgLoca_MAC[0];
    gTxBuffer[7]  = gCfgLoca_MAC[1];
    gTxBuffer[8]  = gCfgLoca_MAC[2];
    gTxBuffer[9]  = gCfgLoca_MAC[3];
    gTxBuffer[10] = gCfgLoca_MAC[4];
    gTxBuffer[11] = gCfgLoca_MAC[5];
  
    gTxBuffer[12] = gCfgEnet_Type[0];
    gTxBuffer[13] = gCfgEnet_Type[1];
    //ENET_MacSendData(gTxBuffer, sizeof(gTxBuffer));
    //  memset(gTxBuffer, 0, sizeof(gTxBuffer));
        
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
