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
#include "sram.h"


#include "usb.h"
#include "usb_msc.h"


#define RTT_IRAM_HEAP_START               (0x1FFF0000)
#define RTT_IRAM_HEAP_END                 (0x1FFF0000 + 0x10000)


//int rt_hw_spi_bus_init(uint32_t instance, const char *name);

//void rt_hw_board_init(void)
//{

//    rt_hw_beep_init("beep");
//    rt_hw_spi_bus_init(HW_SPI2, "spi2");

//    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); 
//    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); 
//    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); 
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
//}



