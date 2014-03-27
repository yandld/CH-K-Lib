#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>




void led1_thread_entry(void* parameter)
{
    int fd;
	rt_int32_t i;
    rt_uint8_t * ptr;
    struct stat f_stat;
    
    dfs_init();
    elm_init();
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("sd0 mount to /SD \n");
    }
    else
    {
        rt_kprintf("sd0 mount to /SD failed!\n");
    }

    GUI_Init();//³õÊ¼»¯GUI 
	GUI_DispString("Hello emWin!");//ÏÔÊ¾²âÊÔ  
    
    fd = open("/PIC/25.BMP", O_RDONLY , 0);
    if(fd >= 0)
    {
        
        stat("/PIC/25.BMP", &f_stat);
        rt_kprintf("file size:%d\r\n", f_stat.st_size);
        ptr = rt_malloc(f_stat.st_size);
        if(ptr == NULL)
        {
            rt_kprintf("no memory\r\n");
        }
        else
        {
            i = read(fd, ptr, f_stat.st_size);
            
            rt_kprintf("i = %d\r\n", i);
            rt_kprintf("Xsize:%d\r\n", GUI_BMP_GetXSize(ptr));
            rt_kprintf("Ysize:%d\r\n", GUI_BMP_GetYSize(ptr));
            GUI_BMP_Draw(ptr , 20, 20);
        }
    }
	while(1)
	{
        
        rt_thread_delay(100);
	}
}