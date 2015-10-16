#include "chlib_k.h"
//uCOS
#include "includes.h"



/* 各个任务堆栈 */
OS_STK  APP_START_STK[256];
OS_STK  APP_LED_STK[256];
OS_STK  APP_WDOG_STK[256];

/* 软件定时器句柄 */
OS_TMR   * tmr1;
			  	   
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{	
    static uint32_t counter;
    UART_printf("Enter tmr1_callback:%d times\r\n", counter);
    counter++;	
}

void AppStartTask(void *pdata)
{
    uint8_t err;
    pdata = pdata;
    
    /* 初始化定时器 1S后开始 以后每200MS触发一次 */    
 	tmr1=OSTmrCreate(1000,200,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0, (INT8U*)"tmr1",&err);
	OSTmrStart(tmr1,&err);	
}

int main(void)
{
    DelayInit();
    SYSTICK_Init(1000*1000/OS_TICKS_PER_SEC);
    SYSTICK_ITConfig(true);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("uCOSII test\r\n");

    OSInit();
	OSTaskCreate(AppStartTask,(void *)0,
							(OS_STK*)(APP_START_STK + sizeof(APP_START_STK)-1),
							5);
    
    SYSTICK_Cmd(true);
    
    OSStart();
    while(1);
}


