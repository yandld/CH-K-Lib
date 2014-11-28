#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

#include "WM.H"

#define GUI_IMAGE_BMP   0
#define GUI_IMAGE_JPG   1
#define GUI_IMAGE_JPEG  2


//!< API functions 
void GUI_IMAGE_DisplayImage(int fd, U32 size);
WM_HWIN GUI_IMAGE_CreateWidget(WM_HWIN hWin);

#endif


