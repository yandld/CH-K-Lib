#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "rtt_ksz8041.h"
#include "spi_flash_w25qxx.h"
#include "chlib_k.h"
#include "sram.h"
#include "rtt_spi.h"

void led_thread_entry(void* parameter);
void usb_thread_entry(void* parameter);
void sd_thread_entry(void* parameter);
int cmd_gui_start(int argc, char** argv);

rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);

void init_thread_entry(void* parameter)
{
    rt_thread_t tid;
    
    SRAM_Init();
    rt_system_heap_init((void*)(0x1FFF0000), (void*)(0x1FFF0000 + 0x10000));
    rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));

    /* init finsh */
    rt_device_t dev = rt_device_find("uart0");
    finsh_system_init();

    rt_hw_spi_bus_init(HW_SPI2, "spi2");
    rt_hw_beep_init("beep");
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); 
    
    /* attacted spi2 - 1*/
    {
        static struct rt_spi_device spi_device;
        static struct kinetis_spi_cs spi_cs_0;
        spi_cs_0.ch = 1;
        PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
        rt_spi_bus_attach_device(&spi_device, "spi21", "spi2", &spi_cs_0);
    }
    
    /* attacted spi2 - 0*/
    {
        static struct rt_spi_device spi_device;
        static struct kinetis_spi_cs spi_cs_0;
        spi_cs_0.ch = 0;
        PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
        rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", &spi_cs_0);
    }
    
    touch_ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    dfs_mount("sf0", "/SF", "elm", 0, 0);
    dfs_mount("sd0", "/SD", "elm", 0, 0);

    at24cxx_init("at24c02", "i2c0");

    tid = rt_thread_create("led", led_thread_entry, RT_NULL, 256, 0x24, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
  
    cmd_gui_start(RT_NULL, RT_NULL);
    
#ifdef RT_USING_LWIP
//    rt_kprintf(" link beginning \r\n");
    rt_hw_ksz8041_init(BOARD_ENET_PHY_ADDR);
//    time_out = 0;
//	while(!(netif_list->flags & NETIF_FLAG_UP)) 
//	{
//		rt_thread_delay(RT_TICK_PER_SECOND);
//        if((time_out++) > 3)
//        {
//            rt_kprintf("link failed\r\n");
//            break;
//        }
//	}
//    list_if();
#endif

    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

