#include "chgui.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "SimSun6x12.h"

static chgui_font_t _gFontTbl[] = 
{
    {"SimSun", FONT_SimSun6x12_XSize, FONT_SimSun6x12_YSize, FONT_SimSun6x8 },
};

__weak void GUI_DrawPixel(int color, int x, int y)
{
    /* defatult function shell */
}

void CHGUI_Init(void)
{
    /* do initalizeion here */ 
}


void _GUI_DispChar(char c, int x, int y, const char *pdata, int font_xsize, int font_ysize, int fcolor, int bcolor)
{
    uint8_t j,pos,t;
    uint8_t temp;
    uint8_t XNum;
    uint32_t base;
    XNum = (font_xsize/8) + 1;
    if(font_ysize%8 == 0)
    {
        XNum--;
    }
    if(c < ' ')
    {
        return;
    }
    c = c - ' ';
    base = (c*XNum*font_ysize);

    for(j=0;j<XNum;j++)
    {
        for(pos = 0; pos < font_ysize; pos++)
        {
            temp = (uint8_t)pdata[base+pos+j*font_ysize];
            if(j < XNum)
            {
                for(t = 0; t < 8; t++)
                {
                    if((temp>>t)&0x01)
                    {
                        GUI_DrawPixel(fcolor, x+t, y+pos);
                    }
                    else
                    {
                        GUI_DrawPixel(bcolor, x+t, y+pos);
                    }
                }
            }
            else
            {
                for(t = 0; t < font_xsize%8; t++)
                {
                    if((temp>>t)&0x01)
                    {
                        GUI_DrawPixel(fcolor, x+t, y+pos);
                    }
                    else
                    {
                        GUI_DrawPixel(bcolor, x+t, y+pos);
                    }
                }
            }
        }
    x += 8;
    }
}

void GUI_DispChar(char c, int x, int y)
{
    _GUI_DispChar(c, x, y, _gFontTbl[0].data, _gFontTbl[0].x_size, _gFontTbl[0].y_size, 0xFFFF, 0x0000);
}

int GUI_printf(int x, int y, const char *format,...)
{
    int chars;
    int i;
    va_list ap;
    char printbuffer[64];
    va_start(ap, format);
    chars = vsprintf(printbuffer, format, ap);
    va_end(ap);
    for(i = 0; i < chars; i++)
    {
        GUI_DispChar(printbuffer[i],x + i*_gFontTbl[0].x_size ,y);
    }
    return chars ;
}

