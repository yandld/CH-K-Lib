#ifndef _USRJPEGIO_H
#define _USRJPEGIO_H

	#include "tjpgd.h"
	#include "ff.h"
	#include "uart.h"
	#include "GUI.h"

	#if 0
		#define JPEG_TRACE printf
	#else
		#define JPEG_TRACE(...)
	#endif
	
	typedef struct {
		FIL *fp;      	/* 指向输入文件的指针（JPEG文件）*/  
		BYTE *fbuf;    	/* 指向输出缓冲区的指针  */
		UINT wfbuf;    	/* 输出缓冲区的宽度[pix] */
	} IODEV;
	
	void jpegDispFromFile(const char *filename);
	static UINT JpegInFunc(JDEC *jd, BYTE *buff, UINT num);
	static UINT JpegOutFunc(JDEC *jd, void * bitmap, JRECT *rect);
	
#endif
