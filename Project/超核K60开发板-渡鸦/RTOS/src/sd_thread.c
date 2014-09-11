#include <rtthread.h>
#include <rthw.h>

#include "sd.h"
#include <rtthread.h>
#include <dfs_posix.h>

/* make SD card removable */

void sd_thread_entry(void* parameter)
{
    rt_thread_t thread = rt_thread_self();
    
    int r;
    rt_device_t  dev = rt_device_find("sd0");
    if(dev == RT_NULL)
    {
        rt_thread_suspend(thread); 
    }
    
	DIR *dir;
	dir = opendir("/SD");
    if (dir == RT_NULL)
    {
        mkdir("/SD", 0x777);
    }
    
    dfs_mount("sd0", "/SD", "elm", 0, 0);

	while(1)
	{
        if(dev->open_flag == RT_DEVICE_OFLAG_CLOSE)
        {
            dfs_unmount("/SD");
            r = dfs_mount("sd0", "/SD", "elm", 0, 0);
            if(!r)
            {
                rt_kprintf("sd0 mounted on /SD !\r\n");
            }
        }
        rt_thread_delay(100);
	}
}
