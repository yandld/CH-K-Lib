#include <rtthread.h>
#include <rthw.h>

#include "components.h"
#include "rtt_ksz8041.h"
#include <drivers/spi.h>
#include "spi_flash_w25qxx.h"

extern void gui_thread_entry(void* parameter);
extern void led_thread_entry(void* parameter);
extern void sd_thread_entry(void* parameter);

rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);

void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_err_t r;

#ifdef RT_USING_FINSH
	finsh_system_init(); /* init finsh */
#endif
#ifdef RT_USING_DFS
	dfs_init();
#endif
#ifdef RT_USING_DFS_ELMFAT
	elm_init();
#endif
    
   // touch_ads7843_init("ads7843", "spi20");
    r = w25qxx_init("sf0", "spi21");
    RT_DEBUG_LOG(RT_TRUE, ("w25qxx init@%d...\r\n", r));
    r = dfs_mount("sf0", "/", "elm", 0, 0);
    RT_DEBUG_LOG(RT_TRUE, ("dfs_mount@%d...\r\n", r));
    
#ifdef RT_USING_LWIP
    RT_DEBUG_LOG(RT_TRUE, ("Initalizing enet system...\r\n"));
    eth_system_device_init();
    rt_hw_ksz8041_init(0x01);
	lwip_system_init();
#endif




    

    /* sd_thread */
    thread = rt_thread_create("sd", sd_thread_entry, RT_NULL, 1024, 0x23, 20); 
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    
   /* gui thread */
    thread = rt_thread_create("gui", gui_thread_entry, RT_NULL, 1024*8, 0x23, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    /* led thread */
    thread = rt_thread_create("led", led_thread_entry, RT_NULL, 1024*8, 0x21, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}

