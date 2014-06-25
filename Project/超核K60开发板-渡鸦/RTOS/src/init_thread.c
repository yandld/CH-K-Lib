#include <rtthread.h>
#include <rthw.h>

#include "components.h"

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
    rt_err_t result;
    
#ifdef RT_USING_LWIP
	/* initialize lwip stack */
    eth_system_device_init();
	/* register ethernetif device */
    rt_hw_ksz8041_init();
	/* initialize lwip system */
	lwip_system_init();
#endif  /* RT_USING_LWIP */
    
    rt_kprintf("start init thread @ DFS:\r\n");
#ifdef RT_USING_DFS
	/* initialize the device file system */
	dfs_init();
    
#ifdef RT_USING_DFS_ELMFAT
	/* initialize the elm chan FatFS file system*/
	elm_init();
#endif
    
#ifdef RT_USING_DFS_YAFFS2
	dfs_yaffs2_init();
#endif
    
#ifdef RT_USING_DFS_UFFS
	dfs_uffs_init();
#endif 

#if defined(RT_USING_DFS_NFS) && defined(RT_USING_LWIP)
	/* initialize NFSv3 client file system */
	nfs_init();
#endif

#endif /* RT_USING_DFS */

    /* attacted ads7843 to spi20 */
    touch_ads7843_init("ads7843", "spi20");

    /* attacted ads7843 to spi21 */
    w25qxx_init("spi_flash", "spi21");

    /* mount spi_flash */
    dfs_mount("spi_flash", "/", "elm", 0, 0);

    
#ifdef RT_USING_FINSH
	finsh_system_init(); /* init finsh */
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

