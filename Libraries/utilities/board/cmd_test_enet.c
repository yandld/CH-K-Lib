#include "shell.h"
#include "gpio.h"
#include "board.h"
#include "enet.h"
#include "ksz8041.h"

__align(16)uint8_t gBuffer[1500];
 uint8_t gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
 uint8_t gCfgDest_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void ENET_ISR(void)
{
    printf("%s\r\n", __func__);
    
}

int DoENET(int argc, char * const argv[])
{
    int r;
    printf("%s\r\n", __func__);
    printf("RMII clock is fiexd to OSCERCLK and must be 50Mhz\r\n");
    
    /* enable PinMux */
    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    
    r = ksz8041_init(0x01);

    ENET_InitTypeDef ENET_InitStrut;
    ENET_InitStrut.is10MSpped = false;
    ENET_InitStrut.isHalfDuplex = false;
    ENET_InitStrut.pMacAddress = gCfgLoca_MAC;
    ENET_Init(&ENET_InitStrut);
    
    ENET_CallbackRxInstall(ENET_ISR);
    ENET_ITDMAConfig(kENET_IT_RXF);
    
    while(1)
    {
        ENET_MacSendData(gBuffer, 40);
        DelayMs(50);
        printf("Send\r\n");
    }
    return 0;
}

SHELL_EXPORT_CMD(DoENET, enet , enet test)



