#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "sd.h"


typedef enum
{
    kUART_SEL_RS485, 
    kUART_SEL_ESP8266,
    kUART_SEL_IMU,
    kUART_SEL_GPS,
}Sel_t;

static void UART4_ISR(uint16_t byteReceived)
{
    printf("%c", byteReceived & 0xFF);
}



void SelectUART(Sel_t sel)
{
    /* UART SEL */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_OPP);
    
    switch(sel)
    {
        case kUART_SEL_RS485:
            PEout(26) = 0; PEout(27) = 0;
            break;
        case kUART_SEL_IMU:
            PEout(26) = 1; PEout(27) = 0;
            break;
        case kUART_SEL_GPS:
            PEout(26) = 0; PEout(27) = 1;
            break;
        case kUART_SEL_ESP8266:
            PEout(26) = 1; PEout(27) = 1;
            break;
    }
}

int main(void)
{
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 9600);

    printf("GPS demo\r\n");
    
    SelectUART(kUART_SEL_GPS);

    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);

    while(1)
    {
        DelayMs(500);
    }
}


