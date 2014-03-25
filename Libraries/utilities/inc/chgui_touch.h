#ifndef _CHGUI_TOUCH_H_
#define _CHGUI_TOUCH_H_
#include <stdint.h>

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint8_t Pressed;
}CHGUI_PID_TypeDef;


#define GUI_TOUCH_AD_LEFT    140   
#define GUI_TOUCH_AD_RIGHT   1900   
#define GUI_TOUCH_AD_TOP     1960  
#define GUI_TOUCH_AD_BOTTOM  160
#define GUI_TOUCH_SWAP_XY    0
#define GUI_TOUCH_MIRROR_X   0
#define GUI_TOUCH_MIRROR_Y   0
#define GUI_TOUCH_XSIZE      240
#define GUI_TOUCH_YSIZE      320


//API functions
uint32_t GUI_TOUCH_GetxPhys(void);
uint32_t GUI_TOUCH_GetyPhys(void);
void GUI_TOUCH_Exec(void);
int GUI_TOUCH_GetState(CHGUI_PID_TypeDef *pState);

#endif


