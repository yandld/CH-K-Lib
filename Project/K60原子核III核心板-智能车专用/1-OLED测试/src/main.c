#include "gpio.h"
#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "ssd1306.h"


int main(void)
{
    int i;
    DelayInit();
    DelayMs(10);
    
    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("Hello K60!\r\n");
    
    GPIO_WriteBit(HW_GPIOE, 0, 0);
    I2C_QuickInit(I2Cx_SCL_PC14_SDA_PC15,100*1000);

    ssd1306_init();

    while(1)
    {
        OLED_ShowStr(5, 0, "www.beyondcore.net");
        for(i=0;i<4;i++)
        {
            OLED_ShowCN(22+i*16, 3, i);
        }
        DelayMs(2000);
        OLED_Fill(0xFF);
        DelayMs(50);
        OLED_Fill(0x00);
        DelayMs(50);
        PEout(0) = !PEout(0);
    }
}


