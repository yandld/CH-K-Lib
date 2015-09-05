#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "sd.h"

uint8_t sd_buffer[512];
static void UART0_ISR(uint16_t byteReceived)
{
    UART_WriteByte(HW_UART4, byteReceived);
}

static void UART4_ISR(uint16_t byteReceived)
{
    UART_WriteByte(HW_UART0, byteReceived);
}


typedef enum
{
    kUART_SEL_RS485, 
    kUART_SEL_ESP8266,
    kUART_SEL_IMU,
    kUART_SEL_GPS,
}Sel_t;

void SelectUART(Sel_t sel)
{
    /* UART SEL */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_OPP);
    
    switch(sel)
    {
        case kUART_SEL_RS485:
            PEout(26) = 0;
            PEout(27) = 0;
            break;
        case kUART_SEL_IMU:
            PEout(26) = 1;
            PEout(27) = 0;
            break;
        case kUART_SEL_GPS:
            PEout(26) = 0;
            PEout(27) = 1;
            break;
        case kUART_SEL_ESP8266:
            PEout(26) = 1;
            PEout(27) = 1;
            break;
    }
}

 


int main(void)
{
    int i;
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 9600);
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 9600);

    printf("Test\r\n");
    
    GPIO_QuickInit(HW_GPIOE, 9, kGPIO_Mode_OPP); /* MODE */
    PEout(9) = 1;
    
    GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP); /* RST */
    PEout(8) = 0;
    DelayMs(10);
    PEout(8) = 1;
    
    SelectUART(kUART_SEL_ESP8266);

    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);

    UART_CallbackRxInstall(HW_UART0, UART0_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
//    DelayMs(1000);
//    
//    UART_WriteByte(4, 'A');
//    UART_WriteByte(4, 'T');
//    UART_WriteByte(4, '+');
//    UART_WriteByte(4, 'R');
//    UART_WriteByte(4, 'S');
//    UART_WriteByte(4, 'T');
//    UART_WriteByte(4, '\n');
//    UART_WriteByte(4, '\r');
    
    while(1)
    {
       // GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


