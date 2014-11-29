#include <rtthread.h>
#include <finsh.h>
#include <dfs_posix.h>

#include "msh.h"
#include "gui_image.h"

#ifdef DFS_USING_WORKDIR
extern char working_directory[];
#endif


int show_pic(const char *path)
{
    rt_thread_t tid = rt_thread_find("gui_exe");
    if(!tid)
    {
        rt_kprintf("ERROR! no GUI\r\n");
        return -1;
    }


    GUI_IMAGE_DisplayImage(path);

    return 0;
}


int cmd_pic(int argc, char** argv)
{
    if (argc == 2)
    {
        show_pic(argv[1]);
    }
    return 0;
}


FINSH_FUNCTION_EXPORT_ALIAS(cmd_pic, __cmd_pic, show a picture file.);
