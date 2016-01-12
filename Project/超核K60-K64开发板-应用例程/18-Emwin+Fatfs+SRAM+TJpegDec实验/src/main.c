/*
 * 实验名称：Emwin_Fatfs_SRAM_TJpegDec
 * 实验平台：渡鸦开发板
 * 板载芯片：MK60DN512ZVLQ10
 * 实验步骤：
 *				1.将../Pictures/hkk.jpg文件放到已经格式化的SD卡中；
 *				2.因LCD驱动器的型号不同，ili9320_get_id()函数可获得驱动器型号，本例程的驱动为0x8989 (ssd1289)；
 *    			3.若驱动器型号不同，参考该帖子http://www.beyondcore.net/forum.php?mod=viewthread&tid=2650&extra=page%3D1&page=2 进行相应的修改；
 * 实验说明：
 *				1.GUI_DrawBitmap(&bma, 0, 0);显示存放在flash中的bmp图像；
 *				2.GUI_JPEG_Draw(jpgHankeku.picAddr,jpgHankeku.picSize,0,0);显示存放在flash中的jpeg图像；
 *				3.sdhc_maintask();SD卡的读写文件操作；
 *				4.jpegDispFromFile(filename);从SD卡中读取名为filename的jpeg图片。
 *				5.以上emwin的内存选用的是外部的512KB的SRAM作为动态内存；
 *				6.JPG图片的编码/解码采用ChaN的TJpegDec解码方式，仅供学习交流，相关版权归作者本人所有；
 *				7.例程中的5个warning是由于将int型返回给enum类型，强制转换即可消除warning，笔者留着就当作是提醒吧！
 */
#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "GUI.h"				/* Emwin Header */
#include "IS61WV25616.h"		/* Extern SRAM Header */
#include "sd.h"					/* SD card Header */

/* defined by user for specific funcations */
#include "usrSdhc.h"
#include "usrJpegIO.h"
#include "usrPicture.h"

extern GUI_CONST_STORAGE GUI_BITMAP bma;

int main(void)
{
	TCHAR *filename = "0:/hkk.jpg";
	
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
	
	/* display the bitmap */
	GUI_DrawBitmap(&bma, 0, 0);
	DelayMs(2000);
	GUI_Clear();
	
	/* draw the jpeg file stored in flash */
	GUI_JPEG_Draw(jpgHankeku.picAddr,jpgHankeku.picSize,0,0);
	DelayMs(2000);
	GUI_Clear();
	
	/* create/read and write file on sd card */
	sdhc_maintask();
	DelayMs(2000);
	GUI_Clear();
	
	/* read the hkk.jpg from sd card and display on lcd with external sram */
	jpegDispFromFile(filename);
	
    while(1)
    {
        //led flashing
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(200);
    }
}
