/* * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "enet.h"
#include "enet_phy.h"



#define     ENET_TYPE_ARP   {0x08, 0x06}
#define     ENET_TYPE_IP    {0x08, 0x00}
uint8_t     gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
uint8_t     gCfgDest_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t     gTxBuffer[1500];
uint8_t     gCfgEnet_Type[] = ENET_TYPE_ARP;

void ENET_ISR(void)
{
    static uint32_t cnt = 0;
    uint32_t len;
    len = ENET_MacReceiveData(gTxBuffer);
    if(len)
    {
        printf("enet frame received len:%d @ %d\r\n", len, cnt++);
    }
}

int main(void)
{
    int ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    
     /** print message before mode change*/
    printf("uart will be send on interrupt mode...\r\n");
    
    printf("ENET test!\r\n");
    printf("RMII clock is fiexd to OSCERCLK and must be 50Mhz\r\n");
    
    /* enable PinMux */
    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
	PORT_PinPullConfig(HW_GPIOB, 0, kPullUp);
	PORT_PinOpenDrainConfig(HW_GPIOB, 0, ENABLE);

    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    
    ret = ksz8041_init();
    if(ret)
    {
        printf("ksz8041 init failed! code:%d\r\n", ret);
        while(1);
    }
    if(!ksz8041_is_linked())
    {
        printf("no wire connected\r\n");
    }
    
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = ksz8041_is_phy_10m_speed();
    ENET_InitStruct1.isHalfDuplex = !ksz8041_is_phy_full_dpx();
    ENET_Init(&ENET_InitStruct1);
    ENET_CallbackRxInstall(ENET_ISR);
    ENET_ITDMAConfig(kENET_IT_RXF);

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
    
    while(1)
    {
        ENET_MacSendData(gTxBuffer, sizeof(gTxBuffer));
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);	
    }
}
