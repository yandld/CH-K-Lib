#include <rtthread.h>
#include <finsh.h>
#include <dfs_posix.h>

#include "msh.h"


#ifdef DFS_USING_WORKDIR
extern char working_directory[];
#endif



static int _bmp(const char *path)
{
    char *fullpath;
    int fd;
    if (path == RT_NULL)
    {
        dfs_lock();
        rt_kprintf("%s\n", working_directory);
        dfs_unlock();

        return 0;
    }

    if (rt_strlen(path) > DFS_PATH_MAX)
    {
        rt_set_errno(-DFS_STATUS_ENOTDIR);

        return -1;
    }

    fullpath = dfs_normalize_path(NULL, path);
    if (fullpath == RT_NULL)
    {
        rt_set_errno(-DFS_STATUS_ENOTDIR);

        return -1; /* build path failed */
    }

    dfs_lock();
    fd = open(fullpath, O_RDONLY , 0);
    if(fd >= 0)
    {
        struct stat f_stat;
        rt_uint8_t *ptr;
        stat(fullpath, &f_stat);
        rt_kprintf("file size:%d", f_stat.st_size);
        ptr = rt_malloc(f_stat.st_size);
        if(ptr == NULL)
        {
            rt_kprintf("no memory\r\n");
        }
        else
        {
            read(fd, ptr, f_stat.st_size);
            rt_kprintf(" W:%d", GUI_BMP_GetXSize(ptr));
            rt_kprintf(" H:%d\r\n", GUI_BMP_GetYSize(ptr));
            GUI_BMP_Draw(ptr , 0, 0);
            rt_free(ptr); 
            close(fd);
        }
    }
    else
    {
        rt_kprintf("open file failed\r\n");
    }
    /* close directory stream */
    rt_free(fullpath);
    
    dfs_unlock();
    return 0;
}


int cmd_bmp(int argc, char** argv)
{
    if (argc == 1)
    {
        rt_kprintf("%s\n", working_directory);
    }
    else if (argc == 2)
    {
        _bmp(argv[1]);
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_bmp, __cmd_bmp, show a BMP file.);
