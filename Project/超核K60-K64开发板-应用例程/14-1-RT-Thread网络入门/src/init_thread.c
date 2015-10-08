#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "chlib_k.h"
#include "IS61WV25616.h"
#include "rtt_drv.h"

#define SYS_HEAP_SIZE           (1024*32)

extern void rt_system_comonent_init(void);


void rt_heap_init(void)
{
    rt_err_t err;
    volatile static uint8_t SYSHEAP[SYS_HEAP_SIZE];
    SRAM_Init();
    err = SRAM_SelfTest();
    if(err)
        rt_system_heap_init((void*)SYSHEAP, (void*)(SYS_HEAP_SIZE + (uint32_t)SYSHEAP));
    else
        rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));
}

void init_thread(void* parameter)
{
    rt_err_t err;
    rt_system_comonent_init();
    rt_thread_delay(1);
    
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_kprintf("rt-thread system start!\r\n");
    
    finsh_system_init();
    
    rt_hw_dflash_init("dflash0");
    dfs_mount("dflash0", "/", "elm", 0, 0);
    rt_hw_enet_phy_init();
    
    rt_kprintf("waitting for connection...");
    
    /* tcp server demp */
    tcpserv();

}

void rt_application_init(void* parameter)
{
    rt_thread_t tid;
    tid = rt_thread_create("init", init_thread, RT_NULL, 1024, 8, 20);
    rt_thread_startup(tid);
}

