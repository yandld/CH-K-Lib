#include "chlib_k.h"

#include "enet_phy.h"

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

//uCOS
#include "includes.h"
#include "drv_can.h"

struct netif fsl_netif0;
extern err_t ethernetif_init(struct netif *netif);
extern err_t ethernetif_input(struct netif *netif);


/* 各个任务堆栈 */
OS_STK  APP_START_STK[1024];
OS_STK  APP_CAN_STK[1024];

void test_thread(void *pdata)
{
    uint8_t err;
    while(1)
    {
        printf("BB\r\n");
        OSCAN_Send(HW_CAN1, 5, "BBBBBBBB", 8);
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}

void start_thread_entry(void *pdata)
{
    uint32_t i;
    uint8_t err;
    pdata = pdata;
    
    OSCAN_Init();
    printf("CAN initialized!\r\n");
    
    ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
    
    tcpip_init(NULL, NULL);

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

    OSTaskCreate(test_thread, (void *)0, (OS_STK*)(APP_CAN_STK + sizeof(APP_CAN_STK)-1), 6);
    udp_sever();
    while(1)
    {
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}

uint8_t gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
 
void ENET_ISR(void)
{
	OSIntEnter();
    ethernetif_input(&fsl_netif0); 
	OSIntExit();
}


void HW_Init(void)
{
    int ret;
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
#if defined(MK64F12)
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200); 
#else
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); 
#endif

    printf("UART initialized!\r\n");

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
        while(1);
    }
    
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = enet_phy_is_10m_speed();
    ENET_InitStruct1.isHalfDuplex = !enet_phy_is_full_dpx();
    ENET_Init(&ENET_InitStruct1);
    ENET_CallbackRxInstall(ENET_ISR);
    printf("ENET initialized!\r\n");
}

int main(void)
{
    HW_Init();
    SYSTICK_Init(1000*1000/OS_TICKS_PER_SEC);

    printf("CoreClock:%dHz\r\n", GetClock(kCoreClock));

    OSInit();
	OSTaskCreate(start_thread_entry,
                            (void *)0,
							&APP_START_STK[1023],
							5);
    
    SYSTICK_Cmd(true);
    SYSTICK_ITConfig(true);
    OSStart();
    while(1);
}


