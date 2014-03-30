#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>

#include <drivers/spi.h>



void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t t_self = rt_thread_self();
    rt_kprintf("start init thread\r\n");
    
    dfs_init();
    elm_init();
    
    spi_device = (struct rt_spi_device *)rt_device_find("spi20");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device found\r\n");
        rt_thread_suspend(t_self);
    }
    
    if(w25qxx_init("spi_flash", "spi20")!= RT_EOK)
    {
        rt_kprintf("init spi flash failed\r\n");
        rt_thread_suspend(t_self); 
    }
    /* mount spi_flash */
    if (dfs_mount("spi_flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("spi_flash mount to / \n");
    }
    else
    {
        rt_kprintf("spi_flash mount to / failed!\n");
        rt_thread_suspend(t_self); 
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

    
    
    GUI_Init();//≥ı ºªØGUI 
	//GUI_DispString("Hello emWin!");//œ‘ æ≤‚ ‘  
 //   MainTask();
	while(1)
	{
        rt_thread_delay(100);
	}
}