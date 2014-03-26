#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "cpuidy.h"
#include "common.h"

#include <rtthread.h>
#include <rthw.h>
#include "finsh.h"
#include "shell.h"
#include "sram.h"

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define KINETIS_SRAM_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define KINETIS_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define KINETIS_SRAM_BEGIN    (&__bss_end)
#endif

#define KINETIS_SRAM_SIZE_IN_KB         (16)
#define KINETIS_SRAM_END                (0x20000000 + KINETIS_SRAM_SIZE_IN_KB * 1024)


void rt_hw_board_init(void)
{
    SYSTICK_Init(10);
    SYSTICK_ITConfig(ENABLE);
    SYSTICK_Cmd(ENABLE);
	rt_hw_usart_init();
#ifdef RT_USING_CONSOLE
	rt_console_set_device("uart");
#endif 
    SRAM_Init();
    if(SRAM_SelfTest())
    {
        printf("SRAM SELF TEST FAILED\r\n");
        while(1);
    }
}


void rtthread_startup(void)
{
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(1);
//    CMD_FLEXBUS(0, NULL);
    rt_hw_board_init();
	rt_show_version(); /* print logo */
	rt_system_tick_init();  /* init tick system */
	rt_system_object_init();
	rt_system_timer_init();
    rt_system_heap_init((void*)KINETIS_SRAM_BEGIN, (void*)KINETIS_SRAM_END); /* register system IRAM */
    rt_system_heap_init((void*)SRAM_START_ADDRESS, (void*)(SRAM_SIZE + SRAM_START_ADDRESS)); /* register ERAM */
	rt_system_scheduler_init();
#ifdef RT_USING_FINSH
	finsh_system_init(); /* init finsh */
#endif
	rt_device_init_all(); /* register all devices */
    rt_system_timer_thread_init(); /* enable software timer system */
	rt_thread_idle_init(); /* init idle thread */
	rt_system_scheduler_start(); /* brunning the system */
    
}
