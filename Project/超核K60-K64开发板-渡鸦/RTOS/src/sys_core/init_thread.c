#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "spi_flash_w25qxx.h"
#include "chlib_k.h"
#include "sram.h"
#include "drv_spi.h"

void led_thread_entry(void* parameter);
void usb_thread_entry(void* parameter);
void sd_thread_entry(void* parameter);
int ui_startup(int argc, char** argv);
int network_startup(int argc, char** argv);
void key_thread_entry(void* parameter);


rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);


void init_thread_entry(void* parameter)
{
    rt_thread_t tid;
    
    rt_system_heap_init((void*)(0x1FFF0000), (void*)(0x1FFF0000 + 0x10000));
    
    SRAM_Init();
    rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));

    touch_ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    
    if(dfs_mount("sf0", "/SF", "elm", 0, 0))
    {
        dfs_mkfs("elm", "sf0");
    }
    dfs_mount("sd0", "/SD", "elm", 0, 0);

    at24cxx_init("at24c02", "i2c0");

    //tid = rt_thread_create("led", led_thread_entry, RT_NULL, 256, 0x24, 20);
    //if (tid != RT_NULL) rt_thread_startup(tid);
  
    tid = rt_thread_create("key", key_thread_entry, RT_NULL, 512, 0x14, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
    
    ui_startup(RT_NULL, RT_NULL);
    network_startup(RT_NULL, RT_NULL);

    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

