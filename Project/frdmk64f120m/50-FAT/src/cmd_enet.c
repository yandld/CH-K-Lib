#include "shell.h"
#include "enet.h"
#include "gpio.h"
#include "ksz8041.h"

int DoENET(int argc, char * const argv[])
{
    int r;
    printf("%s\r\n", __func__);
    printf("ENET test!\r\n");
    printf("RMII clock is fiexd to OSCERCLK and must be 50Mhz\r\n");
    
    ENET_InitTypeDef ENET_InitStrut;
    ENET_InitStrut.is10MSpped = true;
    ENET_InitStrut.isHalfDuplex = true;
    ENET_InitStrut.pMacAddress = NULL;
    ENET_Init(&ENET_InitStrut);
    /* enable PinMux */
//    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    
   // r = ksz8041_init(0x01);
    
}

SHELL_EXPORT_CMD(DoENET, enet , enet test)
    

