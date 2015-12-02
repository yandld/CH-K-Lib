#include "includes.h"
#include "enet.h"
#include "gpio.h"
#include "enet_phy.h"
#include "common.h"
#include "lwip/tcpip.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"

#include "drv_enet.h"

uint8_t gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
struct netif fsl_netif0;
extern err_t ethernetif_init(struct netif *netif);
extern err_t ethernetif_input(struct netif *netif);

void ENET_ISR(void)
{
	OSIntEnter();
    ethernetif_input(&fsl_netif0); 
	OSIntExit();
}


uint32_t OSENET_Init(void)
{
    int ret;
    
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
        printf("enet phy init failed! code:%d\r\n", ret);
        return ret;
    }
    
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = enet_phy_is_10m_speed();
    ENET_InitStruct1.isHalfDuplex = !enet_phy_is_full_dpx();
    ENET_Init(&ENET_InitStruct1);
    ENET_CallbackRxInstall(ENET_ISR);
    return 0;
}

uint32_t OSLwIP_Init(void)
{
    int i;
    ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
    
    tcpip_init(NULL, NULL);
    OSTimeDlyHMSM(0, 0, 0, 100);
    printf("tcp ip init ok!\r\n");
    
#if LWIP_DHCP					 // DHCP
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
    ENET_ITDMAConfig(kENET_IT_RXF);
#if LWIP_DHCP	
    printf("dhcp start getting addr...\r\n");
    
    dhcp_start(&fsl_netif0);
    i = sys_now();
    while((fsl_netif0.dhcp->offered_ip_addr.addr == 0) || (fsl_netif0.dhcp->offered_gw_addr.addr == 0) || (fsl_netif0.dhcp->offered_sn_mask.addr == 0))
    {
        if(sys_now()-i > 8000)
        {
            i = sys_now();
            printf("dhcp restart\r\n");
            dhcp_start(&fsl_netif0);
        }
        OSTimeDlyHMSM(0, 0, 0, 100);
    }
    
    dhcp_fine_tmr();
    netif_set_addr(&fsl_netif0, &(fsl_netif0.dhcp->offered_ip_addr), &(fsl_netif0.dhcp->offered_sn_mask), &(fsl_netif0.dhcp->offered_gw_addr));
    netif_set_default(&fsl_netif0);
    netif_set_up(&fsl_netif0);
        
    u8_t *ip = (u8_t*)&fsl_netif0.ip_addr.addr;
    printf("dhcp new ip: %u.%u.%u.%u \r\n",ip[0], ip[1],ip[2], ip[3]);
    
    ip = (u8_t*)&fsl_netif0.dhcp->offered_sn_mask;
    printf("dhcp new net mask: %u.%u.%u.%u \r\n",ip[0], ip[1],ip[2], ip[3]);
    
    ip = (u8_t*)&fsl_netif0.dhcp->offered_gw_addr;
    printf("dhcp new net gw: %u.%u.%u.%u \r\n",ip[0], ip[1],ip[2], ip[3]);
   
#endif  
    return 0;
}









