#include <rtthread.h>
#include <rthw.h>
#include <dfs_romfs.h> 
#include "board.h"
#include "components.h"
#include "rtt_ksz8041.h"
#include <drivers/spi.h>
#include "spi_flash_w25qxx.h"

void led_thread_entry(void* parameter);
void sd_thread_entry(void* parameter);
void gui_thread_entry(void* parameter);

rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);

void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_err_t r;
    rt_uint8_t time_out;
	dfs_init();
	elm_init();
    dfs_romfs_init(); 
    dfs_mount(RT_NULL, "/", "rom", 0, &romfs_root);
    RT_DEBUG_LOG(RT_TRUE, ("root rom file system inialized..\r\n"));

#ifdef RT_USING_LWIP
	eth_system_device_init();
	lwip_system_init();
    RT_DEBUG_LOG(RT_TRUE, ("TCP/IP initialized..\r\n"));
    rt_kprintf(" link beginning \r\n");
    rt_hw_ksz8041_init(BOARD_ENET_PHY_ADDR);
    time_out = 0;
	while(!(netif_list->flags & NETIF_FLAG_UP)) 
	{
		rt_thread_delay(RT_TICK_PER_SECOND);
        if((time_out++) > 3)
        {
            rt_kprintf("link failed\r\n");
            break;
        }
	}
    list_if();
#ifdef RT_USING_DFS_NFS
    nfs_init();
#endif
#endif
    

    // touch_ads7843_init("ads7843", "spi20");
    r = w25qxx_init("sf0", "spi21");
    if(r) RT_DEBUG_LOG(RT_TRUE, ("w25qxx init fail@%d...\r\n", r));
    r = dfs_mount("sf0", "/SF", "elm", 0, 0);
    if(r) RT_DEBUG_LOG(RT_TRUE, ("sf0 mount fail@%d...\r\n", r));
    
    /* sd_thread */
    thread = rt_thread_create("sd", sd_thread_entry, RT_NULL, 1024, 0x23, 20); 
    if (thread != RT_NULL) rt_thread_startup(thread);
         
   /* gui thread */
  //  thread = rt_thread_create("gui", gui_thread_entry, RT_NULL, 1024*8, 0x23, 20);
  //  if (thread != RT_NULL) rt_thread_startup(thread);
        
    /* led thread */
    thread = rt_thread_create("led", led_thread_entry, RT_NULL, 1024, 0x24, 20);
    if (thread != RT_NULL) rt_thread_startup(thread);
    
	finsh_system_init(); /* init finsh */
    
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}

