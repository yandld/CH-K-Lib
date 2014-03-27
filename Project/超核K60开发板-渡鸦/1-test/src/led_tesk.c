#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_elm.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui_server.h>
#include <driver.h>

#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

void led1_thread_entry(void* parameter)
{
	rt_uint32_t i;
    dfs_init();
    elm_init();
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("sd0 mount to /SD \n");
    }
    else
    {
        rt_kprintf("sd0 mount to /SD failed!\n");
    }

    rt_device_t device;

    device = rt_device_find("lcd");
    if (device == RT_NULL)
    {
        rt_kprintf("no graphic device in the system.\n");
        return ;
    }

    /* re-set graphic device */
    rtgui_graphic_set_device(device);
    rtgui_system_server_init();
    
    
    {
        extern void application_init();
        application_init();
    }
    
	while(1)
	{
        rt_thread_delay(100);

	}
}