#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "dfs_posix.h"
#include "spi_flash_w25qxx.h"
#include "chlib_k.h"
#include "sram.h"
#include "rtt_drv.h"


    
    
void usb_thread_entry(void* parameter);
void sd_thread_entry(void* parameter);
int ui_startup(int argc, char** argv);


rt_err_t ads7843_init(const char * name, const char * spi_device_name);

void init_thread_entry(void* parameter)
{
    rt_thread_t tid;
    rt_err_t err;
    
    #ifndef FRDM
    SRAM_Init();
    rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));
    #else
    rt_system_heap_init((void*)(0x1FFF0000), (void*)(0x1FFF0000 + 0x10000));
    #endif
    
    rt_thread_delay(1);
    
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
    rt_kprintf("rt-thread system start!\r\n");
    
    finsh_system_init();
    
    rt_hw_spi_init();
    rt_hw_rtc_init();
    rt_hw_dflash_init("dflash0");
    rt_hw_sd_init();
    /* start usbd core thread */
    tid = rt_thread_create("usbd", usb_thread_entry, RT_NULL, (1024*1), 0x15, 20);                                                      
    rt_thread_startup(tid);	
    
    ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");

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
    
    
    at24cxx_init("at24c02", "i2c0");

    #ifndef FRDM
    ui_startup(RT_NULL, RT_NULL);
    #endif
    
    rt_hw_ksz8041_init();
    
    
    /* ceate default index.html*/
    int fd;
    char buf[64] = "Hello Webnet!";
    
    mkdir("/www", 0x777);
    fd = open("/www/index.htm", O_RDWR|O_CREAT, 0);
    write(fd, buf, sizeof(buf));
    close(fd);
    
    /* init webnet server root is /www */
    webnet_init();
    
    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

