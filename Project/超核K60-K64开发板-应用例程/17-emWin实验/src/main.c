#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "gui.h"
#include "pit.h"
#include "spi.h"
#include "ili9320.h"
#include "ads7843.h"


void PIT_ISR(void)
{
    GUI_TOUCH_Exec();
}

int main(void)
{
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* µ÷ÊÔ´®¿Ú */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
   
    PIT_QuickInit(HW_PIT_CH0, 1000*10);
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
    
    SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14, kSPI_CPOL0_CPHA0, 2*1000*1000);

    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); /* SPI2_PCS0 */
    ads7843_init(HW_SPI2, HW_SPI_CS0);
    
    TOUCH_MainTask();
    
    GUI_Init();
    GUI_CURSOR_Show();
    MainTask();
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


