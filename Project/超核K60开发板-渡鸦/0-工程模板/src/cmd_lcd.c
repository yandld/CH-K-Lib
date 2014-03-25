
#include "shell.h"
#include "ili9320.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include "board.h"
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     
#include "chgui_touch.h"

//底层操作连接结构 目前的版本只需实现LCD_DrawPoint和 LCD_Init就可以工作，其他填NULL就可以
CHGUI_CtrlOperation_TypeDef lcd_ops = 
{
	ili9320_Init,        //底层函数与CHGUI无关 LCD_Init的实现在 LCD_CHK60EVB.c中 下同
	NULL,
	LCD_DrawPoint,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ILI9320_GetDeivceID,
};

    
static struct ads7843_device ads7843;
struct spi_bus bus; 
static void tp_init(void)
{
    spi_bus_init(&bus, BOARD_SPI_INSTANCE);
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    
    ads7843.bus = &bus;
    ads7843_init(&ads7843, BOARD_TP_SPI_PCSN, HW_CTAR0, 80*1000);
}

static uint32_t tp_get_x(void)
{
    uint16_t x = 0;
    uint32_t sum = 0;
    uint32_t i;
    for(i=0;i<7;i++)
    {
        ads7843.readX(&ads7843, &x);
        sum += x;
    }
    sum/=7;
    return sum;
}
static uint32_t tp_get_y(void)
{
    uint32_t sum = 0;
    uint16_t y = 0;
    uint32_t i;
    for(i=0;i<7;i++)
    {
        ads7843.readY(&ads7843, &y);
        sum += y;
    }
    sum/=7;
    return sum;
}
    
//触摸屏操作连接器
CHGUI_TouchCtrlOperation_TypeDef tp_ops = 
{
    tp_init,
    tp_get_x,
    tp_get_y,
};
//CHGUI初始化结构
CHGUI_InitTypeDef CHGUI_InitStruct1 = 
{
    "TFT_9320",
    0,
    LCD_X_MAX,
    LCD_Y_MAX,
    &lcd_ops,
    &tp_ops,
};

int CMD_LCD(int argc, char * const * argv)
{
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED\r\n");
    return 0;
#else
    CHGUI_PID_TypeDef State;
    ili9320_Init();
    shell_printf("LCD ID:0x%X\r\n", ILI9320_GetDeivceID());
    //初始化GUI
    GUI_Init(&CHGUI_InitStruct1);
    GUI_Clear(BLACK);
    GUI_SetBkColor(BLACK);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetColor(LGRAY);
    GUI_SetFontFormName("FONT_CourierNew");
    GUI_printf("HelloWorld\r\n");
    GUI_printf("CHGUI_Version:%0.2f\r\n", (float)(GUI_VERSION/100));
    GUI_printf("ID:%X\r\n", GUI_GetDeivceID());
    while(1)
    {
        GUI_GotoXY(0, 0);
        GUI_TOUCH_GetState(&State);
        //打印物理AD坐标
        GUI_printf("Phy:X:%04d Y:%04d\r\n", GUI_TOUCH_GetxPhys(), GUI_TOUCH_GetyPhys());
        //打印逻辑AD坐标
        GUI_printf("Log:X:%04d Y:%04d\r\n", State.x, State.y);	
        GUI_printf("State:%01d\r\n", State.Pressed);
        //LCD 画笔跟踪
        GUI_DrawPoint(State.x, State.y);	
        GUI_TOUCH_Exec();
    }
    return 0;
#endif
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
