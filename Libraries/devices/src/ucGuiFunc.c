#include "ili9320.h"
#include "ads7843.h"


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

