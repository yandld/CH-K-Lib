#include <rtthread.h>
#include <stdint.h>
#include <ir_thread.h>
#include <gpio.h>
#include "pin.h"

#define IR_PIN              ((HW_GPIOE<<8) + 28)
#define BUZZER_PIN          ((HW_GPIOA<<8) + 6)

#define IR_MQ_NAME          "ir_mq"
#define IR_DATA             rt_pin_read(IR_PIN)

extern void DelayUs(uint32_t us);
static rt_mq_t mq;

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

    /* begin decode */
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

void IR_PIN_ISR(uint32_t mask)
{
    uint8_t code[4];
    if(!InfraredExec(code))
    {
        rt_mq_send(mq, code, 4);
    }
}


void ir_init(void)
{
    rt_thread_t tid;
    uint8_t code[4];
    rt_pin_mode(IR_PIN,         PIN_MODE_INPUT);
    
    GPIO_ITDMAConfig((IR_PIN>>8), (IR_PIN & 0xFF), kGPIO_IT_FallingEdge, true);
    GPIO_CallbackInstall((IR_PIN>>8), IR_PIN_ISR);
    mq = rt_mq_create(IR_MQ_NAME, 4, 3, RT_IPC_FLAG_FIFO);
}


static void ir_recv_thread(void *arg)
{
    uint8_t code[4];
    rt_pin_mode(BUZZER_PIN,     PIN_MODE_OUTPUT);
    
    rt_mq_t  mq = (rt_mq_t)rt_object_find(IR_MQ_NAME, RT_Object_Class_MessageQueue);
    if(mq)
    {
        rt_kprintf("ir start scaning...\r\n");
        while(1)
        {
            if(rt_mq_recv(mq, code, sizeof(code), RT_WAITING_FOREVER) == RT_EOK)
            {
                rt_pin_write(BUZZER_PIN, 1);
                rt_thread_delay(4);
                rt_pin_write(BUZZER_PIN, 0);
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
FINSH_FUNCTION_EXPORT(ir_recv, ir_rev test.);
