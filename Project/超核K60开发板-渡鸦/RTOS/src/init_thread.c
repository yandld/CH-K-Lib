#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>

#include <drivers/spi.h>

extern void gui_thread_entry(void* parameter);
extern void led_thread_entry(void* parameter);
/* static thread source */
static rt_uint8_t gui_thread_stack[1024*4];
static struct rt_thread gui_thread;

void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_err_t result;
    rt_kprintf("start init thread\r\n");
    dfs_init();
    elm_init();
    /* check device */
    spi_device = (struct rt_spi_device *)rt_device_find("spi21");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device(bus2 - device1) found\r\n");
        rt_thread_suspend(thread);
    }
    spi_device = (struct rt_spi_device *)rt_device_find("spi20");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device(bus2 - device0) found\r\n");
        rt_thread_suspend(thread);
    }
    /* attacted ads7843 to spi20 */
    if(touch_ads7843_init("ads7843", "spi20") != RT_EOK)
    {
        rt_kprintf("init touch failed\r\n");
        rt_thread_suspend(thread); 
    }
    /* attacted ads7843 to spi21 */
    if(w25qxx_init("spi_flash", "spi21")!= RT_EOK)
    {
        rt_kprintf("init spi flash failed\r\n");
        rt_thread_suspend(thread); 
    }
    /* mount spi_flash */
    if (dfs_mount("spi_flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("spi_flash mount to / OK \n");
    }
    else
    {
        rt_kprintf("spi_flash mount to / failed!\n");
        rt_kprintf("try to format disk...\r\n");
        mkfs("elm", "spi_flash");
        dfs_mount("spi_flash", "/", "elm", 0, 0);
    }
    if (dfs_mount("sd0", "/SD", "elm", 0, 0) == 0)
    {
        rt_kprintf("sd0 mount to /SD \n");
    }
    else
    {
        rt_kprintf("sd0 mount to /SD failed!\n");
    }
   
#ifdef RT_USING_LWIP
    
	/* initialize lwip stack */
    eth_system_device_init();
	/* register ethernetif device */
    rt_hw_ksz8041_init();
	/* initialize lwip system */
	lwip_system_init();
	rt_kprintf("TCP/IP initialized!\n");
#endif
#if defined(RT_USING_DFS) && defined(RT_USING_LWIP) && defined(RT_USING_DFS_NFS)
{

/* NFSv3 Initialization */
    rt_kprintf("begin init NFSv3 File System ...\n");
    nfs_init();
    if (dfs_mount(RT_NULL, "/NFS", "nfs", 0, RT_NFS_HOST_EXPORT) == 0)
    rt_kprintf("NFSv3 File System initialized!\n");
    else
    rt_kprintf("NFSv3 File System initialzation failed!\n");
}
#endif
    /* gui thread , use static thread*/
    result = rt_thread_init(&gui_thread,
                            "gui",
                            gui_thread_entry,
                            RT_NULL,
                            &gui_thread_stack[0],
                            sizeof(gui_thread_stack),
                            0x23,
                            20);
   // thread = rt_thread_create("gui", gui_thread_entry, RT_NULL, 1024*8, 0x23, 20);                                                      
    if (result == RT_EOK)
    {
        rt_thread_startup(&gui_thread);		
    }
    /* led thread */
    thread = rt_thread_create("led", led_thread_entry, RT_NULL, 1024*4, 0x21, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}

