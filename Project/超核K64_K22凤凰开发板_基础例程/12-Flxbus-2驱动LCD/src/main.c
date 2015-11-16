#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "IS61WV25616.h"
#include "ili9320.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：Flexbus驱动LCD
     实验平台：凤凰开发板
     板载芯片：MK64FN1MVLQ12
     实验效果：使用Flxbus总线驱动3.2寸液晶屏，液晶屏的颜色周期性变化
*/
static uint16_t ColorTable[] = 
{
    RED,
    BLUE,
    YELLOW,
    GREEN,
    CYAN,
    LGRAY,
    GRED,
};

int main(void)
{
    uint8_t i;
    int x,y;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    ili9320_init();
    printf("controller id:0x%X\r\n", ili9320_get_id());
    ili9320_get_lcd_size(&x, &y);
    printf("size: %dx%d\r\n", x, y);
    
    while(1)
    {
        i++; i%= ARRAY_SIZE(ColorTable);
        ili9320_clear(ColorTable[i]);
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


