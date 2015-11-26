/*
 * 实验名称: uc/os-II 动态内存管理基础实验
 * 实验平台：凤凰开发板
 * 板载芯片：MK64FN1MVLQ12
 * 实验效果：
 * 			1.观察任务1中OSAddr和OSFreeList指针的变化情况，理解动态内存的分配原理与目的
 * 注意：	
 *			1.嵌入式系统中若调用malloc()和free()会把一块很大的连续内存区域逐渐分割成许多
 *			非常小但又不相邻的内存块，以至于最后都无法分配到一块很小的内存区域。uc/os-II
 *  		的动态内存管理只是在内存中划定一块分区，调用一块内存块用完之后原封不动的放回，
 *			保证不会出现内存碎片；
 *			2.在得到一块内存之后，使用时注意不要越界
 */
#include "includes.h"
#include "common.h"
#include "uart.h"
#include "systick.h"

#define TASK_STK_SIZE 512

OS_STK taskInitStk[TASK_STK_SIZE];
OS_STK task1Stk[TASK_STK_SIZE];

void taskInit(void *pdata);
void task1(void *pdata);

/* Global vars */
INT8U err;
OS_MEM *initBuffer;
INT8U initPart[8][6];
INT8U *userPtr;
OS_MEM_DATA memInfo;

int main(void)
{
	/* 硬件初始化 */
	UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	/* 初始化 uc/os-ii */ 
	OSInit();
	/* 建立一个内存分区，注意内存分区的个数在os_cfg.h中有定义 */	
	initBuffer = OSMemCreate(initPart,8,6,&err);
	if(initBuffer != (OS_MEM *)0)
	{
		printf("get memory successfully\n");
	}
	printf("0x%x\n",initPart);
	OSTaskCreate(taskInit,(void *)0,&taskInitStk[TASK_STK_SIZE - 1],4);
	
	OSStart();
}

void task1(void *pdata)
{
	(void)pdata;
	INT8U time = 0;
	while(1)
	{
		if(time == 0)
		{
			userPtr = OSMemGet(initBuffer,&err);
		}	
		/* 申请到内存之后便可以操作这块内存(userPtr指针)，注意不要越界 */
		
		time++;
		if(time > 4)
		{
			/* 执行四次之后释放内存 */
			OSMemPut(initBuffer,userPtr);
			time = 0;
		}
		/* 打印内存的分配情况，观察OSAddr和OSFreeList指针的变化 */
		OSMemQuery(initBuffer,&memInfo);
		printf("OSAddr 		= 0x%x\n",memInfo.OSAddr);
		printf("OSFreeList 	= 0x%x\n",memInfo.OSFreeList);
		printf("OSNBlks 	= %d\n",memInfo.OSNBlks);
		printf("OSBlkSize 	= %d\n",memInfo.OSBlkSize);
		printf("OSNFree 	= %d\n",memInfo.OSNFree);
		printf("OSNUsed 	= %d\n",memInfo.OSNUsed);
			
		OSTimeDlyHMSM(0, 0, 2, 0);	
	}
}

void taskInit(void *pdata)
{
	(void)pdata;
	
	SYSTICK_Init((1000*1000)/OS_TICKS_PER_SEC);
	SYSTICK_ITConfig(true);
	SYSTICK_Cmd(true);
	printf("Enter the initialization task!\n");
	
	OSTaskCreate(task1,(void *)0,&task1Stk[TASK_STK_SIZE - 1],5);
	
	while(1)
	{		
		/* 初始化完成之后，挂起当前任务 */
		OSTaskSuspend(OS_PRIO_SELF);
	}
}
