
#include "DIALOG.h"
#include <stdint.h>
#include <dfs.h>
#include <dfs_file.h>
#include <dfs_posix.h>

#define _MAX_PATH 256
static char fullpath[_MAX_PATH];

#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x0A)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "File", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
};


static int _GetData(CHOOSEFILE_INFO * pInfo)
{
    int         r;
    static      DIR *dir;
    r = 1;
    struct dirent* dirent;
    struct stat s;

    switch (pInfo->Cmd)
    {
        case CHOOSEFILE_FINDFIRST:
                dir = opendir(pInfo->pRoot); 
                //rt_kprintf("CHOOSEFILE_FINDFIRST:%d\r\n", dir);
                if(dir == RT_NULL)
                {
                    return 1;
                }
        case CHOOSEFILE_FINDNEXT:
                dirent = readdir(dir);
                if(dirent == RT_NULL)
                {
                    r = 1;
                    break;
                }
                    /* build full path for each file */
                    rt_sprintf(fullpath, "%s/%s", pInfo->pRoot, dirent->d_name);
                    stat(fullpath, &s);
                    if (s.st_mode & DFS_S_IFDIR)
                    {
                      //  rt_kprintf("%s\t\t<DIR>\n", dirent->d_name);
                        pInfo->Flags = CHOOSEFILE_FLAG_DIRECTORY;
                    }
                    else
                    {
                        pInfo->Flags = 0;
                        //rt_kprintf("%s\t\t%lu\n", dirent->d_name, s.st_size);
                    }
                    r = 0;
            break;
    }
    
    if(r == 0)
    {
        pInfo->pAttrib = "RW";
        pInfo->pName   = dirent->d_name;
        if(s.st_mode & DFS_S_IFDIR)
        {
            pInfo->pExt = "<DIR>";
        }
        else
        {
            pInfo->pExt = dirent->d_name + dirent->d_namlen - 3;
        }
        pInfo->SizeL   = s.st_size%0xFFFF;
        pInfo->SizeH   = s.st_size/0xFFFF;
    }
    if(r == 1)
    {
        closedir(dir);
    }
    return r;
}

WM_HWIN _CreateChooseFileWidget(WM_HWIN hParent)
{
    WM_HWIN hWin;
    CHOOSEFILE_INFO   Info                = { 0 };
    char const      * apDrives[1]         = { working_directory };
    const char        acMask[]            = "*.*";
    Info.pfGetData = _GetData;
    Info.pMask     = acMask;
    CHOOSEFILE_SetDelim('/');
    hWin = CHOOSEFILE_Create(hParent, 0, 0, 240, 320, apDrives, GUI_COUNTOF(apDrives), 0, "File Dialog", 0, &Info);
    return hWin;
}



static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = pMsg->hWin;
    _CreateChooseFileWidget(hItem);
    break;

  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

WM_HWIN CreateChooseFile(hParent)
{
    WM_HWIN hWin;
    if(hParent == NULL)
    {
        hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    }
    else
    {
        hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hParent, 0, 0);
    }
    return hWin;
}

