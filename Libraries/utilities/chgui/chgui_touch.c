#include "chgui.h"
#include "chgui_char.h"
#include "chgui_touch.h"
#include <stdlib.h>

static int xPhys, yPhys;
typedef struct {int Min; int Max; } tMinMax;
static CHGUI_PID_TypeDef gPID_State;

// Decide 4 max AD Value 
static tMinMax xyMinMax[2] = {
#if ((GUI_TOUCH_SWAP_XY==0) && (GUI_TOUCH_MIRROR_X==0)) || ((GUI_TOUCH_SWAP_XY) && (GUI_TOUCH_MIRROR_Y==0))
  { GUI_TOUCH_AD_LEFT, GUI_TOUCH_AD_RIGHT },
#else
  { GUI_TOUCH_AD_RIGHT, GUI_TOUCH_AD_LEFT },
#endif
#if ((GUI_TOUCH_SWAP_XY==0) && (GUI_TOUCH_MIRROR_Y==0)) || ((GUI_TOUCH_SWAP_XY) && (GUI_TOUCH_MIRROR_X==0))
  { GUI_TOUCH_AD_TOP,  GUI_TOUCH_AD_BOTTOM }
#else
  { GUI_TOUCH_AD_BOTTOM,  GUI_TOUCH_AD_TOP }
#endif
};

static int xMin;
static int xMax;
static int yMin;
static int yMax;

static int _AD2X(int adx) 
{
    int32_t r = adx - xyMinMax[0].Min;
    r *= GUI_TOUCH_XSIZE - 1;
    return r / (xyMinMax[0].Max - xyMinMax[0].Min);    
}

static int _AD2Y(int ady)
{
    int32_t r = ady - xyMinMax[1].Min;
    r *= GUI_TOUCH_YSIZE - 1;
    return r/(xyMinMax[1].Max - xyMinMax[1].Min);    
}

uint32_t GUI_TOUCH_GetxPhys(void)
{
    return xPhys;
}

uint32_t GUI_TOUCH_GetyPhys(void)
{
   return yPhys;
}

static void GUI_TOUCH_StoreState(int x, int y)
{
    static int _x, _y;
    int xDiff, yDiff;
    static CHGUI_PID_TypeDef _State;
    if((x < 0) && (y < 0))
    {
        _State.Pressed = 0;
        gPID_State = _State;
        return;
    }
	
    xDiff = abs (x - _x);
    yDiff = abs (y - _y);
	
    if (xDiff + yDiff > 2)
    {
        _x = x;
        _y = y;
        _State.x = x;
        _State.y = y;
        _State.Pressed = 1;
        gPID_State = _State;
    } 
}


int GUI_TOUCH_GetState(CHGUI_PID_TypeDef *pState) 
{
    *pState = gPID_State;
    return (gPID_State.Pressed != 0) ? 1 : 0;
}

void GUI_TOUCH_Exec(void)
{
    int x,y;
    // calculate Min / Max values 
    if (xyMinMax[0].Min < xyMinMax[0].Max)
    {
        xMin = xyMinMax[0].Min;
        xMax = xyMinMax[0].Max;
    }
    else
    {
        xMax = xyMinMax[0].Min;
        xMin = xyMinMax[0].Max;
    }
    if (xyMinMax[1].Min < xyMinMax[1].Max)
    {
        yMin = xyMinMax[1].Min;
        yMax = xyMinMax[1].Max;
    }
    else
    {
        yMax = xyMinMax[1].Min;
        yMin = xyMinMax[1].Max;
    }
    //参数合法性检测
    if((gpCHGUI->tops->ctrl_get_touch_x == NULL) || (gpCHGUI->tops->ctrl_get_touch_y == NULL))
    {
        return;
    }
    xPhys = gpCHGUI->tops->ctrl_get_touch_x();
    yPhys = gpCHGUI->tops->ctrl_get_touch_y();
    // Convert values into logical values 
    #if !GUI_TOUCH_SWAP_XY   // Is X/Y swapped ?
      x = xPhys;
      y = yPhys;
    #else
      x = yPhys;
      y = xPhys;
    #endif
    if ((x < xMin) || (x > xMax)  || (y < yMin) || (y > yMax)) 
    {
        GUI_TOUCH_StoreState(-1, -1);
    }
    else 
    {
        x = _AD2X(x);
        y = _AD2Y(y);
        GUI_TOUCH_StoreState(x, y);
    }
}



