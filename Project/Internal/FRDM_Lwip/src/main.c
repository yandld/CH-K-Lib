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
#include "pit.h"
#include "enet_phy.h"

// Standard C Included Files
#include <stdio.h>
// lwip Included Files
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"

u32_t last_arp_time;			
u32_t last_tcp_time;	
u32_t last_ipreass_time;

struct netif fsl_netif0;
extern err_t ethernetif_init(struct netif *netif);
extern void ethernetif_input(struct netif *netif);
u8_t timer_expired(u32_t *last_time,u32_t tmr_interval);
extern void PIT_ISR(void);
void LWIP_Polling(void);


#define     ENET_TYPE_ARP   {0x08, 0x06}
#define     ENET_TYPE_IP    {0x08, 0x00}
uint8_t     gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
uint8_t     gCfgDest_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t     gCfgEnet_Type[] = ENET_TYPE_ARP;
uint8_t     gTxBuffer[200];
void ENET_ISR(void)
{
  //  uint32_t len;
  //  len = ENET_MacReceiveData(gTxBuffer);
   // printf("enet frame received len\r\n");
    ethernetif_input(&fsl_netif0); 
   // if(len)
    {
    //    printf("enet frame received len:%d @ %d\r\n", len, cnt++);
    }
}

#include "lwip/opt.h"




int main(void)
{
    int ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    
    PIT_QuickInit(0, 1000*10);
    PIT_CallbackInstall(0, PIT_ISR);
    PIT_ITDMAConfig(0, kPIT_IT_TOF, true);
    
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
    
    ret = enet_phy_init();
    if(ret)
    {
        printf("ksz8041 init failed! code:%d\r\n", ret);
        while(1);
    }
    if(!enet_phy_is_linked())
    {
        printf("no wire connected\r\n");
    }
    
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = enet_phy_is_10m_speed();
    ENET_InitStruct1.isHalfDuplex = !enet_phy_is_full_dpx();
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
    
  ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

 // app_low_level_init();
 // OSA_Init();
  lwip_init();

  IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
  IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
  IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);
  netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, ethernet_input);
  netif_set_default(&fsl_netif0);
  netif_set_up(&fsl_netif0);
    
    while(1)
    {
        LWIP_Polling();
      //  printf("!!\r\n");
      //  ENET_MacSendData(gTxBuffer, sizeof(gTxBuffer));
      //  GPIO_ToggleBit(HW_GPIOE, 6);
     //   DelayMs(50);	
    }
}

u32_t input_time;
#define CLOCKTICKS_PER_MS   10
#define IP_TMR_INTERVAL 1000
#define TCP_TMR_INTERVAL       250
//LWIP查询
void LWIP_Polling(void){
	
    if(timer_expired(&input_time,5)) //接收包，周期处理函数
  {
 //   ethernetif_input(&fsl_netif0); 
  }
  if(timer_expired(&last_tcp_time,TCP_TMR_INTERVAL/CLOCKTICKS_PER_MS))//TCP处理定时器处理函数
  {
	 tcp_tmr();
  }
  if(timer_expired(&last_arp_time,ARP_TMR_INTERVAL/CLOCKTICKS_PER_MS))//ARP处理定时器
  {
	etharp_tmr();
  }
 	if(timer_expired(&last_ipreass_time,IP_TMR_INTERVAL/CLOCKTICKS_PER_MS)){ //IP重新组装定时器
 		ip_reass_tmr();
   }

  
}
