/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"

#include <stdint.h>

#ifdef USE_RTOS



//当消息指针为空时,指向一个常量pvNullPointer所指向的值.
//在UCOS中如果OSQPost()中的msg==NULL会返回一条OS_ERR_POST_NULL
//错误,而在lwip中会调用sys_mbox_post(mbox,NULL)发送一条空消息,我们
//在本函数中把NULL变成一个常量指针0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 

err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
    (*mbox) = malloc(sizeof(TQ_DESCR));
    memset((*mbox),0,sizeof(TQ_DESCR));
    
	if(*mbox)
	{
		if(size > MAX_QUEUE_ENTRIES)
        {
            size=MAX_QUEUE_ENTRIES;
        }
 		(*mbox)->pQ = OSQCreate(&((*mbox)->pvQEntries[0]), size);  //使用UCOS创建一个消息队列
		LWIP_ASSERT("OSQCreate",(*mbox)->pQ!=NULL); 
		if((*mbox)->pQ != NULL)
        {
            return ERR_OK;
        }
		else
		{ 
			free((*mbox));
			return ERR_MEM;
		}
	}
    else return ERR_MEM;
} 

void sys_mbox_free(sys_mbox_t * mbox)
{
	u8_t ucErr;
	(void)OSQDel((*mbox)->pQ, OS_DEL_ALWAYS, &ucErr);
	LWIP_ASSERT( "OSQDel ",ucErr == OS_ERR_NONE ); 
	free((*mbox)); 
	*mbox=NULL;
}

void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{    
	if(msg == NULL)
    {
        msg=(void*)&pvNullPointer;//当msg为空时 msg等于pvNullPointer指向的值 
    }
    while(OSQPost((*mbox)->pQ,msg) != OS_ERR_NONE);
}
//尝试向一个消息邮箱发送消息
//此函数相对于sys_mbox_post函数只发送一次消息，
//发送失败后不会尝试第二次发送
//*mbox:消息邮箱
//*msg:要发送的消息
//返回值:ERR_OK,发送OK
// 	     ERR_MEM,发送失败
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{ 
	if(msg == NULL)
    {
        msg=(void*)&pvNullPointer;//当msg为空时 msg等于pvNullPointer指向的值 
    }
	if((OSQPost((*mbox)->pQ, msg)) != OS_ERR_NONE)
    {
       return ERR_MEM;
    }
	return ERR_OK;
}

//等待邮箱中的消息
//*mbox:消息邮箱
//*msg:消息
//timeout:超时时间，如果timeout为0的话,就一直等待
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
	u8_t ucErr;
	u32_t ucos_timeout,timeout_new;
	void *temp;
	sys_mbox_t m_box=*mbox;
	if(timeout!=0)
	{
		ucos_timeout=(timeout*OS_TICKS_PER_SEC)/1000; //转换为节拍数,因为UCOS延时使用的是节拍数,而LWIP是用ms
		if(ucos_timeout<1)ucos_timeout=1;//至少1个节拍
	}else ucos_timeout = 0; 
	timeout = OSTimeGet(); //获取系统时间 
	temp=OSQPend(m_box->pQ,(u16_t)ucos_timeout,&ucErr); //请求消息队列,等待时限为ucos_timeout
	if(msg!=NULL)
	{	
		if(temp==(void*)&pvNullPointer)*msg = NULL;   	//因为lwip发送空消息的时候我们使用了pvNullPointer指针,所以判断pvNullPointer指向的值
 		else *msg=temp;									//就可知道请求到的消息是否有效
	}    
	if(ucErr==OS_ERR_TIMEOUT)timeout=SYS_ARCH_TIMEOUT;  //请求超时
	else
	{
		LWIP_ASSERT("OSQPend ",ucErr==OS_ERR_NONE); 
		timeout_new=OSTimeGet();
		if (timeout_new>timeout) timeout_new = timeout_new - timeout;//算出请求消息或使用的时间
		else timeout_new = 0xffffffff - timeout + timeout_new; 
		timeout=timeout_new*1000/OS_TICKS_PER_SEC + 1; 
	}
	return timeout; 
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return sys_arch_mbox_fetch(mbox, msg, 1);
}

int sys_mbox_valid(sys_mbox_t *mbox)
{  
	u8_t ucErr;
	int ret;
	OS_Q_DATA q_data;
	memset(&q_data, 0, sizeof(OS_Q_DATA));
	ucErr = OSQQuery ((*mbox)->pQ,&q_data);
	ret=(ucErr<2 && (q_data.OSNMsgs<q_data.OSQSize))?1:0;
	return ret; 
} 

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	*mbox = NULL;
} 

err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{  
	u8_t err; 
	*sem = OSSemCreate((u16_t)count);
	if(*sem == NULL)
    {
        return ERR_MEM; 
    }
	return ERR_OK;
} 
//等待一个信号量
//*sem:要等待的信号量
//timeout:超时时间
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
	u8_t ucErr;
	u32_t ucos_timeout, timeout_new; 
	if(	timeout!=0) 
	{
		ucos_timeout = (timeout * OS_TICKS_PER_SEC) / 1000;//转换为节拍数,因为UCOS延时使用的是节拍数,而LWIP是用ms
		if(ucos_timeout < 1)
		ucos_timeout = 1;
	}else ucos_timeout = 0; 
	timeout = OSTimeGet();  
	OSSemPend (*sem,(u16_t)ucos_timeout, (u8_t *)&ucErr);
 	if(ucErr == OS_ERR_TIMEOUT)timeout=SYS_ARCH_TIMEOUT;//请求超时	
	else
	{     
 		timeout_new = OSTimeGet(); 
		if (timeout_new>=timeout) timeout_new = timeout_new - timeout;
		else timeout_new = 0xffffffff - timeout + timeout_new;
 		timeout = (timeout_new*1000/OS_TICKS_PER_SEC + 1);//算出请求消息或使用的时间(ms)
	}
	return timeout;
}

void sys_sem_signal(sys_sem_t *sem)
{
	OSSemPost(*sem);
}

void sys_sem_free(sys_sem_t *sem)
{
	u8_t ucErr;
	(void)OSSemDel(*sem,OS_DEL_ALWAYS,&ucErr);
	if(ucErr != OS_ERR_NONE)
    {
        LWIP_ASSERT("OSSemDel ",ucErr==OS_ERR_NONE);
    }
	*sem = NULL;
} 

int sys_sem_valid(sys_sem_t *sem)
{
	OS_SEM_DATA  sem_data;
	return (OSSemQuery (*sem,&sem_data) == OS_ERR_NONE )? 1:0;              
} 

void sys_sem_set_invalid(sys_sem_t *sem)
{
	*sem = NULL;
} 

void sys_init(void)
{ 

}

OS_STK TCPIP_THREAD_TASK_STK[1024];
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	OS_CPU_SR cpu_sr;
	if(strcmp(name,TCPIP_THREAD_NAME) == 0)
	{
		OSTaskCreate(thread,arg,(OS_STK*)&TCPIP_THREAD_TASK_STK[stacksize-1],prio);//创建TCP IP内核任务 
	} 
	return 0;
} 


u32_t sys_now(void)
{
	return (OSTimeGet()*1000/OS_TICKS_PER_SEC+1);
}

#else

static uint32_t lwip_timer;//lwip本地时间计数器,单位:ms

void PIT_ISR(void)
{
    lwip_timer++;
}

u32_t sys_now(void)
{
	return lwip_timer*10;
}

u8_t timer_expired(u32_t *last_time,u32_t tmr_interval)
{
	u32_t time;
	time = *last_time;	
	if((lwip_timer-time)>=tmr_interval){
		*last_time = lwip_timer;
		return 1;
	}
	return 0;
}

#endif


