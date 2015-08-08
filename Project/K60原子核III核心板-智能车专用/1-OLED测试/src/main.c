#include "gpio.h"
#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "ssd1306.h"


int main(void)
{
    int i;
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
    UART_QuickInit(UART3_RX_PB10_TX_PB11, 115200);
    GPIO_WriteBit(HW_GPIOE, 0, 0);
    I2C_QuickInit(I2Cx_SCL_PC14_SDA_PC15,100*1000);
   // I2C_Scan(I2C1_SCL_PC10_SDA_PC11);
    ssd1306_init();
    OLED_ShowStr(5, 0, "www.beyondcore.net");
    for(i=0;i<4;i++)
    {
        OLED_ShowCN(22+i*16, 3, i);
    }
    DelayMs(5000);
    while(1)
    {
        OLED_Fill(0xFF);
        DelayMs(200);
        OLED_Fill(0x00);
        DelayMs(200);
        PEout(0) = !PEout(0);
        printf("!!!\r\n");
    }
}


