#include <rtthread.h>
#include <rthw.h>
#include <dfs_romfs.h> 
#include "board.h"
#include "components.h"
#include "rtt_ksz8041.h"
#include <drivers/spi.h>
#include "spi_flash_w25qxx.h"

extern void led_thread_entry(void* parameter);
extern void sd_thread_entry(void* parameter);

rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);

void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_err_t r;

	dfs_init();
	elm_init();
    dfs_romfs_init(); 
    dfs_mount(RT_NULL, "/", "rom", 0, &romfs_root);
    RT_DEBUG_LOG(RT_TRUE, ("file system inialized..\r\n"));

#ifdef RT_USING_LWIP
	eth_system_device_init();
	lwip_system_init();
    RT_DEBUG_LOG(RT_TRUE, ("TCP/IP initialized!..\r\n"));
    RT_DEBUG_LOG(RT_TRUE, ("initalizing enet phy...\r\n"));
    rt_hw_ksz8041_init(BOARD_ENET_PHY_ADDR);
#endif

    // touch_ads7843_init("ads7843", "spi20");
    r = w25qxx_init("sf0", "spi21");
    if(r) RT_DEBUG_LOG(RT_TRUE, ("w25qxx init fail@%d...\r\n", r));
    r = dfs_mount("sf0", "/SF", "elm", 0, 0);
    if(r) RT_DEBUG_LOG(RT_TRUE, ("sf0 mount fail@%d...\r\n", r));
    

    /* sd_thread */
    thread = rt_thread_create("sd", sd_thread_entry, RT_NULL, 1024, 0x23, 20); 
    if (thread != RT_NULL) rt_thread_startup(thread);
         
//   /* gui thread */
//    thread = rt_thread_create("gui", gui_thread_entry, RT_NULL, 1024*8, 0x23, 20);
//    if (thread != RT_NULL)
//    {
//        rt_thread_startup(thread);
//    }
    /* led thread */
    thread = rt_thread_create("led", led_thread_entry, RT_NULL, 1024, 0x24, 20);
    if (thread != RT_NULL) rt_thread_startup(thread);
    
	finsh_system_init(); /* init finsh */
    
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}

