#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
#include "filter.h"
#include "calibration.h"


#define     VERSION_STRING          "V2.2.0"


static bool FLAG_TIMER;
static bool RAW_DATA_RDY_FLAG;
static bool is_cal_data_ok;
struct dcal_t dcal;

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
    
    if(UART_DMAGetRemain(HW_UART0) == 0)
    {
        UART_DMASend(HW_UART0, 0, buf, len);
    }
}

int sensor_init(void)
{
    uint32_t ret;

    I2C_Init(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    DelayMs(10);
    ret = mpu9250_init(0);
    if(ret)
    {
        printf("mpu9250 init:%d\r\n", ret);
        printf("restarting...\r\n");
        DelayMs(500);
        SystemSoftReset();
    }
    
    ret = bmp180_init(0);
    if(ret)
    {
        printf("bmp 180 init failed:%d\r\n", ret);
    }
    
    struct mpu_config config;
    
    config.afs = AFS_16G;
    config.gfs = GFS_2000DPS;
    config.mfs = MFS_14BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);
    mpu9250_enable_raw_data_int();
    return ret;
}

static void ShowInfo(void)
{
    printf("Uranus %s\r\n", VERSION_STRING);
    printf("CoreClock:%dHz\r\n", GetClock(kCoreClock));
}

#define FALL_LIMIT      100
uint32_t FallDetection(int16_t *adata)
{
    uint32_t ret;
    
    ret = 0;
    if((abs(adata[0])< FALL_LIMIT) && (abs(adata[1]) < FALL_LIMIT) && (abs(adata[2]) < FALL_LIMIT))
    {
        ret = 1;
    }
    return ret;
}

uint32_t FallDetectionG(int16_t *gdata)
{
    uint32_t sum, ret;
    ret = 0;
    
    sum = gdata[0]*gdata[0] + gdata[1]*gdata[1] + gdata[2]*gdata[2];
    if(sum > 500000)
    {
        ret = 1;
    }

    return ret;
}


int main(void)
{
    int i,j;
    int16_t adata[3], gdata[3], mdata[3], cp_mdata[3];
    uint32_t fall;
    float fadata[3], fgdata[3], fmdata[3];
    static attitude_t angle;
    uint32_t ret;
    float pressure, dummy, temperature;
    float ares, gres, mres;
    DelayInit();
    GPIO_Init(HW_GPIOC, PIN3, kGPIO_Mode_OPP);
    GPIO_Init(HW_GPIOA, 18, kGPIO_Mode_IFT);
    GPIO_SetIntMode(HW_GPIOA, 18, kGPIO_Int_RE, true);
    
    UART_Init(UART0_RX_PA01_TX_PA02, 115200);
    UART_SetIntMode(HW_UART0, kUART_IntRx, true);
    ShowInfo();
    veep_init();
    sensor_init();

    ares  = mpu9250_get_ares();
    gres  = mpu9250_get_gres();
    mres  = mpu9250_get_mres();
    
    veep_read((uint8_t*)&dcal, sizeof(struct dcal_t));
    if(dcal.magic == 0x5ACB)
    {
        printf("cal data read ok!\r\n");
        dcal_print(&dcal);
        is_cal_data_ok = true;
    }
    else
    {
        printf("cal data read err!\r\n");
        is_cal_data_ok = false;
        dcal_init(&dcal);
    }
    
    PIT_Init();
    PIT_SetTime(0, 50*1000);
    PIT_SetIntMode(0, true);

    PIT_SetTime(1, 1000*1000);


    static uint32_t time, load_val, fac_us;
    fac_us = GetClock(kBusClock);
    fac_us /= 1000000;
   

    while(1)
    {
        uint8_t val;
 //       val = mpu9250_get_int_status();
        if(RAW_DATA_RDY_FLAG)
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
                gdata[i] = gdata[i] - dcal.go[i];
                mdata[i] = (mdata[i] - dcal.mo[i])/dcal.mg[i];
            }

            /* set timer */
            time = PIT_GetCnt(HW_PIT_CH1);
            PIT_SetCnt(HW_PIT_CH1, 1000*1000);
            load_val = PIT_GetCnt(HW_PIT_CH1);
            time = load_val - time;
            time /= fac_us;

            /* low pass filter */
            float factor[3];
            factor[2] = lpf_1st_factor_cal(halfT*2, 10);
            float real_mag[3];
            for(i=0;i<3;i++)
            {
                fadata[i] = ((float)adata[i])*ares;
                fgdata[i] = ((float)gdata[i])*gres;
                fmdata[i] = lpf_1st(fmdata[i], (float)mdata[i], factor[2]);
                real_mag[i] = fmdata[i]*mres;
                //fmdata[i] = 0;
            }
            
            ret = imu_get_euler_angle(fadata, fgdata, real_mag, &angle);
            
            halfT = ((float)time)/1000/2000;
            if(FLAG_TIMER)
            {
                /* dcal process */
                if(is_cal_data_ok == false)
                {
                    dcal_minput(cp_mdata);
                    dcal_output(&dcal);  
                }
                
                /* bmp read */
                ret = bmp180_conversion_process(&dummy, &temperature);
                if(!ret)
                {
                    pressure = dummy;
                }
                
                GPIO_PinToggle(HW_GPIOC, 3);
                FLAG_TIMER = false;
            }
            
            for(i=0;i<3;i++)
            {
                adata[i] = (int16_t)(fadata[i]*1000);
                gdata[i] = (int16_t)(fgdata[i]);
                mdata[i] = (int16_t)(fmdata[i]);
            }
            fall = FallDetectionG(gdata);
            //GPIO_PinWrite(HW_GPIOC, 3, fall);
            
            send_data_process(&angle, adata, gdata, mdata, (int32_t)pressure);
            RAW_DATA_RDY_FLAG = false;
            EnterWaitMode(false);
        }
    }
}

void UART0_IRQHandler(void)
{
    uint8_t data;
    static rev_data_t rd;
    if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART_C2_RIE_MASK))
    {
        UART_GetChar(HW_UART0, &data);
        if(!ano_rec((uint8_t)data, &rd))
        {
            if(rd.buf[0] == 0xAA)
            {
                veep_write((uint8_t*)&dcal, sizeof(struct dcal_t));
                printf("cal data write ok!\r\n");
                DelayMs(100);
            }
        }
    } 

}

void PIT_IRQHandler(void)
{
    if(PIT->CHANNEL[0].TFLG)
    {
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
        FLAG_TIMER = true;
    }
}

void PORTA_IRQHandler(void)
{
    PORTA->ISFR |= (1<<18);
    RAW_DATA_RDY_FLAG = true;
}
