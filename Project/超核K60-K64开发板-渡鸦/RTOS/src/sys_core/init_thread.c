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
void usb_thread_entry(void* parameter);
int ui_startup(int argc, char** argv);
int network_startup(int argc, char** argv);
void key_thread_entry(void* parameter);


rt_err_t touch_ads7843_init(const char * name, const char * spi_device_name);

static void spi_init(void)
{
    rt_hw_spi_bus_init(HW_SPI2, "spi2");
    
    struct rt_spi_device* spi_21;
    struct kinetis_spi_cs* cs_21;
    spi_21 = rt_malloc(sizeof(struct rt_spi_device));
    cs_21 = rt_malloc(sizeof(struct kinetis_spi_cs));
    
    cs_21->ch = 1;
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    rt_spi_bus_attach_device(spi_21, "spi21", "spi2", cs_21);
    
    struct rt_spi_device* spi_20;
    struct kinetis_spi_cs* cs_20;
    spi_20 = rt_malloc(sizeof(struct rt_spi_device));
    cs_20 = rt_malloc(sizeof(struct kinetis_spi_cs));
    cs_20->ch = 0;
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    rt_spi_bus_attach_device(spi_20, "spi20", "spi2", cs_20);
}

void init_thread_entry(void* parameter)
{
    rt_thread_t tid;
    
    rt_system_heap_init((void*)(0x1FFF0000), (void*)(0x1FFF0000 + 0x10000));
    
    SRAM_Init();
    rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));

    rt_hw_beep_init("beep");
    spi_init();
    
    touch_ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    
    if(dfs_mount("sf0", "/SF", "elm", 0, 0))
    {
        dfs_mkfs("elm", "sf0");
    }
    dfs_mount("sd0", "/SD", "elm", 0, 0);

    at24cxx_init("at24c02", "i2c0");

    tid = rt_thread_create("led", led_thread_entry, RT_NULL, 256, 0x24, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
  
    tid = rt_thread_create("key", key_thread_entry, RT_NULL, 512, 0x14, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
    
    ui_startup(RT_NULL, RT_NULL);
    network_startup(RT_NULL, RT_NULL);

    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

