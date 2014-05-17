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
        rt_kprintf("file size:%d\r\n", f_stat.st_size);
        ptr = rt_malloc(f_stat.st_size);
        if(ptr == NULL)
        {
            rt_kprintf("no memory\r\n");
        }
        else
        {
            
            read(fd, ptr, f_stat.st_size);
            char * ex_name_pos = (char*)((rt_uint32_t)path + rt_strlen(path) - 3);
            if(!rt_strncmp(ex_name_pos, "BMP", 3))
            {
                GUI_IMAGE_DisplayImage(ptr, f_stat.st_size, GUI_IMAGE_BMP);
            }
            if(!rt_strncmp(ex_name_pos, "JPG", 3))
            {
                GUI_IMAGE_DisplayImage(ptr, f_stat.st_size, GUI_IMAGE_JPG);
            }
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


int cmd_pic(int argc, char** argv)
{
    if (argc == 1)
    {
        rt_kprintf("%s\n", working_directory);
    }
    else if (argc == 2)
    {
        show_pic(argv[1]);
    }
    return 0;
}


FINSH_FUNCTION_EXPORT_ALIAS(cmd_pic, __cmd_pic, show a picture file.);
