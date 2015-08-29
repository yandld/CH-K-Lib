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
#include "mq.h"

#define     VERSION     221

enum 
{
    kMSG_CMD_TIMER,
    kMSG_CMD_SENSOR_DATA_READY,
    kMSG_CMD_DATA_REV,
};

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
    len = ano_encode_packet(&payload, buf);
    
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
    printf("Uranus %d\r\n", VERSION);
    printf("CoreClock:%dHz\r\n", GetClock(kCoreClock));
    printf("UID:0x%X\r\n", GetUID());
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
    int i;
    int RunState;
    struct dcal_t dcal;
    int16_t adata[3], gdata[3], mdata[3];
    int16_t radata[3], rgdata[3], rmdata[3];
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
    mq_init();
    veep_init();
    sensor_init();

    ares  = mpu9250_get_ares();
    gres  = mpu9250_get_gres();
    mres  = mpu9250_get_mres();
    
    veep_read((uint8_t*)&dcal, sizeof(struct dcal_t));
    dcal_init(&dcal);
    dcal_print(&dcal);
    printf("cal data read %s", (dcal.magic == 0x5ACB)?("ok!\r\n"):("err!\r\n"));

    PIT_Init();
    PIT_SetTime(0, 50*1000);
    PIT_SetIntMode(0, true);

    PIT_SetTime(1, 1000*1000);

    static uint32_t time, load_val, fac_us;
    fac_us = GetClock(kBusClock);
    fac_us /= 1000000;

    RunState = kPTL_REQ_MODE_9AXIS;
    
    while(1)
    {
        if(mq_exist())
        {
            msg_t* pMsg;
            pMsg = mq_pop();
            switch(pMsg->cmd)
            {
                /* timer event */
                case kMSG_CMD_TIMER:
                    if(RunState == kPTL_REQ_MODE_CAL)
                    {
                        dcal_minput(&dcal, rmdata);
                        dcal_output(&dcal);
                        GPIO_PinToggle(HW_GPIOC, 3);
                    }
                    /* bmp read */
                    ret = bmp180_conversion_process(&dummy, &temperature);
                    if(!ret)
                    {
                        pressure = dummy;
                    }
                    break;
                    
                 /* IMU sensor data ready */
                case kMSG_CMD_SENSOR_DATA_READY:
                    mpu9250_read_accel_raw(radata);
                    mpu9250_read_gyro_raw(rgdata);
                    ret = mpu9250_read_mag_raw(rmdata);
                    if(ret)
                    {
                        break;
                    }

                    for(i=0;i<3;i++)
                    {
                        adata[i] = radata[i];
                        gdata[i] = rgdata[i];
                        mdata[i] = rmdata[i];
                        
                        adata[i] = adata[i] - dcal.ao[i];
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

                    for(i=0;i<3;i++)
                    {
                        adata[i] = (int16_t)(fadata[i]*1000);
                        //gdata[i] = (int16_t)(fgdata[i]);
                        mdata[i] = (int16_t)(fmdata[i]);
                    }

                    if(RunState == kPTL_REQ_MODE_9AXIS || RunState == kPTL_REQ_MODE_6AXIS)
                    {
                        GPIO_PinToggle(HW_GPIOC, 3);
                        send_data_process(&angle, adata, gdata, mdata, (int32_t)pressure);   
                    }

                    break;
                /* data reviecved from PC */
                case kMSG_CMD_DATA_REV:
                    int len, i;
                    static uint8_t buf[64];
                    len = 0;
                    switch(pMsg->type)
                    {
                        case kPTL_REQ_FW:
                        {
                            fw_info_t fwinfo;
                            fwinfo.version = VERSION;
                            fwinfo.uid = GetUID();
                            len = ano_encode_fwinfo(&fwinfo, buf);
                            break;
                        }
                        /* send all cal data to PC */
                        case kPTL_REQ_OFS_ALL:
                        {
                            offset_t offset;
                            for(i=0; i<3; i++)
                            {
                                offset.acc_offset[i] = dcal.ao[i];
                                offset.gyro_offset[i] = dcal.go[i];
                                offset.mag_offset[i] = dcal.mo[i];
                            }
                            len = ano_encode_offset_packet(&offset, buf);
                            break;
                        }
                        /* get all cal data and program to flash */
                        case kPTL_DATA_OFS_ALL:
                            {
                                rev_data_t* rd = (rev_data_t*)pMsg->msg;
                                
                                dcal.ao[0] = (rd->buf[0]<<8) + (rd->buf[1]<<0);
                                dcal.ao[1] = (rd->buf[2]<<8) + (rd->buf[3]<<0);
                                dcal.ao[2] = (rd->buf[4]<<8) + (rd->buf[5]<<0);
                                dcal.go[0] = (rd->buf[6]<<8) + (rd->buf[7]<<0);
                                dcal.go[1] = (rd->buf[8]<<8) + (rd->buf[9]<<0);
                                dcal.go[2] = (rd->buf[10]<<8) + (rd->buf[11]<<0);
                                dcal.mo[0] = (rd->buf[12]<<8) + (rd->buf[13]<<0);
                                dcal.mo[1] = (rd->buf[14]<<8) + (rd->buf[15]<<0);
                                dcal.mo[2] = (rd->buf[16]<<8) + (rd->buf[17]<<0);
                                break;
                            }
                        case kPTL_REQ_SAVE_OFS:
                            veep_write((uint8_t*)&dcal, sizeof(struct dcal_t));
                            break;
                        case kPTL_REQ_MODE_6AXIS:
                        case kPTL_REQ_MODE_9AXIS:
                            RunState = pMsg->type;
                            break;
                        case kPTL_REQ_MODE_CAL:
                            dcal_reset_mag(&dcal);
                            RunState = pMsg->type;
                            break;
                    }
                    
                    while(UART_DMAGetRemain(HW_UART0) != 0);
                    for(i=0; i<len; i++)
                    {
                        UART_PutChar(HW_UART0, buf[i]);
                    }
                    break;


            }
        }
    }
}

void UART0_IRQHandler(void)
{
    uint8_t ch;
    static rev_data_t rd, rd_cp;
    if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART_C2_RIE_MASK))
    {
        ch = UART0->D;
        if(ano_rec(ch, &rd) == 0)
        {
            rd_cp = rd;
            msg_t msg;
            msg.cmd = kMSG_CMD_DATA_REV;
            msg.type = rd_cp.cmd;
            msg.msg_len = rd_cp.len;
            msg.msg = &rd_cp;
            mq_push(msg);
        }
    }
}

void PIT_IRQHandler(void)
{
    if(PIT->CHANNEL[0].TFLG)
    {
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
        msg_t msg;
        msg.cmd = kMSG_CMD_TIMER;
        mq_push(msg);
    }
}

void PORTA_IRQHandler(void)
{
    PORTA->ISFR |= (1<<18);
    msg_t msg;
    msg.cmd = kMSG_CMD_SENSOR_DATA_READY;
    mq_push(msg);
}
