#include "DIALOG.h"
#include <stdint.h>
#include <dfs.h>
#include <dfs_file.h>
#include <dfs_posix.h>


#define ID_FRAMEWIN_0    (GUI_ID_USER + 0x00)
#define ID_MULTIEDIT_0    (GUI_ID_USER + 0x02)
#define ID_BUTTON_0    (GUI_ID_USER + 0x03)
#define ID_BUTTON_1    (GUI_ID_USER + 0x04)

static rt_sem_t sem = RT_NULL;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Notepad", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_0, -2, 1, 235, 275, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "open", ID_BUTTON_0, 1, 280, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "close", ID_BUTTON_1, 150, 279, 80, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

int MYGUI_DLG_NotepadSetText(WM_HWIN hItem, const char * FilePath, U32 MaxChars)
{
    int fd;
    char *pData;
    int  len;
    struct stat s;
    fd = open(FilePath, O_RDONLY, 0);
	if (fd < 0)
	{
		rt_kprintf("open file for read failed\n");
		return 1;
	}
    rt_kprintf("path:%s\r\n", FilePath);
    stat(FilePath, &s);
    if(MaxChars < s.st_size) 
    {
        len = MaxChars;
    }
    else
    {
        len = s.st_size;
    }
    rt_kprintf("file_size:%d alloc_size:%d\r\n",s.st_size, len);
    //MULTIEDIT_SetMaxNumChars(hItem, len);
    pData = rt_malloc(len);
    if(pData == RT_NULL)
    {
        rt_kprintf("no memory!\r\n");
    }
    else
    {
        len = read(fd, pData, len);
        MULTIEDIT_SetText(hItem, pData);
        rt_free(pData);
    }
    close(fd);
    return 0;
}

struct
{
    WM_HWIN hParent;
    const char *pFilePath;
}NotePad;

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
    MULTIEDIT_SetFont(hItem, GUI_FONT_13B_1);
    MULTIEDIT_SetInsertMode(hItem, 0);
    //MULTIEDIT_SetReadOnly(hItem, 1);
    MULTIEDIT_SetAutoScrollV(hItem, 1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'open'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        break;
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'close'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        if(rt_sem_take(sem, RT_WAITING_NO) == RT_EOK)
        {
            GUI_EndDialog(pMsg->hWin, 0);
        }
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



const char *MYGUI_ExecDialog_ChFile(WM_HWIN hParent, const char *pMask);

static void thread_entry(void* parameter)
{
    int r;
    WM_HWIN  hDialog;
    WM_HWIN  hMutiEdit;
    const char *p;
    sem = rt_sem_create("sem_nopepad", 1, RT_IPC_FLAG_FIFO);
    hDialog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, NotePad.hParent, 0, 0);
    hMutiEdit = WM_GetDialogItem(hDialog, ID_MULTIEDIT_0);
    if(NotePad.pFilePath != NULL)
    {
        r = MYGUI_DLG_NotepadSetText(hMutiEdit, NotePad.pFilePath, 1024);
        if(r)
        {
            goto __OpenNew;
        }
    }
    else
    {
        __OpenNew:
        rt_sem_take(sem, RT_WAITING_FOREVER);
        p = MYGUI_ExecDialog_ChFile(hDialog, "*.*");
        rt_sem_release(sem);
        if(p != NULL)
        {
            MYGUI_DLG_NotepadSetText(hMutiEdit, p, 1024);
        }
    }
}


    
int THREAD_Notepad(WM_HWIN hParent, const char *pFilePath)
{
    NotePad.hParent = hParent;
    NotePad.pFilePath = pFilePath;
    rt_thread_t tid1;
    tid1 = rt_thread_create("t_notepad", thread_entry, (void*)0, 4096*2, 0x10, 5);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
        return 0;
    }
    return 1;
}



