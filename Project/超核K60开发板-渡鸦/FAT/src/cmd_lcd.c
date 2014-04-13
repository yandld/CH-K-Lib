
#include "shell.h"
#include "ili9320.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include "board.h"       

//D4D_EXTERN_SCREEN(screen_entry);
#include "d4d.h"
#include "fonts.h"

static uint16_t buf[100]; 
//D4D_EXTERN_SCREEN(screen_template);
D4D_POINT point;
    D4D_STRING  string1;
    D4D_STR_PROPERTIES str_prop;
int CMD_LCD(int argc, char * const * argv)
{
    uint32_t i;
    ili9320_init();
    memset(buf, BLUE, sizeof(buf));
  //  ili9320_set_window(0,0,10,10);
 //   ili9320_write_gram(buf, ARRAY_SIZE(buf));
    ili9320_hline(0,60,50,RED);
    ili9320_vline(100, 150, 90, GREEN);
    D4D_Init(NULL);
   // D4D_SetOrientation(D4D_ORIENT_LANDSCAPE);    
   // D4D_FillRectXY(0, 10, 20, 20, BLUE);
    
    point.x = 30;
    point.y = 100;
    D4D_FillCircle(&point, 10, RED);

    
    str_prop.font_properties.bits.bStrikeThrough = D4D_FNT_PRTY_STRIKETHROUGH_NONE;
    str_prop.font_properties.bits.bTransparent = D4D_FNT_PRTY_TRANSPARENT_YES;
    str_prop.font_properties.bits.bUnderLine = D4D_FNT_PRTY_UNDERLINE_LINE;
    str_prop.text_properties.bits.bAlignHoriz = D4D_TXT_PRTY_ALIGN_H_LEFT;
    str_prop.text_properties.bits.bAlignVertic = D4D_TXT_PRTY_ALIGN_V_TOP;
    
    string1.pText = "HelloWorldasddsa";
    string1.printLen = 10;
    string1.fontId = FONT_BERLIN_SANS_FBDEMI12;
    string1.buffSize = 30;
    string1.printOff = 0;
    string1.str_properties = &str_prop;
    D4D_DrawText(&point, &string1, RED, WHITE);

    while(1)
    {
        D4D_Poll();
    }
}

const cmd_tbl_t CommandFun_LCD = 
{
    .name = "LCD",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_LCD,
    .usage = "LCD",
    .complete = NULL,
    .help = "\r\n"
};
