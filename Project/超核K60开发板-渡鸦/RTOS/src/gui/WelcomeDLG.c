#include <dfs.h>
#include <dfs_file.h>
#include "finsh.h"




// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x06)
#define ID_TREEVIEW_0     (GUI_ID_USER + 0x08)
#define ID_BUTTON_0     (GUI_ID_USER + 0x09)
#define ID_BUTTON_1     (GUI_ID_USER + 0x0A)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "FileBraswer", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { TREEVIEW_CreateIndirect, "File", ID_TREEVIEW_0, 2, 2, 222, 267, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "open", ID_BUTTON_0, 3, 279, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "back", ID_BUTTON_1, 142, 279, 80, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
static void ListFile(WM_HWIN hItem, const char *pathname, TREEVIEW_ITEM_Handle hTreeItem)
{
    TREEVIEW_ITEM_Handle   hTreeItemCur;
    TREEVIEW_ITEM_Handle   hTreeItemNew;
    struct stat stat; 
    int length;
    char *fullpath, *path;
    struct dfs_fd fd;
    struct dirent dirent;
    fullpath = RT_NULL;
    if (pathname == RT_NULL)
    {
#ifdef DFS_USING_WORKDIR
        /* open current working directory */
        path = rt_strdup(working_directory);
#else
        path = rt_strdup("/");
#endif
        if (path == RT_NULL)
            return ; /* out of memory */
    }
    else
    {
        path = (char *)pathname;
    }
    
    /* list directory */
    if (dfs_file_open(&fd, path, DFS_O_DIRECTORY) == 0)
    {
        rt_kprintf("Directory %s:\n", path);
        do
        {
            rt_memset(&dirent, 0, sizeof(struct dirent));
            length = dfs_file_getdents(&fd, &dirent, sizeof(struct dirent));
            if (length > 0)
            {
                rt_memset(&stat, 0, sizeof(struct stat));

                /* build full path for each file */
                fullpath = dfs_normalize_path(path, dirent.d_name);
                if (fullpath == RT_NULL) 
                    break;

                if (dfs_file_stat(fullpath, &stat) == 0)
                {
                    rt_kprintf("%-20s", dirent.d_name);
                    if ( DFS_S_ISDIR(stat.st_mode))
                    {
                        hTreeItemNew = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, dirent.d_name, 66);
                        TREEVIEW_AttachItem(hItem, hTreeItemNew, hTreeItem, TREEVIEW_INSERT_FIRST_CHILD);
                        ListFile(hItem, fullpath, hTreeItemNew);
                    }
                    else
                    {
                        hTreeItemNew = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF, dirent.d_name, 66);
                        TREEVIEW_AttachItem(hItem, hTreeItemNew, hTreeItem, TREEVIEW_INSERT_BELOW);
                        ListFile(hItem, fullpath, hTreeItemNew);
                    }
                }
                else
                rt_kprintf("BAD file: %s\n", dirent.d_name);
                rt_free(fullpath);
            }
        }while(length > 0);

        dfs_file_close(&fd);
    }
    else
    {
        rt_kprintf("No such directory\n");
    }
    if (pathname == RT_NULL) 
        rt_free(path);
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  TREEVIEW_ITEM_Handle   hTreeItemCur;
  TREEVIEW_ITEM_Handle   hTreeItemNew;
  WM_HWIN                hItem;
  int                    NCode;
  int                    Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'File'
    //
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
    /* enable auto  Scrol */
    TREEVIEW_SetAutoScrollH(hItem, 1);
    TREEVIEW_SetAutoScrollV(hItem, 1);
    hTreeItemNew = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "/", 66);
    TREEVIEW_AttachItem(hItem, hTreeItemNew, 0, TREEVIEW_INSERT_BELOW);
    /* list all files */
    ListFile(hItem, "/", hTreeItemNew);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_TREEVIEW_0: // Notifications sent by 'File'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_0: // Notifications sent by 'open'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'back'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        GUI_EndDialog(pMsg->hWin, 0);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateRaven
*/
WM_HWIN CreateRaven(void);
WM_HWIN CreateRaven(void) {
  WM_HWIN hWin;
  BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
