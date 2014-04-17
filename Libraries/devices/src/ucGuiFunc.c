#include "ili9320.h"
#include "ads7843.h"
#include "common.h"

/* 重定义 GUI接口函数 */
int GUI_TOUCH_X_MeasureX(void)
{
    int ret;
    int x;
    static int x_last;
    ads7843_readX(&x);
    ret = x;
    if(ABS(x - x_last) > 5)
    {
        /* discard this result */
        ret = x_last;
    }
    x_last = x;
    return ret;
}

int GUI_TOUCH_X_MeasureY(void)
{
    int ret;
    int x;
    static int x_last;
    ads7843_readY(&x);
    ret = x;
    if(ABS(x - x_last) > 5)
    {
        /* discard this result */
        ret = x_last;
    }
    x_last = x;
    return ret;
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

