#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "chlib_k.h"
#include "sram.h"
#include "rtt_drv.h"

#define SYS_HEAP_SIZE           (1024*32)
volatile static uint8_t SYSHEAP[SYS_HEAP_SIZE];

void init_thread_entry(void* parameter)
{
    rt_err_t err;
    rt_thread_t tid;
    
    SRAM_Init();
    err = SRAM_SelfTest();
    if(err)
        rt_system_heap_init((void*)SYSHEAP, (void*)(SYS_HEAP_SIZE + (uint32_t)SYSHEAP));
    else
        rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));
    rt_system_comonent_init();
    rt_thread_delay(1);
    
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_kprintf("rt-thread system start!\r\n");
    
    finsh_system_init();
    
    rt_hw_dflash_init("dflash0");
    
    if(dfs_mount("sf0", "/", "elm", 0, 0))
    {
        dfs_mkfs("elm", "sf0");
        err = dfs_mount("sf0", "/", "elm", 0, 0);
    }
    
    if(err)
    {
        if(dfs_mount("dflash0", "/", "elm", 0, 0))
        {
            dfs_mkfs("elm", "dflash0");
            dfs_mount("dflash0", "/", "elm", 0, 0);
        }
    }

    rt_hw_ksz8041_init();
    
    rt_kprintf("waitting for connection...");
    
    /* tcp server demp */
    tcpserv();

    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

