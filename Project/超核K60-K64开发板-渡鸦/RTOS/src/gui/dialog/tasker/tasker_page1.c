#include "gui_appdef.h"
#include "tasker.h"

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);

#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_TEXT_0  (GUI_ID_USER + 0x0B)
#define ID_PROGBAR_0  (GUI_ID_USER + 0x0C)
#define ID_GRAPH_0  (GUI_ID_USER + 0x0D)
#define ID_TEXT_1  (GUI_ID_USER + 0x0E)
#define ID_TEXT_2  (GUI_ID_USER + 0x10)
#define ID_PROGBAR_1  (GUI_ID_USER + 0x11)
#define ID_TEXT_3  (GUI_ID_USER + 0x12)
#define ID_GRAPH_1  (GUI_ID_USER + 0x13)

const GUI_WIDGET_CREATE_INFO _aDialogCreate1[] =
{
  { WINDOW_CreateIndirect, "Dialog", ID_FRAMEWIN_0, 0, 0, 250, 270, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 5, 5, 82, 17, 0, 0x64, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 8, 21, 50, 63, 1, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 80, 21, 135, 65, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "CPU Usage History", ID_TEXT_1, 92, 5, 104, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Memory", ID_TEXT_2, 6, 91, 90, 20, 0, 0x64, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_1, 8, 111, 50, 63, 1, 0x0, 0 },
  { TEXT_CreateIndirect, "CPU Usage History", ID_TEXT_3, 103, 91, 89, 20, 0, 0x64, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_1, 80, 110, 135, 65, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    char * buffer = rt_malloc(64);
    static GRAPH_DATA_Handle  _ahData[2]; 
    rt_uint8_t major, minor;
    rt_uint32_t  total, used, max_used;
    switch (pMsg->MsgId)
    {
        case WM_TIMER:
            cpu_usage_get(&major, &minor);
            rt_memory_info(&total, &used, &max_used);
            
            sprintf(buffer, "CPU :%d.%2d%%", major, minor);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), buffer);
            PROGBAR_SetValue(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0), major);
        
            sprintf(buffer, "Used mem:%d%%", used*100/total);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), buffer);
            PROGBAR_SetValue(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1), used*100/total);
        
            GRAPH_DATA_YT_AddValue(_ahData[0], major);
            GRAPH_DATA_YT_AddValue(_ahData[1], used*100/total);
            WM_RestartTimer(pMsg->Data.v, 100);
            break;
        case WM_INIT_DIALOG:
            WM_CreateTimer(WM_GetClientWindow(pMsg->hWin), 0, 100, 0);
        
            hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
            PROGBAR_SetBarColor(hItem, 1, GUI_GREEN);
            PROGBAR_SetBarColor(hItem, 0, GUI_RED);
        
            hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1);
            PROGBAR_SetBarColor(hItem, 1, GUI_GREEN);
            PROGBAR_SetBarColor(hItem, 0, GUI_RED);
        
            // GRAPH
            _ahData[0] = GRAPH_DATA_YT_Create(GUI_DARKGREEN, 500, 0, 0);
            _ahData[1] = GRAPH_DATA_YT_Create(GUI_BLUE, 500, 0, 0);
        
            WM_HWIN hGRAPH[2];
            int i;  
            hGRAPH[0] = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
            hGRAPH[1] = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_1);
            for(i=0;i<GUI_COUNTOF(hGRAPH);i++)
            {
                GRAPH_SetBorder(hGRAPH[i], 1, 1, 1, 1);
                GRAPH_SetGridVis(hGRAPH[i], 1);
                GRAPH_SetGridDistX(hGRAPH[i], 10);
                GRAPH_SetGridDistY(hGRAPH[i], 10); 
                GRAPH_AttachData(hGRAPH[i], _ahData[i]);                
            }
        
         //   static GRAPH_SCALE_Handle _hScaleV;   // Handle of vertical scale
         //   static GRAPH_SCALE_Handle _hScaleH;   // Handle of horizontal scale
            
        //    _hScaleV = GRAPH_SCALE_Create( 35, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 25);
        //    GRAPH_SCALE_SetTextColor(_hScaleV, GUI_YELLOW);
          //  GRAPH_AttachScale(hItem, _hScaleV);
            
          //  _hScaleH = GRAPH_SCALE_Create(155, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
         //   GRAPH_SCALE_SetTextColor(_hScaleH, GUI_DARKGREEN);
            //GRAPH_AttachScale(hItem, _hScaleH);
            
        break;
        default:
            WM_DefaultProc(pMsg);
        break;
    }
    rt_free(buffer);
}

WM_HWIN _TaskerAddPageTest(void)
{
    return GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), _cbDialog, WM_UNATTACHED, 0, 0);
}



