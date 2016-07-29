/**
 * 实验名称：Emwin_Fatfs_SRAM_中文字符显示
 * 实验平台：渡鸦开发板
 * 板载芯片：MK60DN512ZVLQ10	
 * 实验步骤：
 *       		1.将fwryh20.xbf和st20.xbf（分别表示微软雅黑20号和宋体20号）放入
 *				已经格式化的SD卡根目录；
 *				2.GUI_LoadFont_XBF函数中修改你需要使用的字体（路径）；
 * 实验效果：	
 *				1.在TFT屏幕上可显示中文字符；
 * 实验说明：
 *				1.首先你需要确保对应于你的开发板，已经能够正常使用Emwin显示英文字符，有可
 *				能不同的TFT硬件驱动需要做小幅修改，本次实验的液晶驱动为SSD1289；
 *				2.在Edit-->Configuration-->Encoding:Encode in UTF-8 without sigurature；在这种编码下；
 *				在代码中输入的中文在编译时会转化成UTF-8编码格式，Emwin才能正常的显示；
 *				3.在使用fatfs文件系统的时候，在文件diskio.c文件中有个disk_initialize函数，该函数会被
 *				其他函数调用，只要你用到了f_open等等的函数。也就是说，只要你选择两个地方之一对sd卡进行
 *				初始化就可以了，一个是main函数中，另一个是disk_initialize函数中，该例程中选择了前者。
 */
#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "GUI.h"				/* Emwin Header */
#include "IS61WV25616.h"		/* Extern SRAM Header */
#include "sd.h"					/* SD card Header */

/* defined by user for specific funcations */
#include "usrSdhc.h"

extern GUI_FONT XBF_Font; /* 字体 */

int main(void)
{	
	const TCHAR path[] = "0:/wryh20.xbf"; 
	DelayInit();

    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	/* initialize the sd card */
	if(SD_QuickInit(20*1000*1000))
    {
        printf("SD card init failed!\r\n");
        while(1);
    }
    printf("SD initialize successfully!Its size:%dMB\r\n", SD_GetSizeInMB());

	/* Initialize the external SRAM, its size is 512KB */
	SRAM_Init();
	/* Initialize the gui(LCD) */
	GUI_Init();	
	GUI_SetColor(GUI_YELLOW);
	GUI_DispStringAt("Hello Guys!", 0, 150);
	DelayMs(2000);
	GUI_Clear();
		
	/* mount file system */
	sdhc_maintask();
	
	/* display Chinese character */
	GUI_LoadFont_XBF(path);
	GUI_SetFont(&XBF_Font);
	GUI_UC_SetEncodeUTF8();	
	GUI_DispStringAt("我是一只树袋熊!", 0, 200);
	
    while(1)
    {
        //led flashing
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(200);
    }
}
