#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "gui.h"
#include "pit.h"
#include "spi.h"
#include "ili9320.h"
#include "ads7843.h"

static int Stat1;
static int Stat2;

static void GPIO_ISR(uint32_t array)
{
    if(array & (1<<26))
    {
        DelayMs(3);
        if(GPIO_ReadBit(HW_GPIOE, 26) == 0)
        {
            Stat1 ^= 1;
            if(Stat1 & 1)
            {
                printf("1 ON\r\n");
            }
            else
            {
                printf("1 OFF\r\n");
            }
        }
    }
    if(array & (1<<27))
    {
        DelayMs(3);
        if(GPIO_ReadBit(HW_GPIOE, 27) == 0)
        {
            Stat2 ^= 1;
            if(Stat2 & 1)
            {
                printf("2 ON\r\n");
            }
            else
            {
                printf("2 OFF\r\n");
            }
        }
    }
    PORTE->ISFR = 0xFFFFFFFF;
}

int main(void)
{
    int i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
   
    printf("PS!\r\n");
    
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_IPU);
    PORTE->PCR[26] |= PORT_PCR_PFE_MASK;
    PORTE->PCR[27] |= PORT_PCR_PFE_MASK;
    
    GPIO_CallbackInstall(HW_GPIOE, GPIO_ISR);

    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_RisingEdge, true);
    GPIO_ITDMAConfig(HW_GPIOE, 27, kGPIO_IT_RisingEdge, true);
    
//    PIT_QuickInit(HW_PIT_CH0, 1000*10);
//    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
//    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
//    
//    SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14, kSPI_CPOL0_CPHA0, 2*1000*1000);

//    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); /* SPI2_PCS0 */
//    ads7843_init(HW_SPI2, HW_SPI_CS0);
    
    GUI_Init();
    GUI_SetColor(GUI_RED);
    GUI_SetFont(GUI_FONT_32B_1);
    
    while(1)
    {
        GPIO_ReadBit(HW_GPIOE, 26);
        
        if(Stat1 & 0x01)
        {
            GUI_DispStringAt("Stat1 ON  ", 10, 10);
        }
        else
        {
            GUI_DispStringAt("Stat1 OFF  ", 10, 10);
        }
        
        if(Stat2 & 0x01)
        {
            GUI_DispStringAt("Stat2 ON  ", 10, 50);
        }
        else
        {
            GUI_DispStringAt("Stat2 OFF  ", 10, 50);
        }
        
        GUI_DispStringAt("Value: ", 0, 100);
        GUI_DispDecAt(i++, 100, 100, 6);
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


