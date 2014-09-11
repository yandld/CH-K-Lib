#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "common.h"
#include "board.h"
#include <rtthread.h>
#include <rthw.h>
#include "finsh.h"
#include "shell.h"
#include "sram.h"

#include <drivers/spi.h>
#include "rtt_spi.h"

#include "components.h"
#include "spi.h"


#define RTT_IRAM_HEAP_START               (0x1FFF0000)
#define RTT_IRAM_HEAP_END                 (0x1FFF0000 + 0x10000)


int rt_hw_usart_init(uint32_t instance, const char * name);
int rt_hw_spi_bus_init(uint32_t instance, const char *name);
int rt_hw_rtc_init(const char* name);
void rt_hw_sd_init(uint32_t instance, const char *name);
extern void init_thread_entry(void* parameter);

void rt_hw_board_init(void)
{
    rt_hw_usart_init(HW_UART0, "uart0");
	rt_console_set_device("uart0");
    rt_hw_sd_init(0, "sd0");
    rt_hw_rtc_init("rtc");  

    rt_hw_spi_bus_init(HW_SPI2, "spi2");
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); 
//    
//    /* attacted spi2 - 1*/
//    {
//        static struct rt_spi_device spi_device;
//        static struct kinetis_spi_cs spi_cs_0;
//        spi_cs_0.ch = 1;
//        PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
//        rt_spi_bus_attach_device(&spi_device, "spi21", "spi2", &spi_cs_0);
//    }
//    
//    /* attacted spi2 - 0*/
//    {
//        static struct rt_spi_device spi_device;
//        static struct kinetis_spi_cs spi_cs_0;
//        spi_cs_0.ch = 0;
//        PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
//        rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", &spi_cs_0);
//    }

    SYSTICK_Init(1000*1000/RT_TICK_PER_SECOND);
    SYSTICK_ITConfig(ENABLE);
    SYSTICK_Cmd(ENABLE);
}

void rt_application_init(void)
{
    rt_thread_t init_thread;
    /* init thread */
    init_thread = rt_thread_create("init", init_thread_entry, RT_NULL, 2048, 0x20, 20);                       
    if (init_thread != RT_NULL)
    {
        rt_thread_startup(init_thread);		
    }
}

void rtthread_startup(void)
{
    /* BM init */
    DelayInit();
    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("IRAM: BEIGN:0x%08XU END:0x%08XU SIZE:%dKB\r\n", (uint32_t)RTT_IRAM_HEAP_START, (uint32_t)RTT_IRAM_HEAP_END, ((uint32_t)RTT_IRAM_HEAP_END - (uint32_t)RTT_IRAM_HEAP_START)/1024);
    rt_system_heap_init((void*)RTT_IRAM_HEAP_START, (void*)RTT_IRAM_HEAP_END);
    /*
    SRAM_Init();
    if(SRAM_SelfTest())
    {
        printf("SRAM SelfTest failed will use IRAM\r\n");
        rt_system_heap_init((void*)RTT_IRAM_HEAP_START, (void*)RTT_IRAM_HEAP_END);
    }

        rt_system_heap_init((void*)SRAM_ADDRESS_BASE, (void*)(SRAM_SIZE + SRAM_ADDRESS_BASE));
    }
    */

    rt_hw_board_init();
	rt_show_version(); /* print logo */
	rt_system_timer_init(); /* init timer */
    
    
	rt_system_scheduler_init();

	rt_device_init_all();
    rt_system_timer_thread_init();
	rt_thread_idle_init();
    rt_application_init();
	rt_system_scheduler_start();
}


