
#include "shell.h"
#include "ili9320.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include "board.h"       


//fat32
#include "znfat.h"


#if 0
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

#endif

struct znFAT_Init_Args initArgSD; //初始化参数集合
struct znFAT_Init_Args initArgFlash; //初始化参数集合
struct FileInfo fileInfo1,fileInfo2; //文件信息集合
struct DateTime dtInfo1; //日期时间结构体变量


static uint8_t gBMPBuffer[512];
//BMP 显示函数 用户回调函数 用于读取数据
//实际上就是读SD卡中的数据
static uint32_t AppGetData(uint32_t ReqestedByte, uint32_t Offset, uint8_t **ppData)
{
    //返回实际可以读取的数据长度
    if(ReqestedByte > sizeof(gBMPBuffer))
    {
        ReqestedByte = sizeof(gBMPBuffer);
    }
    //读取数据
    znFAT_ReadData(&fileInfo1 ,Offset, ReqestedByte, gBMPBuffer);
    //给出数据指针位置
    *ppData = gBMPBuffer;
    //返回实际读到的长度
    return ReqestedByte;
}

static uint16_t buf[100]; 
int CMD_LCD(int argc, char * const * argv)
{
    uint32_t i;
    ili9320_init();
    memset(buf, BLUE, sizeof(buf));
  //  ili9320_set_window(0,0,10,10);
 //   ili9320_write_gram(buf, ARRAY_SIZE(buf));
    ili9320_hline(0,60,50,RED);
    ili9320_vline(100, 150, 90, GREEN);
    
    
    
    #if 0
    uint8_t res;
    uint32_t i;
    uint32_t bmp_pic_cnt;
    SD_InitTypeDef SD_InitStruct1;
    CHGUI_PID_TypeDef State;
    SD_InitStruct1.SD_BaudRate = 2000000;
    //等待SD卡初始化成功
    while(SD_Init(&SD_InitStruct1) != ESDHC_OK);
    //初始化FAT32
    znFAT_Device_Init(); //设备初始化
    znFAT_Select_Device(0,&initArgSD); //选择SD卡设备
    res = znFAT_Init();
    if(res == ERR_SUCC)
    {
        printf("Suc. to init FS\r\n");
        printf("BPB_Sector_No:%d\r\n",initArgSD.BPB_Sector_No);   
        printf("Total_SizeKB:%d\r\n",initArgSD.Total_SizeKB); 
        printf("BytesPerSector:%d\r\n",initArgSD.BytesPerSector); 
        printf("FATsectors:%d\r\n",initArgSD.FATsectors);  
        printf("SectorsPerClust:%d\r\n",initArgSD.SectorsPerClust); 
        printf("FirstFATSector:%d\r\n",initArgSD.FirstFATSector); 
        printf("FirstDirSector:%d\r\n",initArgSD.FirstDirSector); 
        printf("FSsec:%d\r\n",initArgSD.FSINFO_Sec);
        printf("Next_Free_Cluster:%d\r\n",initArgSD.Next_Free_Cluster);
        printf("FreenCluster:%d\r\n",initArgSD.Free_nCluster); 
    }
    else
    {
        GUI_printf("FAT32 Init failed CODE:%d",res);
        while(1);
    }
    
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
    
    LCD_DrawPoint(100, 100, RED);
    i = LCD_ReadPoint(100, 100);
    if (RED != i)
    {
        printf("read ponit failed 0x%X\r\n", i);
        while(1);
    }
    LCD_DrawHLine(0, 100, 30, RED);
    LCD_DrawVLine(30, 0, 100, RED);
    DelayMs(500);
    while(1)
    {
        /*
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
        */
        //确定有多少BMP
        while(znFAT_Open_File(&fileInfo1, "/PIC/*.bmp", bmp_pic_cnt, 1) == ERR_SUCC)
        {
            bmp_pic_cnt++;
        }
        while(1)
        {
            //打开SD卡中的BMP文件
            znFAT_Open_File(&fileInfo1, "/PIC/*.bmp", i, 1);
            //画图
            GUI_BMP_DrawEx(0,0, AppGetData);
            //关闭文件
            znFAT_Close_File(&fileInfo1);
            //停顿一秒
            DelayMs(1000);
            i++;
            if(i == bmp_pic_cnt) i = 0;
        }
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
