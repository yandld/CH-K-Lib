#include <rtthread.h>
#include <stdint.h>
#include <ir_thread.h>
#include "pin.h"

#define IR_PIN          ((4<<8)+28)
#define IR_DATA         rt_pin_read(IR_PIN)

#define BUZZER_PIN      ((0<<8)+6)

extern void DelayUs(uint32_t us);

/* 红外解码数据 */
static uint8_t InfraredExec(uint8_t * code)
{
	uint8_t i,j;
	uint32_t time_out = 0;
	while((IR_DATA == 0) && (time_out < 30*10))
	{
		time_out++;
		DelayUs(100);
	}
	if(time_out > 92 || time_out < 87) return 1;
	time_out = 0;
	while((IR_DATA == 1) && (time_out < 30*10))
	{
		time_out++;
		DelayUs(100);
	}
	if((time_out > 47) || (time_out < 43)) return 2;

	//开始解码
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 8; j++)
		{
			while(IR_DATA == 0); //浪费掉低电平时间
			time_out = 0;
			while(IR_DATA == 1)  //读取高电平时间
			{
				DelayUs(100);
				time_out++;
			}
			code[i] = code[i]>>1;
			if(time_out > 9) code[i] |= 0x80;	 //1.68为高电平
			
		}
	}
	return 0;
}

static void ir_thread(void *arg)
{
    uint8_t code[4];
    rt_pin_mode(IR_PIN,         PIN_MODE_INPUT);
    rt_pin_mode(BUZZER_PIN,     PIN_MODE_OUTPUT);

    rt_mq_t mq = rt_mq_create(IR_MQ_NAME, sizeof(ir_msg), 3, RT_IPC_FLAG_FIFO);
    
    if(mq)
    {
        while(1)
        {
            if(!InfraredExec(code))
            {
                rt_pin_write(BUZZER_PIN, 1);
                rt_thread_delay(4);
                rt_pin_write(BUZZER_PIN, 0);
                rt_mq_send(mq, code, 4);
            }
        }
    }
    else
    {
        rt_kprintf("cannot create ir message queue!\r\n");
    }
}

void ir_init(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("ir_thread", ir_thread, RT_NULL, 256, 26, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
}


static void ir_recv_thread(void *arg)
{
    uint8_t code[4];
    rt_mq_t  mq = (rt_mq_t)rt_object_find(IR_MQ_NAME, RT_Object_Class_MessageQueue);

    if(mq)
    {
        rt_kprintf("ir start scaning...\r\n");
        while(1)
        {
            if(rt_mq_recv(mq, code, sizeof(code), RT_WAITING_FOREVER) == RT_EOK)
            {
                rt_kprintf("infrared:0x%02X 0x%02X 0x%02X 0x%02X\r\n", code[0], code[1], code[2], code[3]);
            }
        }   
    }
}

void ir_recv(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("ir_iecv", ir_recv_thread, RT_NULL, 256, 26, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
}


#include <finsh.h>
FINSH_FUNCTION_EXPORT(ir_init, ir_init.);
FINSH_FUNCTION_EXPORT(ir_recv, ir_rev test.);
