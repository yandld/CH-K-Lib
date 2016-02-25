#include "usrSdhc.h"

GUI_FONT XBF_Font; // GUI_FONT structure in RAM
static GUI_XBF_DATA XBF_Data; // GUI_XBF_DATA structure in RAM
static FIL gFontNew;
static FATFS gFs; // define for mount file system

void sdhc_maintask(void)
{
	FRESULT res;
	res = f_mount(&gFs, "0:", 0);
	if (res != FR_OK)
	{
		return;
	}
//	res = f_mount(NULL, "0:", 0);
}

void GUI_LoadFont_XBF(const TCHAR *str)
{
	FRESULT res;
	
	res = f_open(&gFontNew, str, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (res != FR_OK)
	{
		return;
	}

	/* Create XBF font */
	GUI_XBF_CreateFont(&XBF_Font, // Pointer to GUI_FONT structure in RAM(3)
					   &XBF_Data, // Pointer to GUI_XBF_DATA structure in RAM
					   GUI_XBF_TYPE_PROP_AA4_EXT, // Font type to be created
					   _cbGetData, // Pointer to callback function
					   &gFontNew); // Pointer to be passed to GetData function
}

/**
 * \brief Callback function for getting font data
 */
static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer)
{
	FIL *FontFile;
	FRESULT res;
	UINT br;
	/* The pVoid pointer may be used to get a file handle */
	FontFile = (FIL *)pVoid;
	/* Set file pointer to the required position */
	res = f_lseek(FontFile, Off);
	if (res != FR_OK)
	{
		return 1; /* Error */
	}
	/* Read data into buffer */
	res = f_read(FontFile, pBuffer, NumBytes, &br);
	if (res != FR_OK)
	{
		return 1; /* Error */
	}
	return 0;
}
