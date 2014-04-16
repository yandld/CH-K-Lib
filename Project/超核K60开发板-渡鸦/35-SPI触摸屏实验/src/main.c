#include "gpio.h"
#include "uart.h"
#include "ads7843.h"
#include "ili9320.h"
#include "spi.h"

#include "gui.h"

static struct spi_bus bus;
extern int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance);

/* 重定义 GUI接口函数 */
int GUI_TOUCH_X_MeasureX(void)
{
    int x;
    ads7843_readX(&x);
    return x;
}

int GUI_TOUCH_X_MeasureY(void)
{
    int y;
    ads7843_readY(&y);
    return y;
}

int LCD_L0_Init(void)
{
    ili9320_init();
    return 0;
}

void LCD_L0_SetPixelIndex(int x, int y, int PixelIndex)
{
    ili9320_write_pixel(x, y, PixelIndex);
}

unsigned int LCD_L0_GetPixelIndex(int x, int y)
{
    return ili9320_read_pixel(x, y); 
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("w25qxx test\r\n");
    /* 初始化SPI2 */
    kinetis_spi_bus_init(&bus, HW_SPI2);
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); /* SPI2_SCK */
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); /* SPI2_SOUT */
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); /* SPI2_SIN */ 

    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); /* SPI2_PCS0 */
    /* 初始化w25qxx 使用CS1片选 */
    ads7843_init(&bus, HW_SPI_CS0);
    
    GUI_Init();
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
    GUI_SetPenSize(3);
    GUI_DispString("Measurement of\nA/D converter values");
    while (1)
    {
        GUI_PID_STATE TouchState;
        int xPhys, yPhys;
        GUI_TOUCH_GetState(&TouchState);  /* Get the touch position in pixel */
        xPhys = GUI_TOUCH_GetxPhys();     /* Get the A/D mesurement result in x */
        yPhys = GUI_TOUCH_GetyPhys();     /* Get the A/D mesurement result in y */
        /* Display the measurement result */
        GUI_SetColor(GUI_BLUE);
        GUI_DispStringAt("Analog input:\n", 0, 20);
        GUI_GotoY(GUI_GetDispPosY() + 2);
        GUI_DispString("x:");
        GUI_DispDec(xPhys, 4);
        GUI_DispString(", y:");
        GUI_DispDec(yPhys, 4);
        /* Display the according position */
        GUI_SetColor(GUI_RED);
        GUI_GotoY(GUI_GetDispPosY() + 4);
        GUI_DispString("\nPosition:\n");
        GUI_GotoY(GUI_GetDispPosY() + 2);
        GUI_DispString("x:");
        GUI_DispDec(TouchState.x,4);
        GUI_DispString(", y:");
        GUI_DispDec(TouchState.y,4);
        GUI_DrawPoint(TouchState.x, TouchState.y);
        GUI_TOUCH_Exec();
    }
}


