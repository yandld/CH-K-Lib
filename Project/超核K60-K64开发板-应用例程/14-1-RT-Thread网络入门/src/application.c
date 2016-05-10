#include <rtthread.h>
#include <dfs_fs.h>
#include <dfs_init.h>
#include <dfs_elm.h>
#include <shell.h>
#include <lwip/sys.h>
#include <dfs_posix.h>
#include <netif/ethernetif.h>
extern void lwip_system_init(void);
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
	eth_system_device_init();
	lwip_system_init();
    
    dfs_init();
    elm_init();
    
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_kprintf("rt-thread system start!\r\n");
    
    finsh_system_init();
    
    rt_hw_dflash_init("dflash0");
    if(dfs_mount("dflash0", "/", "elm", 0, 0) != RT_EOK)
    {
        rt_kprintf("mount dflash0 failed. format file system...\r\n");
        dfs_mkfs("elm", "dflash0");
    }
    
    dfs_mount("dflash0", "/", "elm", 0, 0);
    
    mkdir("/www", 0);
    int fd = open("/www/index.htm", O_RDWR | O_CREAT | O_TRUNC, 0);
    write(fd, "Hello!", 6);
    close(fd);

    rt_hw_enet_phy_init();
    
    rt_kprintf("waitting for connection...");
   
    /* tcp server demo */
    //extern void tcpserv(void* parameter);
    //tcpserv(RT_NULL);
    webnet_init();
    
}

void rt_application_init(void* parameter)
{
    rt_thread_t tid;
    tid = rt_thread_create("init", init_thread, RT_NULL, 1024, 8, 20);
    rt_thread_startup(tid);
}

