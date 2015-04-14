#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "spi_flash_w25qxx.h"
#include "chlib_k.h"
#include "sram.h"
#include "drv_spi.h"
#include "drv_uart.h"

void led_thread_entry(void* parameter);
void usb_thread_entry(void* parameter);
void sd_thread_entry(void* parameter);
int ui_startup(int argc, char** argv);
void rt_hw_ksz8041_init(void);
void key_thread_entry(void* parameter);


rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);


void init_thread_entry(void* parameter)
{
    rt_thread_t tid;
    
    #ifndef FRDM
    SRAM_Init();
    rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));
    #else
    rt_system_heap_init((void*)(0x1FFF0000), (void*)(0x1FFF0000 + 0x10000));
    #endif
    
    rt_thread_delay(1);
    
    rt_hw_uart_init();
    
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
    finsh_system_init();
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);

    tid = rt_thread_self();
    rt_thread_delete(tid); 
    
    
    touch_ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    
    if(dfs_mount("sf0", "/SF", "elm", 0, 0))
    {
        dfs_mkfs("elm", "sf0");
    }
    
    if(dfs_mount("sd0", "/SD", "elm", 0, 0))
    {
        rt_kprintf("sd mount on /SD failed\r\n");
    }

    at24cxx_init("at24c02", "i2c0");

    #ifndef FRDM
    ili9320_init();
  //  ui_startup(RT_NULL, RT_NULL);
  //  tid = rt_thread_create("key", key_thread_entry, RT_NULL, 512, 0x14, 20);
  //  if (tid != RT_NULL) rt_thread_startup(tid);
    #endif
    
    rt_hw_ksz8041_init();
    
    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */
    while (!usbd_configured ());          /* Wait for device to configure        */
    
    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

