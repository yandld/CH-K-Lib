#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>

#include <drivers/spi.h>

extern void gui_thread_entry(void* parameter);


void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_kprintf("start init thread\r\n");
    
    dfs_init();
    elm_init();
    
    spi_device = (struct rt_spi_device *)rt_device_find("spi20");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device found\r\n");
        rt_thread_suspend(thread);
    }
    
    if(w25qxx_init("spi_flash", "spi20")!= RT_EOK)
    {
        rt_kprintf("init spi flash failed\r\n");
        rt_thread_suspend(thread); 
    }
    /* mount spi_flash */
    if (dfs_mount("spi_flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("spi_flash mount to / \n");
    }
    else
    {
        rt_kprintf("spi_flash mount to / failed!\n");
        rt_thread_suspend(thread); 
    }
    /* mount SD */
    if (dfs_mount("sd0", "/SD", "elm", 0, 0) == 0)
    {
        rt_kprintf("sd0 mount to /SD \n");
    }
    else
    {
        rt_kprintf("sd0 mount to /SD failed!\n");
    }
    

 //   MainTask();

    /* gui thread */
    thread = rt_thread_create("gui", gui_thread_entry, RT_NULL, 4096, 0x23, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}

