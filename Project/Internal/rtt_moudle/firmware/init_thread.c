#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "chlib_k.h"
#include "sram.h"
#include "rtt_drv.h"
#include "pin.h"

#include "rtt_api.h"

const finsh_t finsh = 
{
    finsh_syscall_append,
};

const shell_t rshell = 
{
    finsh_system_init,
    finsh_set_device,
    finsh_get_device,
};

const pin_t pin = 
{
    rt_pin_mode,
    rt_pin_write,
    rt_pin_read,
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
    rt_malloc,
    rt_free,
    rt_thread_delay,
    rt_tick_get,
    rt_device_find,
    rt_device_init,
    rt_device_register,
    rt_device_control,
    rt_device_open,
    rt_device_read,
    rt_device_write,
    rt_thread_create,
    rt_thread_startup,
    rt_thread_find,
};

const api_t api = 
{
    &pin,
    &rshell,
    &rtthread,
    &finsh,
};

void init_thread_entry(void* parameter)
{
    int ret;
    rt_thread_t tid;

    rt_system_comonent_init();
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_hw_sd_init();
    rt_hw_rtc_init("rtc");
    rt_hw_spi_init();
    rt_hw_pin_init("gpio");
    ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    rt_hw_lcd_init("lcd0");

    finsh_system_init();
    tid = rt_thread_create("init", (void*)(0x40400), (void*)(&api), 1024, 8, 20);
    rt_thread_startup(tid);

    rt_hw_ksz8041_init();

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

 //   void(*theUboot)(void);
 //   theUboot = (void(*)(void))(0x40800);
    //theUboot();

    tid = rt_thread_create("init", init_thread_entry, RT_NULL, 1024, 20, 20);
    rt_thread_startup(tid);
}




