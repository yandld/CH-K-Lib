#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "sram.h"
#include "ili9320.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：Flexbus驱动LCD
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：初始化液晶屏，不断的刷新屏幕，变换颜色
*/
static uint16_t ColorTable[] = 
{
    RED,      //红色
    BLUE,     //蓝色
    YELLOW,   //黄色
    GREEN,    //绿色
    CYAN,
    LGRAY,
    GRED,
};

int main(void)
{
    uint8_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    ili9320_init();   //液晶屏初始化
    printf("controller id:0x%X\r\n", ili9320_get_id()); //获取液晶屏的驱动IC信息
    
    while(1)
    {
        i++; i%= ARRAY_SIZE(ColorTable);
        ili9320_clear(ColorTable[i]);  //屏幕全色显示
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


