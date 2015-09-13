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
#include "lwip/dhcp.h"
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
extern err_t ethernetif_input(struct netif *netif);
u8_t timer_expired(u32_t *last_time,u32_t tmr_interval);
extern void PIT_ISR(void);
void LWIP_Polling(void);

uint8_t     gCfgLoca_MAC[] = {0x00, 0xCF, 0x52, 0x35, 0x00, 0x01};

void ENET_ISR(void)
{
    ethernetif_input(&fsl_netif0); 
}

#include "lwip/opt.h"

void time_init()
{
    
}


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


    
  ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

 // app_low_level_init();
 // OSA_Init();
  lwip_init();

#if LWIP_DHCP					 //??DHCP??
    fsl_netif0_ipaddr.addr=0;
    fsl_netif0_netmask.addr=0;
    fsl_netif0_gw.addr=0;
#else    
    IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
    IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
    IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);
#endif 
  
  netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, ethernet_input);
  netif_set_default(&fsl_netif0);
  netif_set_up(&fsl_netif0);
    dhcp_start(&fsl_netif0);
    
    dhcp_fine_tmr();
    
	/*????????IP?????*/
    while(fsl_netif0.dhcp==NULL||fsl_netif0.dhcp->offered_ip_addr.addr==0 
        ||fsl_netif0.dhcp->offered_sn_mask.addr==0 
          ||fsl_netif0.dhcp->offered_gw_addr.addr==0)
    {
    }

    /*????IP??*/
     netif_set_addr(&fsl_netif0, &(fsl_netif0.dhcp->offered_ip_addr),
                    &(fsl_netif0.dhcp->offered_sn_mask),
                    &(fsl_netif0.dhcp->offered_gw_addr));
      netif_set_default(&fsl_netif0);//???????????    
      netif_set_up(&fsl_netif0);     //??netif

    
    while(1)
    {
       // ethernetif_input(&fsl_netif0); 
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


#if LWIP_DHCP>0	
u32_t last_dhcp_fine_time;			
u32_t last_dhcp_coarse_time;  
u32_t dhcp_ip=0;
#endif


void LWIP_Polling(void){
	
    if(timer_expired(&input_time,5)) //接收包，周期处理函数
  {
 //   ethernetif_input(&fsl_netif0); 
  }
  if(timer_expired(&last_tcp_time,TCP_TMR_INTERVAL/CLOCKTICKS_PER_MS))//TCP处理定时器处理函数
  {
	// tcp_tmr();
  }
  if(timer_expired(&last_arp_time,ARP_TMR_INTERVAL/CLOCKTICKS_PER_MS))//ARP处理定时器
  {
	etharp_tmr();
  }
 	if(timer_expired(&last_ipreass_time,IP_TMR_INTERVAL/CLOCKTICKS_PER_MS)){ //IP重新组装定时器
 		ip_reass_tmr();
   }
  #if LWIP_DHCP>0			   					
  if(timer_expired(&last_dhcp_fine_time,DHCP_FINE_TIMER_MSECS/CLOCKTICKS_PER_MS))
  {
	 dhcp_fine_tmr();
  }
  if(timer_expired(&last_dhcp_coarse_time,DHCP_COARSE_TIMER_MSECS/CLOCKTICKS_PER_MS))
  {
	dhcp_coarse_tmr();
  }  
  #endif
  
}
