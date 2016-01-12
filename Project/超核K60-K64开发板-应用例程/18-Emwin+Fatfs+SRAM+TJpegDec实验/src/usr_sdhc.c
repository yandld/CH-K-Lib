#include "ff.h"
#include "GUI.h"
#include "usrSdhc.h"

void sdhc_maintask(void)
{
	FIL fnew;
	FATFS fs;
	FRESULT res;
	UINT br,bw;
	BYTE buffer[64];
	const TCHAR *testFileBuffer = "Welcome to Kinetis! I am Andyxu\n";
	
	GUI_RECT file_content = {5,120,235,200};
	GUI_ClearRect(0,0,240,320);
	GUI_SetFont(&GUI_Font8x16);
	GUI_DispStringAt("f_mount...",5,20);
	/* reference diskio.c #define SDHC	3 */
	res = f_mount(&fs, "0:", 0);
	GUI_DispStringAt("f_open: newfile.c...",5,40);
    res = f_open(&fnew, "0:/newfile.c", FA_WRITE | FA_CREATE_ALWAYS);
	if(res == FR_OK)
	{
		res = f_write(&fnew, testFileBuffer, strlen(testFileBuffer), &bw);
		f_close(&fnew);
	}
	GUI_DispStringAt("f_read: newfile.c ...",5,60);
	res = f_open(&fnew, "0:/newfile.c", FA_READ);
	res = f_read(&fnew, buffer, sizeof(buffer), &br);
	GUI_DispStringAt("file content:",5,100);
	
	GUI_SetColor(GUI_RED);
	GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
	GUI_DispStringInRectWrap((const char*)buffer, &file_content,
	GUI_TA_LEFT,GUI_WRAPMODE_WORD);
	
	res = f_close(&fnew);
	res = f_mount(NULL, "0:", 0);
}
