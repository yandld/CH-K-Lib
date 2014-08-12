#include <rtthread.h>
#include <rthw.h>

#include "components.h"

#include <drivers/spi.h>
#include "spi_flash_w25qxx.h"

extern void gui_thread_entry(void* parameter);
extern void led_thread_entry(void* parameter);


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
#endif
    
    
#ifdef RT_USING_DFS
    rt_kprintf("init dfs\r\n");
	dfs_init();
    
#ifdef RT_USING_DFS_ELMFAT
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


    /* mount spi_flash */
    dfs_mount("sd0", "/", "elm", 0, 0);

#ifdef RT_USING_FINSH
	finsh_system_init(); /* init finsh */
#endif

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

