#include "shell.h"
#include "clock.h"
#include "common.h"
#include "enet.h"


uint8_t  gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
uint8_t gbufer[1500];
int CMD_ENET(int argc, char * const * argv)
{
    uint32_t ret;
    uint16_t value;
	//¿ªPORTÊ±ÖÓ
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_Init(&ENET_InitStruct1);
    printf("ENET Init OK\r\n");
    while(1)
    {
        ENET_MacSendData(gbufer, sizeof(gbufer));
        DelayMs(500);
        printf("Sended\r\n");
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
