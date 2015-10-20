#include "chlib_k.h"


#include "includes.h"
#include "drv_can.h"
#include "drv_enet.h"

/* 各个任务堆栈 */
OS_STK  APP_START_STK[1024];
OS_STK  APP_CAN_STK[1024];

void can_thread(void *pdata)
{
    while(1)
    {
        printf("test_thread loop\r\n");
        OSCAN_Send(HW_CAN1, 5, "B", 1);
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}

void start_thread_entry(void *pdata)
{
    uint8_t err;
    pdata = pdata;
    
    err = OSCAN_Init();
    printf("CAN init:%d\r\n", err);
    err = OSENET_Init();
    printf("ENET init%d\r\n", err);
    err = OSLwIP_Init();
    printf("LwIP init%d\r\n", err);
    
    OSTaskCreate(can_thread, (void *)0, (OS_STK*)(APP_CAN_STK + sizeof(APP_CAN_STK)-1), 6);
    udp_server();
    while(1)
    {
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}

void HW_Init(void)
{
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
#if defined(MK64F12)
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200); 
#else
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); 
#endif

    printf("UART initialized!\r\n");
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


