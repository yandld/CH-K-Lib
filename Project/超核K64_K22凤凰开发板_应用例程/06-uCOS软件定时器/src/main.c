/*
 * 实验名称：uc/os-II 软件定时实验
 * 实验平台：凤凰开发板
 * 板载芯片：MK64FN1MVLQ12
 * 实验效果：
 *  				1.在启动定时器3s之后，每隔1秒在串口调试助手窗口输出定时器tmr1调用的次数
 * 修改记录：
 *					1.2015.11.03 FreeXc 跟进了新版的UART_printf调用方式
 *															系统初始化完成之后挂起AppStartTask
 */

#include "chlib_k.h"
//uCOS
#include "includes.h"

/* 任务堆栈大小 */
#define TASK_STK_SIZE              (128)

/* 各个任务优先级 */
#define APP_START_TASK_PRIO        (4)
#define APP_LED_TASK_PRIO          (6)
#define APP_WDOG_TASK_PRIO         (20)

/* 各个任务堆栈 */
OS_STK  APP_START_STK[TASK_STK_SIZE];
OS_STK  APP_LED_STK[TASK_STK_SIZE];
OS_STK  APP_WDOG_STK[TASK_STK_SIZE];

/* 软件定时器句柄 */
OS_TMR   * tmr1;
 	   
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{	
    static uint32_t counter;
    UART_printf(HW_UART0,"Enter tmr1_callback : %d times\r\n", counter);
    counter++;	
}

void AppStartTask(void *pdata)
{
#if OS_CRITICAL_METHOD == 3
		OS_CPU_SR cpu_sr;
#endif
	
    uint8_t err;
    pdata = pdata;
	
    OS_ENTER_CRITICAL();
    //安装并启动uc/os-II的时钟节拍中断
    SYSTICK_Init(1000*1000/OS_TICKS_PER_SEC);
    SYSTICK_ITConfig(true);
    SYSTICK_Cmd(true);
    OS_EXIT_CRITICAL();
	
  	/* 初始化名为“tmr1”定时器 3S后开始 以后每1000MS触发一次 */    
	  /* OS_TMR_OPT_PERIODIC表示定时器在开始循环模式前等待第一次启动超时模式
		 * dly 		: 延时时间（单位：（1/OS_TMR_CFG_TICKS_PER_SEC）秒）
		 * period : 周期中断时间（单位：（1/OS_TMR_CFG_TICKS_PER_SEC）秒）
	   */
		tmr1 = OSTmrCreate(3*OS_TMR_CFG_TICKS_PER_SEC,1*OS_TMR_CFG_TICKS_PER_SEC,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0, (INT8U*)"tmr1",&err);
		OSTmrStart(tmr1,&err);	
	
		for(;;)
		{
				//初始化完成之后挂起任务
				err = OSTaskSuspend(OS_PRIO_SELF);
		}
}

int main(void)
{
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("OS:uc/os-II,test timer interrupt and start after three seconds\r\n");
	
    //初始化uc/os-II
    OSInit();
    //创建一个起始任务，用于初始化系统和启动其他任务
    OSTaskCreate(AppStartTask,(void *)0,
								&APP_START_STK[TASK_STK_SIZE-1],
								APP_START_TASK_PRIO);
    //开始任务的调度，永远不会返回
    OSStart();
//    while(1);
}


