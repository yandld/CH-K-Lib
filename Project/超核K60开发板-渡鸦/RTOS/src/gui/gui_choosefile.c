
#include "DIALOG.h"
#include <stdint.h>
#include <dfs.h>
#include <dfs_file.h>
#include <dfs_posix.h>
#include <stdbool.h>

#include <finsh.h>

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
                if(dir == RT_NULL) return 1;
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
        pInfo->pName = dirent->d_name;
        if(s.st_mode & DFS_S_IFDIR)
        {
            pInfo->pAttrib = "DIR";
        }
        else
        {
            pInfo->pAttrib = "RW";
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

const char *chfile(WM_HWIN hParent, const char *pMask)
{
    int r;
    WM_HWIN hWin;
    static CHOOSEFILE_INFO Info = { 0 };
    static char const      * apDrives[1]         = {"/"};
    Info.pfGetData = _GetData;
    Info.pMask     = "*.*";
    CHOOSEFILE_SetDelim('/');
    
    hWin = CHOOSEFILE_Create(WM_HBKWIN, 0, 0, LCD_GetXSize()*3/4, LCD_GetYSize()*3/4, apDrives, GUI_COUNTOF(apDrives), 0, "File Dialog", 0, &Info);
    WM_MakeModal(hWin);
    FRAMEWIN_SetMoveable(hWin, 1);
    r = GUI_ExecCreatedDialog(hWin);
    if(!r)
    {
        GUI_MessageBox(Info.pRoot, "File", GUI_MESSAGEBOX_CF_MODAL);
        show_pic(Info.pRoot);
    }
    return Info.pRoot;
}


FINSH_FUNCTION_EXPORT(chfile, create a directory);
