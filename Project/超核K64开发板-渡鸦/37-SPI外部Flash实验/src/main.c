#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "w25qxx.h"
#include "spi.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */
 
/*
     实验名称：GPIO小灯实验
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果: 获取板子上面SPI-Flash的信息通过串口发送出去
      小灯周期性闪烁，闪烁时间间隔500ms     
*/
static struct spi_bus bus;
extern int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance);

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("w25qxx test\r\n");
    /* 初始化SPI2接口 */
    kinetis_spi_bus_init(&bus, HW_SPI2);
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); /* SPI2_SCK */
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); /* SPI2_SOUT */
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); /* SPI2_SIN */ 

    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); /* SPI2_PCS1 */
    /* 初始化w25qxx 使用CS1片选 */
    w25qxx_init(&bus, HW_SPI_CS1);
    //获取SPI-Flash的信息
    if(w25qxx_probe())
    {
        printf("w25qxx device no found!\r\n");
    }
    else
    {
        printf("%s detected! size:%dKB\r\n", w25qxx_get_name(), w25qxx_get_size()/1024);
    }
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);  //小灯闪烁
    }
}


