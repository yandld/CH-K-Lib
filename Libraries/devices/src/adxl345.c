/**
  ******************************************************************************
  * @file    adxl345.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "i2c_abstraction.h"
#include "adxl345.h"
#include <math.h>

#define DEVICE_ID		0X00 	//器件ID,0XE5
#define THRESH_TAP		0X1D   	//敲击阀值
#define OFSX			0X1E
#define OFSY			0X1F
#define OFSZ			0X20
#define DUR				0X21
#define Latent			0X22
#define Window  		0X23
#define THRESH_ACK		0X24
#define THRESH_INACT	0X25
#define TIME_INACT		0X26
#define ACT_INACT_CTL	0X27
#define THRESH_FF		0X28
#define TIME_FF			0X29
#define TAP_AXES		0X2A
#define ACT_TAP_STATUS  0X2B
#define BW_RATE			0X2C
#define POWER_CTL		0X2D

#define INT_ENABLE		0X2E
#define INT_MAP			0X2F
#define INT_SOURCE  	0X30
#define DATA_FORMAT	    0X31
#define DATA_X0			0X32
#define DATA_X1			0X33
#define DATA_Y0			0X34
#define DATA_Y1			0X35
#define DATA_Z0			0X36
#define DATA_Z1			0X37
#define FIFO_CTL		0X38
#define FIFO_STATUS		0X39

static struct i2c_device device;
extern void DelayMs(uint32_t ms);
/* ADXL345 2 possible addr */
static const uint8_t adxl345_addr[] = {0x53, 0x1D};


#define ADXL345_DEBUG		0
#if ( ADXL345_DEBUG == 1 )
#define ADXL345_TRACE	printf
#else
#define ADXL345_TRACE(...)
#endif

int adxl345_init(struct i2c_bus* bus)
{
    uint32_t ret;
    device.bus = bus;
    device.config.baudrate = 100*1000;
    device.config.data_width = 8;
    device.config.mode = 0;
    device.subaddr_len = 1;
    ret = i2c_bus_attach_device(bus, &device);
    return ret;
}

static int adxl345_write_register(uint8_t addr, uint8_t value)
{
    
    uint8_t buf[1];
    device.subaddr = addr;
    buf[0] = value;
    if(device.bus->ops->write(&device, buf, 1))
    {
        return 1;
    }
    return 0;
}

int adxl345_get_addr(void)
{
    return device.chip_addr;
}

int adxl345_probe(void)
{
    uint32_t i;
    uint8_t buf[3];
    device.subaddr = DEVICE_ID;
    for(i=0;i<ARRAY_SIZE(adxl345_addr);i++)
    {
        device.chip_addr = adxl345_addr[i];
        if(device.bus->ops->read(&device, buf, 1) == I2C_EOK)
        {
            /* find the device */
            device.chip_addr = adxl345_addr[i];
            /* ID match */
            if(buf[0] == 0XE5)
            {
                /* init sequence */
                /*低电平中断输出,13位全分辨率,输出数据右对齐,16g量程  */
                adxl345_write_register(DATA_FORMAT, 0x2B);
                /*数据输出速度为100Hz */
                adxl345_write_register(BW_RATE, 0x0A);
                /*链接使能,测量模式 */
                adxl345_write_register(POWER_CTL, 0x28);
                /*不使用中断 */
                adxl345_write_register(INT_ENABLE, 0x00);
                buf[0] = 0x00;
                buf[1] = 0x00;
                buf[2] = 0x00;
                device.subaddr = OFSX;
                device.bus->ops->write(&device, buf, 3);
                return 0; 
            }
        }
    }
    return 1; 
}

int adxl345_readXYZ(short *x, short *y, short *z)
{
    uint8_t buf[6];
    device.subaddr = DATA_X0;
    if(device.bus->ops->read(&device, buf, 6))
    {
        return 1;
    }
    *x=(short)(((uint16_t)buf[1]<<8)+buf[0]); 	    
    *y=(short)(((uint16_t)buf[3]<<8)+buf[2]); 	    
    *z=(short)(((uint16_t)buf[5]<<8)+buf[4]); 
    return 0;
}

int adxl345_calibration(void)
{
	short tx,ty,tz;
	uint8_t i;
	short offx=0,offy=0,offz=0; 
    adxl345_write_register(POWER_CTL, 0x00);//先进入休眠模式.
	DelayMs(40);
	adxl345_write_register(DATA_FORMAT, 0X2B);	//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
	adxl345_write_register(BW_RATE, 0x0A);		//数据输出速度为100Hz
	adxl345_write_register(POWER_CTL, 0x28);	   	//链接使能,测量模式
	adxl345_write_register(INT_ENABLE, 0x00);	//不使用中断
 //   adxl345_write_register(device, FIFO_CTL, 0x9F); //开启FIFO
	DelayMs(12);
	for(i=0;i<10;i++)
	{
		adxl345_readXYZ(&tx, &ty, &tz);
        DelayMs(10);
		offx += tx;
		offy += ty;
		offz += tz;
	}	 	
	offx /= 10;
	offy /= 10;
	offz /= 10;
	offx = -offx/4;
	offy = -offy/4;
	offz = -(offz-256)/4;	
    ADXL345_TRACE("OFFX:%d OFFY:%d OFFZ:%d \r\n" ,offx, offy, offz);
 	adxl345_write_register(OFSX, offx);
	adxl345_write_register(OFSY, offy);
	adxl345_write_register(OFSZ, offz);
    return 0; 
}

//得到角度
//x,y,z:x,y,z方向的重力加速度分量(不需要单位,直接数值即可)
//dir:要获得的角度.0,与Z轴的角度;1,与X轴的角度;2,与Y轴的角度.
//返回值:角度值.单位0.1°.
short adxl345_convert_angle(short x, short y, short z, short *ax, short *ay, short *az)
{
	float temp;
    float fx,fy,fz;
    fx = (float)x; fy = (float)y;  fz = (float)z; 

    temp = sqrt((fx*fx+fy*fy))/fz;
    *az = atan(temp)*1800/3.14;
    temp = fx/sqrt((fy*fy+fz*fz));
    *ax = atan(temp)*1800/3.14;;
    temp = fy/sqrt((fx*fx+fz*fz));
    *ay = atan(temp)*1800/3.14;;
	return 0;
}
