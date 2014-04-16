#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "sram.h"
#include "ili9320.h"

#include "gui.h"

int main(void)
{
    uint8_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    GUI_Init();
    GUI_DispString("ucGUI");
    GUI_DispString(GUI_GetVersionString());
    while(1);

}


