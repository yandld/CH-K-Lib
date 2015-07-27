#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "sram.h"
#include "ili9320.h"

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
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
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
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


