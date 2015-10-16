#include "chlib_k.h"
//uCOS
#include "includes.h"
#include "drv_can.h"


/* 各个任务堆栈 */
OS_STK  APP_START_STK[1024];
OS_STK  APP_CAN_STK[1024];


/* 软件定时器句柄 */
OS_TMR   * tmr1;

static OS_EVENT* CAN_Mutex;

void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{	
    static uint32_t counter;
    UART_printf("Enter tmr1_callback:%d times\r\n", counter);
    counter++;	
}

void test_thread(void *pdata)
{
    uint8_t err;
    while(1)
    {
        OSCAN_Send(HW_CAN1, 5, "BBBBBBBB", 8);
        OSTimeDlyHMSM(0, 0, 0, 500);
    }

}

void start_thread_entry(void *pdata)
{
    uint8_t err;
    pdata = pdata;
    
    OSCAN_Init();
    
    /* 初始化定时器 1S后开始 以后每200MS触发一次 */    
 	tmr1=OSTmrCreate(1000, 200, OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0, (INT8U*)"tmr1", &err);
	//OSTmrStart(tmr1,&err);	
	OSTaskCreate(test_thread, (void *)0, (OS_STK*)(APP_CAN_STK + sizeof(APP_CAN_STK)-1), 6);

    while(1)
    {
        OSCAN_Send(HW_CAN1, 8, "AAAAAAAA", 8);
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}




void HW_Init(void)
{
    int ret;
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); 
    printf("UART initialized!\r\n");
    
//    PIT_QuickInit(0, 1000*10);
//    PIT_CallbackInstall(0, PIT_ISR);
//    PIT_ITDMAConfig(0, kPIT_IT_TOF, true);
//    printf("PIT initialized!\r\n");
    

    printf("CAN initialized!\r\n");
    
//    /* enable PinMux */
//    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
//	PORT_PinPullConfig(HW_GPIOB, 0, kPullUp);
//	PORT_PinOpenDrainConfig(HW_GPIOB, 0, ENABLE);

//    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
//    
//    ret = enet_phy_init();
//    if(ret)
//    {
//        printf("enet phy init failed! code:%d\r\n", ret);
//        while(1);
//    }
//    
//    ENET_InitTypeDef ENET_InitStruct1;
//    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
//    ENET_InitStruct1.is10MSpped = enet_phy_is_10m_speed();
//    ENET_InitStruct1.isHalfDuplex = !enet_phy_is_full_dpx();
//    ENET_Init(&ENET_InitStruct1);
//    ENET_CallbackRxInstall(ENET_ISR);
//    ENET_ITDMAConfig(kENET_IT_RXF);
//    printf("ENET initialized!\r\n");
}

int main(void)
{
    HW_Init();
    SYSTICK_Init(1000*1000/OS_TICKS_PER_SEC);
    SYSTICK_ITConfig(true);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("CoreClock:%dHz\r\n", GetClock(kCoreClock));


    OSInit();
	OSTaskCreate(start_thread_entry,
                            (void *)0,
							(OS_STK*)(APP_START_STK + sizeof(APP_START_STK)-1),
							5);
    

    SYSTICK_Cmd(true);
    
    OSStart();
    while(1);
}


