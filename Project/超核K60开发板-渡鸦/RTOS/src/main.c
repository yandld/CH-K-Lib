#include <rtthread.h>
#include <rthw.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include <drivers/spi.h>
#include "rtt_spi.h"

#define RTT_IRAM_HEAP_START               (0x1FFF0000)
#define RTT_IRAM_HEAP_END                 (0x1FFF0000 + 0x10000)


int rt_hw_usart_init(uint32_t instance, const char * name);
int rt_hw_beep_init(const char *name);
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
    rt_hw_beep_init("beep");
    rt_hw_spi_bus_init(HW_SPI2, "spi2");
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
    rt_hw_board_init();
	rt_show_version();
	rt_system_timer_init();
    
    rt_system_scheduler_init();

    rt_device_init_all();
    rt_system_timer_thread_init();
    rt_thread_idle_init();
    rt_application_init();
    rt_system_scheduler_start();
}

/* baremetel init */
void bm_init(void)
{
    DelayInit();
    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
   // printf("IRAM: BEIGN:0x%08XU END:0x%08XU SIZE:%dKB\r\n", (uint32_t)RTT_IRAM_HEAP_START, (uint32_t)RTT_IRAM_HEAP_END, ((uint32_t)RTT_IRAM_HEAP_END - (uint32_t)RTT_IRAM_HEAP_START)/1024);
    printf("loading RAM to OS...\r\n");
    rt_system_heap_init((void*)RTT_IRAM_HEAP_START, (void*)RTT_IRAM_HEAP_END);
    
    /* list RAM use */
    rt_uint32_t total, used, max_used;
    rt_memory_info(&total, &used, &max_used);
    printf("total:%d used:%d max_used:%d\r\n", total, used, max_used);
    
    /* init systick */
    SYSTICK_Init(1000*(1000/RT_TICK_PER_SECOND));
    SYSTICK_ITConfig(ENABLE);
    SYSTICK_Cmd(ENABLE);
}

void SysTick_Handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

void DelayMs(uint32_t ms)
{
    if(ms < (1000/RT_TICK_PER_SECOND)) rt_thread_delay(1);
    else rt_thread_delay(ms/(1000/RT_TICK_PER_SECOND));  
}

int main(void)
{
	rt_hw_interrupt_disable();
    bm_init();
	rtthread_startup();
	return 0;
}


