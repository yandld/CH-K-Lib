#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "sd.h"

#define SSID        "Yandld"
#define PASSWD      "XXXXXXXX"

typedef enum
{
    kUART_SEL_RS485, 
    kUART_SEL_ESP8266,
    kUART_SEL_IMU,
    kUART_SEL_GPS,
}Sel_t;

static void UART0_ISR(uint16_t byteReceived)
{
    UART_WriteByte(HW_UART4, byteReceived);
}

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
    int i;
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200);

    SelectUART(kUART_SEL_ESP8266);
    
    GPIO_QuickInit(HW_GPIOE, 9, kGPIO_Mode_OPP); /* MODE */
    PEout(9) = 1;
    
    GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP); /* RST */
    PEout(8) = 0;
    DelayMs(1);
    PEout(8) = 1;
    DelayMs(500);
    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);
    
    UART_printf(HW_UART4, "AT+CWAUTOCONN=0\r\n");
    DelayMs(50);
    
    UART_printf(HW_UART4, "AT+CWMODE=3\r\n");
    DelayMs(50);
    
    UART_printf(HW_UART4, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
    DelayMs(8000);
    UART_printf(HW_UART4, "AT+CIFSR\r\n");
   
    
    while(1)
    {
       // GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


