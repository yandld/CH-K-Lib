#include <rtthread.h>
#include <rthw.h>

#include "DIALOG.h"


#define     TOUCH_IDLE          (0x00)
#define     TOUCH_STATUS2       (0x01)

static void GUI_SettingInit(void)
{
	GUI_SetBkColor(GUI_BLACK); 	
	GUI_SetColor(GUI_BLACK);
    ICONVIEW_EnableStreamAuto();
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    
}


void gui_thread_entry(void* parameter)
{
    rt_thread_t thread = rt_thread_self();
    static rt_uint32_t Status;
    GUI_Init();//³õÊ¼»¯GUI 
    GUI_SettingInit();
	GUI_DispString("GUI system OK");//ÏÔÊ¾²âÊÔ
    WM_HWIN hWin;
   // CreateWelcome();
  //  GUI_Exec();
  //  show_pic("/SD/PIC/INDEX.BMP");
  //  GUI_CURSOR_Show();
    
    hWin = GUI_Desktop();
   // MainMenu_Init();
  //  while(WM_GetWindowOrgY(hWin) > 0)
    {
    //    WM_MoveWindow(hWin, 0, -6);
   //     rt_thread_delay(1);
    }
    
    GUI_PID_STATE  InitalTouchPoint;
    GUI_PID_STATE  TouchPoint;
	while(1)
	{
        GUI_TOUCH_GetState(&TouchPoint);
        switch(Status)
        {
            case TOUCH_IDLE:
                if(TouchPoint.Pressed)
                {
                    InitalTouchPoint = TouchPoint;
                    Status = TOUCH_STATUS2;
                }
                break;
            case TOUCH_STATUS2:
                if(TouchPoint.Pressed)
                {
                    if(abs(InitalTouchPoint.x - TouchPoint.x) > 5)
                    {
                        WM_MoveWindow(hWin, TouchPoint.x - InitalTouchPoint.x , 0);
                    }
                    Status = TOUCH_STATUS2;
                }
                else
                {
                    Status = TOUCH_IDLE;
                }
                break;
        }
        
        if(TouchPoint.Pressed)
        {
          //  WM_MoveWindow(hWin, 1, 0);
        }
        
        //   WM_MoveCtrl();
        
        // GUI_TOUCH_Exec();
        rt_thread_delay(1);
	}
}