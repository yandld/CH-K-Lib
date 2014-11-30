#include <rtthread.h>
#include <finsh.h>
#include <dfs_posix.h>

#include "gui_appdef.h"

#ifdef DFS_USING_WORKDIR
extern char working_directory[];
#endif


int cmd_pic(int argc, char** argv)
{
    char *fullpath;
    if(argc != 2)
    {
        return -1;
    }
        rt_thread_t tid = rt_thread_find("gui_exe");
        if(!tid)
        {
            rt_kprintf("ERROR! no GUI\r\n");
            return -1;
        }       
    fullpath = dfs_normalize_path(NULL, argv[1]);
    gui_msg_t msg;
    msg.cmd = 2;
    msg.exec = GUI_AppDispImage;
    msg.parameter = (void *)fullpath;
    rt_mq_send(guimq, &msg, sizeof(msg));
    return 0;
}


FINSH_FUNCTION_EXPORT_ALIAS(cmd_pic, __cmd_pic, show a picture file.);
