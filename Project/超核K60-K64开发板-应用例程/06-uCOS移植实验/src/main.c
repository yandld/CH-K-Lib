/*
 * 实验名称：uc/os-II 移植实验
 * 实验平台：渡鸦开发板
 * 板载芯片：MK60DN512ZVLQ10
 * 实验效果：
 *  				1.在串口上输出交替出现的LED1任务和LED2任务
 *					2.显示当前uc/os-II的版本
 * 					3.每秒显示CPU的使用率(%),大家可以在AppLED1Task中增加printf的个数来观察CPU占用率的情况
 * 修改记录：
 *					1.2015.10.31 FreeXc 将时钟节拍中断移至OSStart()函数之后，增加了版本显示以及CPU占用率的显示
 */

#include "gpio.h"
#include "common.h"
#include "systick.h"
#include "uart.h"
//uCOS
#include "includes.h"



//任务堆栈大小
#define TASK_STK_SIZE              (128)
//定义任务优先级
#define APP_START_TASK_PRIO        (4)
#define APP_LED1_TASK_PRIO         (5)
#define APP_LED0_TASK_PRIO         (7)
//声明任务堆栈
OS_STK  APP_START_STK[TASK_STK_SIZE];
OS_STK  APP_LED1_STK[TASK_STK_SIZE];
OS_STK  APP_LED0_STK[TASK_STK_SIZE];
//LED小灯任务
void AppLED1Task(void *pdata)
{
    pdata = pdata; //防止编译器出错 无实际意义
	while(1)
	{
		printf("LED1 Task!\r\n");
		printf("LED1 Task!\r\n");
		printf("LED1 Task!\r\n");
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}

void AppLED0Task(void *pdata)
{
    pdata = pdata; //防止编译器出错 无实际意义
	while(1)
	{
		printf("LED2 Task!\r\n");
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}

void TaskStart(void *pdata)
{
	pdata = pdata;
	//负责初始化和启动时钟节拍，在OSStart()之后启动时钟节拍是因为通常情况下，用户不希望在多任务还没有开始时
	//就接收到时钟节拍中断
	SYSTICK_Init((1000*1000)/OS_TICKS_PER_SEC);
  SYSTICK_ITConfig(true);
  SYSTICK_Cmd(true);
	//初始化统计任务
	OSStatInit();
  while(1)
	{
		//delay 1 second
		OSTimeDlyHMSM(0, 0, 1, 0);
		printf("Occupancy rate = %d%% \r\n",OSCPUUsage);
		printf("%d\n",OSIdleCtrMax);
		printf("%d\n",OSIdleCtrRun);
		printf("%d",(INT8S)(100 - OSIdleCtrRun/OSIdleCtrMax));
	}		
}

int main(void)
{
		INT16U versionNum;  
		DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
		versionNum = OSVersion();
    printf("uCOSII test，the current uc/os-II version is v%d.%d%d\r\n",versionNum/100,versionNum/10%10,versionNum%10);

    OSInit();  //OS初始化
	
		OSTaskCreate(TaskStart,(void *)0,
                &APP_START_STK[TASK_STK_SIZE - 1],
                APP_START_TASK_PRIO); //create the first task

    OSTaskCreate(AppLED1Task,(void *)0,
                &APP_LED1_STK[TASK_STK_SIZE - 1],
                APP_LED1_TASK_PRIO); //建立LED1 任务
    OSTaskCreate(AppLED0Task,(void *)0,
                &APP_LED0_STK[TASK_STK_SIZE - 1],
                APP_LED0_TASK_PRIO); //建立LED0 任务
        
    /* 控制权交给操作系统,在执行OSStart()之后操作系统开始进行任务调度 */
    OSStart();
		/* 程序永远不会运行到这 */
}
