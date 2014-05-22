
#include "DIALOG.h"
#include <stdint.h>
#include <dfs.h>
#include <dfs_file.h>
#include <dfs_posix.h>
#include <stdbool.h>

#define _MAX_PATH 256
static char fullpath[_MAX_PATH];


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
                    //rt_kprintf("%s\t\t<DIR>\n", dirent->d_name);
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
            //pInfo->pExt = "<DIR>";
        }
        else
        {
            //pInfo->pExt = dirent->d_name + dirent->d_namlen - 3;
        }
        pInfo->SizeL   = s.st_size%0xFFFF;
        pInfo->SizeH   = s.st_size/0xFFFF;
    }
    if(r == 1)
    {
        closedir(dir);
    }
    rt_kprintf("!!%s\r\n", pInfo->pRoot);
    return r;
}

static CHOOSEFILE_INFO Info = { 0 };
static bool gIsSelected = false;

static void _cbBk(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            hItem = pMsg->hWin;
            break;
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            if(WM_NOTIFICATION_CHILD_DELETED == NCode)
            {
                gIsSelected = true;
                rt_kprintf("Disalbe CLOSED:%d\r\n", Id);
            }
            break;
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

char * MYGUI_DLG_ChFileGetPath(WM_HWIN hItem)
{
    if(gIsSelected == true)
    {
        gIsSelected = false;
        return Info.pRoot;
    }
    return RT_NULL;
}

WM_HWIN MYGUI_DLG_ChFile(WM_HWIN hParent)
{
    WM_HWIN hWin;
    char const      * apDrives[1]         = { working_directory };
    const char        acMask[]            = "*.*";
    Info.pfGetData = _GetData;
    Info.pMask     = acMask;
    CHOOSEFILE_SetDelim('/');
    hWin = CHOOSEFILE_Create(hParent, 0, 0, 240, 320, apDrives, GUI_COUNTOF(apDrives), 0, "File Dialog", 0, &Info);
    WM_SetCallback(hParent, _cbBk);
    return hWin;
}

