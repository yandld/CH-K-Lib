#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "scope.h"
#include "i2c.h"
#include "CHZT02.h"

/* i2c bus scan */
static void I2C_Scan(uint32_t instance)
{
    uint8_t i;
    uint8_t ret;
    for(i = 1; i < 127; i++)
    {
        ret = I2C_BurstWrite(instance , i, 0, 0, NULL, 0);
        if(!ret)
        {
            printf("ADDR:0x%2X(7BIT) | 0x%2X(8BIT) found!\r\n", i, i<<1);
        }
    }
}


int main(void)
{
	uint32_t instance, clock;
	int16_t i = 0;

    DelayInit();    
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);    
    printf("HelloWorld\r\n");
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("kBusClock:%dHz\r\n", clock);

	CHZT02_Init();

//	scopeInit();

    while(1)
    {
//		scopeDrawLine(i++,0,0);
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(50);
    }
}


