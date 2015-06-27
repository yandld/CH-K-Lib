#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "chlib_k.h"
#include "sram.h"
#include "rtt_drv.h"

#include "rtt_api.h"

const shell_t rshell = 
{
    finsh_system_init,
    finsh_set_device,
    finsh_get_device,
};

const rtthread_t rtthread = 
{
    rt_kprintf,
    rt_vsprintf,
    rt_vsnprintf,
    rt_sprintf,
    rt_snprintf,
    rt_console_set_device,
    rt_console_get_device,
    rt_thread_delay,
};


const api_t api = 
{
    &rshell,
    &rtthread,
};



void init_thread_entry(void* parameter)
{
    int ret;
    rt_thread_t tid;

    rt_system_comonent_init();
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_kprintf("rt-thread system start!\r\n");
    finsh_system_init();
    rt_hw_sd_init();
    rt_hw_rtc_init();
    rt_hw_spi_init();
    rt_hw_ksz8041_init();
    ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    tid = rt_thread_create("init", (void*)(0x40100), (void*)(&api), 1024, 20, 20);
    rt_thread_startup(tid);


    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

uint8_t SYSHEAP[1024*64];
void rt_application_init(void)
{
    int ret;
    rt_thread_t tid;
    SRAM_Init();
    ret = SRAM_SelfTest();
    if(ret)
        rt_system_heap_init((void*)SYSHEAP, (void*)(sizeof(SYSHEAP) + (uint32_t)SYSHEAP));
    else
        rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));
    
    tid = rt_thread_create("init", init_thread_entry, RT_NULL, 1024, 20, 20);
    rt_thread_startup(tid);
}




