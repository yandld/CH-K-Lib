#include "chgui.h"
#include "chgui_char.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

//#include "TimeNewRoman6x12.h"
//#include "CourierNew24x48.h"
#include "CourierNew8x16.h"
//#include "CourierNew8x16B.h"
#include "simsun6x12.h"

static CHGUI_FontSetup_TypeDef CHGUI_FontTable[] = 
{
	  {"SimSun"              ,    FONT_SimSun6x12_XSize,         FONT_SimSun6x12_YSize,         FONT_SimSun6x8 },
	//  {"TimesNewRoman"       ,    FONT_TimesNewRoman6x12_XSize,  FONT_TimesNewRoman6x12_YSize,  FONT_TimesNewRoman6x8 },
	//  {"FONT_CourierNew"     ,    FONT_CourierNew24x48_XSize,    FONT_CourierNew24x48_YSize,    FONT_CourierNew24x48 },
	  {"FONT_CourierNew"    ,    FONT_CourierNew8x16_XSize,     FONT_CourierNew8x16_YSize,     FONT_CourierNew8x16 },
	//  {"FONT_CourierNewB"    ,    FONT_CourierNew8x16B_XSize,    FONT_CourierNew8x16B_YSize,    FONT_CourierNew8x16B },
};

static uint8_t gGUI_TextMode;
static uint8_t gGUI_FontIndex = 0;
static uint8_t gGUI_NumOfFonts = sizeof(CHGUI_FontTable)/sizeof(CHGUI_FontSetup_TypeDef);
static CHGUI_Point_TypeDef gGUI_CurrentFontPointPos = {0,0};

uint8_t GUI_GetNumOfFonts(void)
{
    return gGUI_NumOfFonts;
}

char* GUI_GetFontNameFormIndex(uint8_t FontIndex)
{
    if(FontIndex <= gGUI_NumOfFonts)
    {
        return CHGUI_FontTable[FontIndex].Name;
    }
    return NULL;
}

char* GUI_GetCurrentFontName(void)
{
    return CHGUI_FontTable[gGUI_FontIndex].Name;
}

uint8_t GUI_GetFont(void)
{
    return gGUI_FontIndex;
}

void GUI_SetFont(uint8_t FontIndex)
{
    gGUI_FontIndex = FontIndex;
}


void GUI_SetFontFormName(const char *pch)
{
    uint8_t i;
    for(i = 0; i < gGUI_NumOfFonts; i++)
    {
        if(!strcmp(pch, CHGUI_FontTable[i].Name))
        {
            gGUI_FontIndex = i;
        }
    }
}

uint32_t GetCharDistX(uint8_t FontIndex)
{
    return CHGUI_FontTable[FontIndex].FontXSize;
}

uint32_t GetCharDistY(uint8_t FontIndex)
{
    return CHGUI_FontTable[FontIndex].FontYSize;
}


void GUI_SetTextMode(uint8_t TextMode)
{
    gGUI_TextMode = TextMode;
}

uint8_t GUI_GetTextMode(void)
{
    return gGUI_TextMode;
}

static void _GUI_DispChar(uint16_t x, uint16_t y, uint8_t c)
{
    uint8_t j,pos,t;
    uint8_t temp;
    uint8_t XNum;
    uint32_t base;
    XNum = (CHGUI_FontTable[gGUI_FontIndex].FontXSize / 8) + 1;
    if(CHGUI_FontTable[gGUI_FontIndex].FontXSize % 8 == 0)
    {
        XNum--;
    }
    c = c - ' ';
    base = (c * XNum *  CHGUI_FontTable[gGUI_FontIndex].FontYSize);

    for(j=0;j<XNum;j++)
    {
        for(pos = 0; pos < CHGUI_FontTable[gGUI_FontIndex].FontYSize; pos++)
        {
            temp = (uint8_t)CHGUI_FontTable[gGUI_FontIndex].pFontDataAdress[base + pos +  j * CHGUI_FontTable[gGUI_FontIndex].FontYSize];
            if(j < XNum)
            {
                for(t = 0; t < 8; t++)
                {
                    if((temp>>t)&0x01)
                    {
                        gpCHGUI->ops->ctrl_point(x + t, y + pos, gGUI_ForntColor);
                    }
                    else
                    {
                        switch(gGUI_TextMode)
                        {
                            case GUI_TEXTMODE_NORMAL:
                                gpCHGUI->ops->ctrl_point(x + t, y + pos, gGUI_BackColor);
                                break;
                            case GUI_TEXTMODE_TRANSPARENT:
            
                                break;
                            default:break;
                        }
                    }
                }
            }
            else
            {
                for(t = 0; t < CHGUI_FontTable[gGUI_FontIndex].FontXSize%8; t++)
                {
                    if((temp>>t)&0x01)
                    {
                        gpCHGUI->ops->ctrl_point(x + t, y + pos, gGUI_ForntColor);
                    }
                    else
                    {
                        gpCHGUI->ops->ctrl_point(x + t, y + pos, gGUI_BackColor);
                    }
                }
    
            }
        }
    x += 8;
    }
}

void GUI_DispChar(uint8_t c)
{
    //Check Pos
    if((gGUI_CurrentFontPointPos.x + CHGUI_FontTable[gGUI_FontIndex].FontXSize) > gpCHGUI->x_max)
    {
        gGUI_CurrentFontPointPos.x = 0;
        gGUI_CurrentFontPointPos.y += CHGUI_FontTable[gGUI_FontIndex].FontYSize;
        if(gGUI_CurrentFontPointPos.y + CHGUI_FontTable[gGUI_FontIndex].FontYSize > gpCHGUI->y_max)
        {
            gGUI_CurrentFontPointPos.y = 0;
        }
    }
    //special char handing
    switch(c)
    {
        case '\n':
            break;
        case '\r':
            gGUI_CurrentFontPointPos.y += CHGUI_FontTable[gGUI_FontIndex].FontYSize;
            gGUI_CurrentFontPointPos.x = 0;
            break;
        case 0x7F: //backspace
            break;
        case '\0': //end
            break;
        default: //normal char
            _GUI_DispChar(gGUI_CurrentFontPointPos.x, gGUI_CurrentFontPointPos.y, c);
            gGUI_CurrentFontPointPos.x += CHGUI_FontTable[gGUI_FontIndex].FontXSize;
            break;
    }
}


void GUI_GotoXY(uint16_t x, uint16_t y)
{
    gGUI_CurrentFontPointPos.x = x;
    gGUI_CurrentFontPointPos.y = y;
}

void GUI_DispCharAt(uint16_t x, uint16_t y, uint8_t c)
{
    gGUI_CurrentFontPointPos.x = x;
    gGUI_CurrentFontPointPos.y = y;
    GUI_DispChar(c);
}



void GUI_DispString(uint8_t* pch)
{
    while(*pch != '\0')
    {
        GUI_DispChar(*pch);
        pch++;
    }
}

void GUI_DispStringAt(uint16_t x, uint16_t y, uint8_t* pch)
{
    gGUI_CurrentFontPointPos.x = x;
    gGUI_CurrentFontPointPos.y = y;
    while(*pch != '\0')
    {
        GUI_DispChar(*pch);
        pch++;
    }
}

int GUI_printf(const char *format,...)
{
    int chars;
    int i;
    va_list ap;
    char printbuffer[GUI_PRINTF_MAX_CHAR_LEN];
    va_start(ap, format);
    chars = vsprintf(printbuffer, format, ap);
    va_end(ap);
    for(i=0;i<chars;i++)
    {
        GUI_DispChar(printbuffer[i]);
    }
    return chars ;
}

uint32_t GUI_GetDeivceID(void)
{
    return gpCHGUI->device_id;
}
