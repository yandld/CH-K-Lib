#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "chlib_k.h"
#include "sram.h"
#include "rtt_drv.h"
#include "pin.h"



void usb_thread_entry(void* parameter);


void init_thread_entry(void* parameter)
{
    int ret;
    rt_thread_t tid;

    rt_system_comonent_init();
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_hw_sd_init();
    rt_hw_rtc_init();
    rt_hw_spi_init();
    rt_hw_pin_init("gpio");
    rt_hw_ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    rt_hw_lcd_init("lcd0");

    finsh_system_init();
 //   tid = rt_thread_create("usb", usb_thread_entry, RT_NULL, 1024, 9, 20);
   // rt_thread_startup(tid);
    
    tid = rt_thread_create("init", (void*)(0x70400), RT_NULL, 1024, 8, 20);
    rt_thread_startup(tid);

    rt_hw_ksz8041_init();

    tid = rt_thread_self();
    rt_thread_delete(tid); 
    MainTask();
    GUI_TOUCH_Exec();
}


void rt_application_init(void)
{
    int ret;
    rt_thread_t tid;
    SRAM_Init();
    ret = SRAM_SelfTest();
//    if(ret)
//        rt_system_heap_init((void*)SYSHEAP, (void*)(sizeof(SYSHEAP) + (uint32_t)SYSHEAP));
//    else
      rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));

    tid = rt_thread_create("init", init_thread_entry, RT_NULL, 1024, 20, 20);
    rt_thread_startup(tid);
}




