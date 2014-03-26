#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_elm.h>

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
    //LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
	while(1)
	{
        rt_thread_delay(100);
      //  rt_kprintf("!!!\r\n");
		 // for(i=0;i<7000;i++);
		  //LED_Toggle(kLED1);
	}
}