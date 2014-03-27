#include "common.h"

//FlexBus总线定义
#define LCD_BASE                    (0x70000000)
#define LCD_COMMAND_ADDRESS         *(unsigned short *)0x70000000
#define LCD_DATA_ADDRESS            *(unsigned short *)0x78000000

//写数据，写命令
#define WMLCDCOM(cmd)	   {LCD_COMMAND_ADDRESS = cmd;}
#define WMLCDDATA(data)	   {LCD_DATA_ADDRESS = data;}

#define LCD_X_MAX   (240)
#define LCD_Y_MAX   (320)


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


static inline void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t c)
{
    WMLCDCOM(0x20);
    WMLCDDATA(x);
    WMLCDCOM(0x21);
    WMLCDDATA(y);
    WMLCDCOM(0x22);
    WMLCDDATA(c);
}



//!< API functions
void ili9320_Init(void);
uint32_t ILI9320_GetDeivceID(void);
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y);
void LCD_DrawHLine(int x1, int x2, int y, uint16_t c);
void LCD_DrawVLine(int x, int y1, int y2, uint16_t c);


