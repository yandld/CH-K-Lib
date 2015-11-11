/*
 * 实验名称: uc/os-II 事件标志组管理
 * 实验平台：渡鸦开发板
 * 板载芯片：MK60DN512ZVLQ10
 * 实验效果：
 *  				1.打开串口调试助手 115200 ，输入1234 回车,然后发送，看是不是返回password is correct, password = 1234
 *					2.按下渡鸦开发板的KEY1按钮，这时候会打印taskLedOn
 * 说明：  
 * 					1.必须同时满足  密码正确（1234）和按下KEY1键才会输出taskLedOn。该实验测试的是uc/os-II的事件标志组管理（event flag）
 */
#include "includes.h"
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "systick.h"

#define TASK_STK_SIZE 512

OS_STK taskInitStk[TASK_STK_SIZE];
OS_STK taskLedOnStk[TASK_STK_SIZE];
OS_STK taskPasswordStk[TASK_STK_SIZE];

void taskInit(void *pdata);
void taskLedOn(void *pdata);
void taskPassword(void *pdata);
static void PTE26_EXTI_ISR(uint32_t pinxArray);

/* Global vars */
INT8U err;
OS_FLAG_GRP *EventFlag;

int main(void)
{
	/* 硬件初始化 */
	uint32_t instance;	
	instance = GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_FallingEdge, true);
    GPIO_CallbackInstall(instance, PTE26_EXTI_ISR);
	UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	/* 初始化 uc/os-ii */ 
	OSInit();
	/* create an event flag */	
	EventFlag = OSFlagCreate(0,&err);
	OSTaskCreate(taskInit,(void *)0,&taskInitStk[TASK_STK_SIZE - 1],4);
		
	OSStart();
}

void taskLedOn(void *pdata)
{
	(void)pdata;
	
	while(1)
	{
		/* OS_FLAG_CONSUME的作用是任务每次等待的事件发生之后，会自动置起或是清楚相应的事件标志位 */
		/* OSFlagPend函数是请求第0位和第1位信号(1有效)同时被置位 */
		OSFlagPend(EventFlag, (OS_FLAGS)3,OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME,0,&err);
		printf("taskLedOn\n");
		OSTimeDlyHMSM(0, 0, 0, 100);	
	}
}

static void PTE26_EXTI_ISR(uint32_t pinxArray)
{
	/* 中断中只允许OSFlagPost()、OSFlagAccept()、OSFlagQuery()非阻塞性质的函数 */
	/* 第1位标志位置1 */
	OSFlagPost(EventFlag, (OS_FLAGS)2,OS_FLAG_SET,&err);
}

void taskPassword(void *pdata)
{
	(void)pdata;
	char str[10] = {0};
	char pwCorrect[] = "1234";
	printf("%d\n",sizeof(pwCorrect));
	uint8_t pwCheckedFlag = 0;
	while(1)
	{
		if(pwCheckedFlag == 0)
		{
			gets(str);
			if(((strncmp(str, pwCorrect,4) == 0)) && (str[4] == 0x0d))
			{
				printf("password is correct, password = %s\n",str);
				/* 第0位标志位置1 */
				OSFlagPost(EventFlag, (OS_FLAGS)1,OS_FLAG_SET,&err);
			}
			else
			{
				printf("password is wrong %s\n",str);
			}
				OSTimeDlyHMSM(0, 0, 0, 20);
		}
	}
}


void taskInit(void *pdata)
{
	(void)pdata;
    		
	SYSTICK_Init((1000*1000)/OS_TICKS_PER_SEC);
	SYSTICK_ITConfig(true);
	SYSTICK_Cmd(true);
	printf("enter the initialization task\n");
	
	OSTaskCreate(taskPassword,(void *)0,&taskPasswordStk[TASK_STK_SIZE - 1],6);
	OSTaskCreate(taskLedOn,(void *)0,&taskLedOnStk[TASK_STK_SIZE - 1],5);
	
	while(1)
	{		
		/* 挂起当前任务 */
		OSTaskSuspend(OS_PRIO_SELF);
	}
}
