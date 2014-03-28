#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>




void led1_thread_entry(void* parameter)
{
    int fd;
	rt_int32_t i;
    rt_uint8_t * ptr;
    struct stat f_stat;
    
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

    GUI_Init();//≥ı ºªØGUI 
	GUI_DispString("Hello emWin!");//œ‘ æ≤‚ ‘  
   // MainTask();
	while(1)
	{
        
        rt_thread_delay(100);
	}
}