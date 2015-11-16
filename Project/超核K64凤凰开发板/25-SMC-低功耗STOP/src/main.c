#include "chlib_k.h"

static void GPIO_ISR(uint32_t array)
{
    printf("enter interrupt, back to RUN now!\r\n");
}

int main(void)
{
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_CallbackInstall(HW_GPIOE, GPIO_ISR);
    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_FallingEdge, true);
    
    printf("low power STOP mode\r\n");
    
    while(1)
    {
        EnterSTOPMode(false);
        printf("Wake Up!\r\n");
    }
}
