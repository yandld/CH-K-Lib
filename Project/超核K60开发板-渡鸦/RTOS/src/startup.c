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

#include <drivers/spi.h>

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

#define KINETIS_SRAM_SIZE_IN_KB         (64)
#define KINETIS_SRAM_END                (0x20000000 + KINETIS_SRAM_SIZE_IN_KB * 1024)

static struct rt_spi_device spi_device;
    
void rt_hw_board_init(void)
{
    DelayInit();
    rt_hw_usart_init();
#ifdef RT_USING_CONSOLE
	rt_console_set_device("uart");
#endif 
    SRAM_Init();
    ili9320_Init();
    
    rt_hw_sd_init();
    rt_hw_lcd_init();
    rt_hw_spi_init();
    /* attacted */
    rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", RT_NULL);
    
    SYSTICK_Init(1000*1000/RT_TICK_PER_SECOND);
    SYSTICK_ITConfig(ENABLE);
    SYSTICK_Cmd(ENABLE);
}


extern void init_thread_entry(void* parameter);
void rt_application_init(void)
{
    rt_thread_t init_thread;
    
    init_thread = rt_thread_create("init",
                                   init_thread_entry, RT_NULL,
                                   2048, 0x20, 20);
    if (init_thread != RT_NULL)
    {
        rt_thread_startup(init_thread);		
    }

}

void rtthread_startup(void)
{
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(0);
//    CMD_FLEXBUS(0, NULL);
    rt_hw_board_init();
	rt_show_version(); /* print logo */
	rt_system_timer_init(); /* init timer */
    /* register IARM and extern RAM */
  //  rt_system_heap_init((void*)KINETIS_SRAM_BEGIN, (void*)KINETIS_SRAM_END);
    SRAM_Init();
    if(SRAM_SelfTest())
    {
        printf("ex sram test failed\r\n");
    }
    else
    {
        rt_system_heap_init((void*)SRAM_START_ADDRESS, (void*)(SRAM_SIZE + SRAM_START_ADDRESS));
    }
    
	rt_system_scheduler_init();
    rt_application_init(); /* init application */
#ifdef RT_USING_FINSH
	finsh_system_init(); /* init finsh */
#endif
	rt_device_init_all(); /* register all devices */
    rt_system_timer_thread_init(); /* enable software timer system */
	rt_thread_idle_init(); /* init idle thread */
	rt_system_scheduler_start(); /* running the system */
}
