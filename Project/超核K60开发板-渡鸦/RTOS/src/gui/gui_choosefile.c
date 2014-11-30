
#include "DIALOG.h"
#include <stdint.h>
#include <dfs.h>
#include <dfs_file.h>
#include <dfs_posix.h>
#include <stdbool.h>
#include "gui_appdef.h"
#include <finsh.h>

#define _MAX_PATH 256
static char fullpath[_MAX_PATH];


static int _GetData(CHOOSEFILE_INFO * pInfo)
{
    int         r;
    static DIR *dir;
    r = 1;
    struct dirent* dirent;
    struct stat s;
    
    switch (pInfo->Cmd)
    {
        case CHOOSEFILE_FINDFIRST:
                dir = opendir(pInfo->pRoot);
                pInfo->pName = "";
                pInfo->pAttrib = "";
                pInfo->SizeH = 0;
                pInfo->SizeL = 0;
                return 0;
        case CHOOSEFILE_FINDNEXT:
                if(dir == RT_NULL)
                {
                    rt_kprintf("opendir error\r\n");
                    r = 1;
                    break;
                }
                dirent = readdir(dir);
                if(dirent == RT_NULL)
                {
                    rt_kprintf("readdir error\r\n");
                    closedir(dir);
                    dir = 0;
                    r = 1;
                    break;
                }
                
                /* build full path for each file */
                rt_sprintf(fullpath, "%s/%s", pInfo->pRoot, dirent->d_name);
                rt_memset(&s, 0, sizeof(struct stat));
                stat(fullpath, &s);
                if (s.st_mode & DFS_S_IFDIR)
                {
                    pInfo->Flags = CHOOSEFILE_FLAG_DIRECTORY;
                }
                else
                {
                    pInfo->Flags = 0;
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
    int show_pic(const char *path);
    if(!r)
    {
        GUI_MessageBox(Info.pRoot, "File", GUI_MESSAGEBOX_CF_MODAL);
        gui_msg_t msg;
        msg.cmd = 2;
        msg.exec = GUI_AppDispImage;
        msg.parameter = Info.pRoot;
        rt_mq_send(guimq, &msg, sizeof(msg));
    }
    return Info.pRoot;
}


FINSH_FUNCTION_EXPORT(chfile, create a directory);
