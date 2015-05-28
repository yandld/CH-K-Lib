#include <string.h>

#include "common.h"
#include "dma.h"
#include "pit.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "flash.h"

#include "protocol.h"
#include "mpu9250.h"
#include "bmp180.h"
#include "virtual_eep.h"
#include "imu.h"
#include "trans.h"
#include "filter.h"
#include "calibration.h"

#include "rl_usb.h"

#define     VERSION_MAJOR       (2)
#define     VERSION_MINOR       (0)


static bool FLAG_TIMER;


static void send_data_process(attitude_t *angle, int16_t *adata, int16_t *gdata, int16_t *mdata, int32_t pressure)
{
    int i;
    static uint8_t buf[64];
    uint32_t len;
    static payload_t payload;
    
    for(i=0;i<3;i++)
    {
        payload.acc[i] = adata[i];
        payload.gyo[i] = gdata[i];
        payload.mag[i] = mdata[i];
    }
    payload.P = (int16_t)(angle->P*100);
    payload.R = (int16_t)(angle->R*100);
    payload.Y = 1800 + (int16_t)(angle->Y*10);
    payload.pressure = pressure;
    /* set buffer */
    len = ano_encode(&payload, buf);
    uart_dma_send(buf, len);

}

int sensor_init(void)
{
    uint32_t ret;

    I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    DelayMs(50);
    ret = mpu9250_init(0);
    if(ret)
    {
        printf("mpu9250 init:%d\r\n", ret);
        printf("restarting...\r\n");
        DelayMs(200);
    //    SystemSoftReset();
    }
    
    ret = bmp180_init(0);
    if(ret)
    {
        printf("bmp 180 init failed:%d\r\n", ret);
    }
    
    struct mpu_config config;
    
    config.afs = AFS_8G;
    config.gfs = GFS_1000DPS;
    config.mfs = MFS_16BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);
    
    return ret;
}

static void ShowInfo(void)
{
    uint32_t clock;
    printf("VERSION%d.%d\r\n", VERSION_MAJOR, VERSION_MINOR);
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%dHz\r\n", clock);
}



void PIT_ISR(void)
{
    FLAG_TIMER = true;
}

extern void UART_ISR(uint16_t data);



#define UDISK_SIZE          (64*1024)
#define FLASH_OFFSET        (50*1024)   /* UDisk start offset */

U8 BlockBuf[1024];

/* init */
void usbd_msc_init ()
{
  USBD_MSC_MemorySize = UDISK_SIZE;
  USBD_MSC_BlockSize  = FLASH_GetSectorSize();
  USBD_MSC_BlockGroup = 1;
  USBD_MSC_BlockCount = USBD_MSC_MemorySize / USBD_MSC_BlockSize;
  USBD_MSC_BlockBuf   = BlockBuf;

  USBD_MSC_MediaReady = __TRUE;

}

/* read */
void usbd_msc_read_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    uint8_t *p;
    uint8_t i;
    
    p = (uint8_t*)(FLASH_OFFSET + block * USBD_MSC_BlockSize);
    
    if (USBD_MSC_MediaReady)
    {
        for(i=0;i<num_of_blocks;i++)
        {
            memcpy(buf, p, num_of_blocks * USBD_MSC_BlockSize);
            p += USBD_MSC_BlockSize;
        }
    }
}

/* write */
void usbd_msc_write_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    int i,j;
    uint8_t *p;
    
    p = (uint8_t*)(FLASH_OFFSET + block * USBD_MSC_BlockSize);
    
    if (USBD_MSC_MediaReady)
    {
        for(i=0;i<num_of_blocks;i++)
        {
            FLASH_EraseSector((uint32_t)p);
            FLASH_WriteSector((uint32_t)p, buf, FLASH_GetSectorSize());
        }
    }
}


int main(void)
{
    int i;
    int16_t adata[3], gdata[3], mdata[3], cp_mdata[3];
    static float fadata[3], fgdata[3], fmdata[3];
    attitude_t angle;
    struct dcal_t dcal;
    uint32_t ret;
    uint32_t uart_instance;
    int32_t pressure, dummy, temperature;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    uart_instance = UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    UART_CallbackRxInstall(uart_instance, UART_ISR);
    UART_ITDMAConfig(uart_instance, kUART_IT_Rx, true);
    
    
    ShowInfo();
//    veep_init();
//   // GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
//   // GPIO_QuickInit(HW_GPIOE, 1, kGPIO_Mode_OPP);
//    
//    sensor_init();
//    veep_read((uint8_t*)&dcal, sizeof(struct dcal_t));

//    PIT_QuickInit(HW_PIT_CH0, 100*1000);
//    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
//    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
//    
//    PIT_QuickInit(HW_PIT_CH1, 1000*1000);
//    
    static uint32_t time, load_val, fac_us;
    CLOCK_GetClockFrequency(kBusClock, &fac_us);
    fac_us /= 1000000;
//   
//    dcal_init(&dcal);
//    dcal_print(&dcal);
//    
//    uart_dma_init(HW_DMA_CH1, uart_instance);
    
    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */
    while (!usbd_configured ());          /* Wait for device to configure        */
    
    while(1)
    {
        /* raw data and offset balance */
        mpu9250_read_accel_raw(adata);
        mpu9250_read_gyro_raw(gdata);
        mpu9250_read_mag_raw(mdata);
        cp_mdata[0] = mdata[0];
        cp_mdata[1] = mdata[1];
        cp_mdata[2] = mdata[2];
        
        for(i=0;i<3;i++)
        {
            mdata[i] = dcal.mg[i]*(mdata[i] - dcal.mo[i]);
        }

        
        /* set timer */
        time = PIT_GetCounterValue(HW_PIT_CH1);
        PIT_ResetCounter(HW_PIT_CH1);
        load_val = PIT_GetCounterValue(HW_PIT_CH1);
        time = load_val - time;
        time /= fac_us;

        /* low pass filter */
        float factor[3];
        factor[0] = lpf_1st_factor_cal(halfT*2, 92);
        factor[1] = lpf_1st_factor_cal(halfT*2, 92);
        factor[2] = lpf_1st_factor_cal(halfT*2, 5);
        for(i=0;i<3;i++)
        {
            fadata[i] = lpf_1st(fadata[i], (float)adata[i], factor[0]);
            fgdata[i] = lpf_1st(fgdata[i], (float)gdata[i], factor[1]);
            fmdata[i] = lpf_1st(fmdata[i], (float)mdata[i], factor[2]);
        }

        /* IMU ipdate */
        ret = imu_get_euler_angle(fadata, fgdata, fmdata, &angle);
        halfT = ((float)time)/1000/2000;

        /* timer task */
        if(FLAG_TIMER)
        {
            /* dcal process */
            dcal_minput(cp_mdata);
            dcal_output(&dcal);
            if(dcal.need_update)
            {
                veep_write((uint8_t*)&dcal, sizeof(struct dcal_t));
            }
            
            /* bmp read */
            ret = bmp180_conversion_process(&dummy, &temperature);
            if(!ret)
            {
                pressure = dummy;
            }
            
            GPIO_ToggleBit(HW_GPIOA, 1);
            FLAG_TIMER = false;
        }
        send_data_process(&angle, adata, gdata, cp_mdata, pressure);
    }
}

void UART_ISR(uint16_t data)
{
    static rev_data_t rd;

    if(!ano_rec((uint8_t)data, &rd))
    {
        if(rd.buf[0] == 0xAA)
        {
            printf("write calibration data!...\r\n");
          //  veep_write((uint8_t*)&dcal, sizeof(struct dcal_t));
        }
    }
}

//void mpu9250_test(void)
//{
//    mpu9250_init(0);
//    struct mpu_config config;
//    
//    config.afs = AFS_8G;
//    config.gfs = GFS_1000DPS;
//    config.mfs = MFS_14BITS;
//    config.aenable_self_test = false;
//    config.genable_self_test = false;
//    mpu9250_config(&config);
//    
//    uint8_t err;
//    int16_t mdata[3], gdata[3], adata[3];
//    while(1)
//    {
//        err = 0;
//    
//        err += mpu9250_read_accel_raw(adata);
//        err += mpu9250_read_gyro_raw(gdata);
//        err += mpu9250_read_mag_raw(mdata);
//    
//        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d    \r", adata[0] ,adata[1], adata[2], gdata[0], gdata[1], gdata[2], mdata[0], mdata[1], mdata[2]);  
//		GPIO_ToggleBit(HW_GPIOC, 3);
//        DelayMs(5);
//    }
//}
