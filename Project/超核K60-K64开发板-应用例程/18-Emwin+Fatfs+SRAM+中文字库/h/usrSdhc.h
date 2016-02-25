#ifndef _USRSDHC_H
#define _USRSDHC_H

	#include <string.h>
	#include "ff.h"
	#include "GUI.h"
	
	extern void sdhc_maintask(void);
	static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer);
	extern void GUI_LoadFont_XBF(const TCHAR *str);
	
#endif
