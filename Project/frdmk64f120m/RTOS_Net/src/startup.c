#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "common.h"
#include "board.h"
#include <rtthread.h>
#include <rthw.h>
#include "finsh.h"
#include "shell.h"
#include "sram.h"
#include "sd.h"

#include <drivers/spi.h>
#include "rtt_spi.h"

#include "components.h"
#include "spi.h"

#ifdef __CC_ARM
extern int Image$$RW_IRAM2$$ZI$$Limit;
extern int Image$$RW_IRAM1$$ZI$$Limit;


#define KINETIS_IRAM1_LIMIT     (&Image$$RW_IRAM1$$ZI$$Limit)
#define KINETIS_IRAM2_LIMIT     (&Image$$RW_IRAM2$$ZI$$Limit)
#define KINETIS_SRAM_BEGIN    (&Image$$RW_IRAM2$$ZI$$Limit)


#elif __ICCARM__
#pragma section="HEAP"
#define KINETIS_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define KINETIS_SRAM_BEGIN    (&__bss_end)
#endif

#define RTT_IRAM_HEAP_START               (0x1FFF0000)
#define RTT_IRAM_HEAP_END                 (0x1FFF0000 + 0x10000)


int rt_hw_usart_init(uint32_t instance, const char * name);
void rt_hw_sd_init(uint32_t instance, const char *name);

void rt_hw_board_init(void)
{
    rt_hw_usart_init(HW_UART0, "uart0");
	rt_console_set_device("uart0");
 
    SYSTICK_Init(1000*1000/RT_TICK_PER_SECOND);
    SYSTICK_ITConfig(ENABLE);
    SYSTICK_Cmd(ENABLE);
}


extern void init_thread_entry(void* parameter);

void rt_application_init(void)
{
    rt_thread_t init_thread;
    /* init thread */
    init_thread = rt_thread_create("init", init_thread_entry, RT_NULL, 2048, 0x20, 20);                       
    if (init_thread != RT_NULL)
    {
        rt_thread_startup(init_thread);		
    }
}

void rtthread_startup(void)
{
    /* BM init */
    DelayInit();
    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("IRAM: BEIGN:0x%08XU END:0x%08XU SIZE:%dKB\r\n", (uint32_t)RTT_IRAM_HEAP_START, (uint32_t)RTT_IRAM_HEAP_END, ((uint32_t)RTT_IRAM_HEAP_END - (uint32_t)RTT_IRAM_HEAP_START)/1024);
    rt_system_heap_init((void*)RTT_IRAM_HEAP_START, (void*)RTT_IRAM_HEAP_END);
    /*
    SRAM_Init();
    if(SRAM_SelfTest())
    {
        printf("SRAM SelfTest failed will use IRAM\r\n");
        rt_system_heap_init((void*)RTT_IRAM_HEAP_START, (void*)RTT_IRAM_HEAP_END);
    }

        rt_system_heap_init((void*)SRAM_ADDRESS_BASE, (void*)(SRAM_SIZE + SRAM_ADDRESS_BASE));
    }
    */

    rt_hw_board_init();
	rt_show_version(); /* print logo */
	rt_system_timer_init(); /* init timer */
    
    
	rt_system_scheduler_init();

	rt_device_init_all();
    rt_system_timer_thread_init();
	rt_thread_idle_init();
    rt_application_init();
	rt_system_scheduler_start();
}


