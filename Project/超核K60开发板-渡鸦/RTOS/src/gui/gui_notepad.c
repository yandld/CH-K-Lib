#include "DIALOG.h"
#include <stdint.h>
#include <dfs.h>
#include <dfs_file.h>
#include <dfs_posix.h>


#define ID_FRAMEWIN_0    (GUI_ID_USER + 0x00)
#define ID_MULTIEDIT_0    (GUI_ID_USER + 0x02)
#define ID_BUTTON_0    (GUI_ID_USER + 0x03)
#define ID_BUTTON_1    (GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Notepad", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_0, -2, 1, 235, 275, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "open", ID_BUTTON_0, 1, 280, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "close", ID_BUTTON_1, 150, 279, 80, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};


WM_HWIN MYGUI_DLG_NotepadAddText(WM_HWIN hItem, const char * FilePath, U32 MaxChars)
{
    int fd;
    char *pData;
    int  len;
    struct stat s;
    fd = open(FilePath, O_RDONLY, 0);
	if (fd < 0)
	{
		rt_kprintf("open file for write failed\n");
		return RT_NULL;
	}
    stat(FilePath, &s);
    if(MaxChars < s.st_size) 
    {
        len = MaxChars;
    }
    else
    {
        len = s.st_size;
    }
    rt_kprintf("size:%d\r\n", len);
    //MULTIEDIT_SetMaxNumChars(hItem, len);
    pData = rt_malloc(len);
    if(pData == RT_NULL)
    {
        rt_kprintf("no memory!\r\n");
        return RT_NULL;
    }
	len = read(fd, pData, len);
    MULTIEDIT_SetText(hItem, pData);
    close(fd);
    rt_free(pData);
}


static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = pMsg->hWin;
    //FRAMEWIN_SetMoveable(hItem, 1);
    FRAMEWIN_SetFont(hItem, GUI_FONT_13B_1);
    FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT,  0);
    FRAMEWIN_AddMaxButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMinButton(hItem, FRAMEWIN_BUTTON_RIGHT, 1);
  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
    MULTIEDIT_SetWrapWord(hItem);
    MULTIEDIT_SetInsertMode(hItem, 0);
    //MULTIEDIT_SetReadOnly(hItem, 1);
    MULTIEDIT_SetFocussable(hItem, 0);
    MULTIEDIT_SetAutoScrollV(hItem, 1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'open'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
         
        //GUI_EndDialog(pMsg->hWin, 0);
        break;
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'close'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        GUI_EndDialog(pMsg->hWin, 0);
        break;
      }
      break;
    }
    break;

  default:
    WM_DefaultProc(pMsg);
    break;
  }
}


static rt_thread_t tid1 = RT_NULL;
char * MYGUI_DLG_ChFileGetPath(WM_HWIN hItem);

static void thread_entry(void* parameter)
{
    char *p;
    U32 r;
    WM_HWIN  hWin;
    hWin = MYGUI_DLG_ChFile(WM_HBKWIN);
    r = GUI_ExecCreatedDialog(hWin);
    p = MYGUI_DLG_ChFileGetPath(hWin);
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    hWin = WM_GetDialogItem(hWin, ID_MULTIEDIT_0);
    MYGUI_DLG_NotepadAddText(hWin, p, 1024);
    rt_kprintf("~~~%s\r\n", "!!");
}

int THREAD_Notepad(void)
{
    tid1 = rt_thread_create("t_notepad", thread_entry, (void*)1, 4096, 20, 5);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1); 
    }
    return 0;
}



