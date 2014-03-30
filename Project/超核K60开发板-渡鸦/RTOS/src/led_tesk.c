#include <rtthread.h>
#include <rthw.h>

//DFS
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>

#include <drivers/spi.h>


void led1_thread_entry(void* parameter)
{
    int fd;
	rt_int32_t i;
    rt_uint8_t * ptr;
    struct stat f_stat;
    struct rt_spi_device *spi_device;
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
    
    spi_device = (struct rt_spi_device *)rt_device_find("spi20");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device found\r\n");
    }
    else
    {
        w25qxx_init("spi_flash", "spi20");
        rt_uint8_t send_buffer[10];
        send_buffer[0] = 10;
        send_buffer[1] = 11;
        rt_uint8_t rec_buffer[10];
       // spi->open(spi, RT_DEVICE_OFLAG_RDWR);
        rt_spi_send(spi_device, send_buffer, 3);                      
    }

    
    
   // GUI_Init();//≥ı ºªØGUI 
	//GUI_DispString("Hello emWin!");//œ‘ æ≤‚ ‘  
 //   MainTask();
	while(1)
	{
        rt_thread_delay(100);
	}
}