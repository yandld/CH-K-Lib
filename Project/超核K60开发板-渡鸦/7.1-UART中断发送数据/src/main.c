#include "gpio.h"
#include "common.h"
#include "uart.h"


static const char UART_String1[] = "HelloWorld\r\n";

static void UART_TX_ISR(uint16_t * byteToSend)
{
    static const char *p = UART_String1;
    *byteToSend = *p++;
    if((p - UART_String1) == sizeof(UART_String1))
    {
        p = UART_String1;
        UART_ITDMAConfig(HW_UART0, kUART_IT_Tx, false);  
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    /** install module */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_InitStruct1.bitPerChar = kUART_8BitsPerChar;
    UART_InitStruct1.parityMode = kUART_ParityDisabled;
    UART_Init(&UART_InitStruct1);
    
    /** Initation pin for UART which you chosen*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /** print message before mode change*/
    printf("uart will be send on interrupt mode...\r\n");
    
    /** register callback function*/
    UART_CallbackTxInstall(HW_UART0, UART_TX_ISR);
    
    /** open TX interrupt */
    UART_ITDMAConfig(HW_UART0, kUART_IT_Tx, true);
    
    /**main loop*/
    while(1)
    {
        /** indicate program is running */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}

