#include <rthw.h>
#include <rtthread.h>
#include "ili9320.h"


static rt_uint16_t deviceid;     //设置一个静态变量用来保存LCD的ID
struct rt_device lcd_device;	 //设备框架结构体  RTGRAPHIC_PIXEL_FORMAT_RGB565

/*  设置像素点 颜色,X,Y */
void rt_hw_lcd_set_pixel(const char* c, int x, int y)
{
    rt_uint16_t p;
    p =  *(uint16_t *)c;
    ili9320_write_pixel(x, y, p);

}

/* 获取像素点颜色 */
void rt_hw_lcd_get_pixel(char* c, int x, int y)
{
    *(uint16_t*)c = ili9320_read_pixel(x, y);
}


void rt_hw_lcd_draw_blit_line(const char* c, int x, int y, rt_size_t size)
{
    rt_kprintf("rt_hw_lcd_draw_blit_line\r\n");
    rt_uint16_t *ptr;
    ptr = (rt_uint16_t*)c;
    WMLCDCOM(0x20);
    WMLCDDATA(x);
    WMLCDCOM(0x21);
    WMLCDDATA(y);
    WMLCDCOM(0x22);
    while (size)
    {
        WMLCDDATA(*ptr ++);
        size--;
    }
}

void rt_hw_lcd_draw_hline(const char* c, int x1, int x2, int y)
{
    rt_uint16_t p;
    p = *(uint16_t *)c;
   // ILI9320_DrawHLine(x1, x2, y, p);
}

void rt_hw_lcd_draw_vline(const char* c, int x, int y1, int y2)
{
    rt_uint16_t p;
    p = *(uint16_t *)c;
  //  ILI9320_DrawVLine(x, y1, y2, p);
}

struct rt_device_graphic_ops lcd_ili_ops =
{
    rt_hw_lcd_set_pixel,
    rt_hw_lcd_get_pixel,
    rt_hw_lcd_draw_hline,
    rt_hw_lcd_draw_vline,
    rt_hw_lcd_draw_blit_line
};

static rt_err_t lcd_init (rt_device_t dev)
{

    return RT_EOK;
}

static rt_err_t lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t lcd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info;

        info = (struct rt_device_graphic_info*) args;
        RT_ASSERT(info != RT_NULL);

        info->bits_per_pixel = 16;
        info->pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565P;
        info->framebuffer = RT_NULL;

        info->width = LCD_X_MAX;
        info->height = LCD_Y_MAX;

    }
    break;

    case RTGRAPHIC_CTRL_RECT_UPDATE:
        /* nothong to be done */
        break;

    default:
        break;
    }

    return RT_EOK;
}

void rt_hw_lcd_init(void)
{

	lcd_device.type 		= RT_Device_Class_Graphic;
	//sd_device.rx_indicate = rt_sd_indicate;
//	sd_device.tx_complete = RT_NULL;
	lcd_device.init 		= lcd_init;
	lcd_device.open		= lcd_open;
	lcd_device.close		= lcd_close;
	lcd_device.read 		= RT_NULL;
	lcd_device.write 		= RT_NULL;
	lcd_device.control 	= lcd_control;
	lcd_device.user_data	= &lcd_ili_ops;
	
    rt_device_register(&lcd_device, "lcd",  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
    
}