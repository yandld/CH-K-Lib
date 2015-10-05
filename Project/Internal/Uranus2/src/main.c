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

#define VERSION             (202)
#define MPU9250_INT_PIN     (18)

#define DMA_TX_CH      (HW_DMA_CH0)
#define DMA_RX_CH      (HW_DMA_CH1)

KalmanState_t KState[3];
static uint8_t gRevBuf[64];
struct dcal_t dcal;
int RunState;
    
enum 
{
    kMSG_CMD_TIMER,
    kMSG_CMD_SENSOR_DATA_READY,
    kMSG_CMD_DATA_REV,
};

static uint32_t encode_data_packet(uint8_t *buf, attitude_t *angle, int16_t *adata, int16_t *gdata, int16_t *mdata, int32_t pressure)
{
    int i;
    static payload_t payload;
    
    for(i=0;i<3;i++)
    {
        payload.acc[i] = adata[i];
        payload.gyo[i] = gdata[i];
        payload.mag[i] = mdata[i];
    }
    payload.P = (int16_t)(angle->P*100);
    payload.R = (int16_t)(angle->R*100);
    payload.Y = (180 + (angle->Y))*10;
    payload.pressure = pressure;
    /* set buffer */
    return ano_encode_packet(&payload, buf);
}

int sensor_init(void)
{
    uint32_t ret;

    I2C_Init(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    DelayMs(15);
    ret = mpu9250_init(0);
    if(ret)
    {
        printf("mpu9250 init:%d\r\n", ret);
        printf("restarting...\r\n");
        DelayMs(300);
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
    switch(RunState)
    {
        case kPTL_REQ_MODE_9AXIS:
            printf("9 AXIS mode\r\n");
            break;
        case kPTL_REQ_MODE_6AXIS:
            printf("6 AXIS mode\r\n");
            break;
        default:
            printf("RunState:0x%X\r\n", RunState);
            break;
    }
    dcal_print(&dcal);
}

//#define FALL_LIMIT      100
//uint32_t FallDetection(int16_t *adata)
//{
//    uint32_t ret;
//    
//    ret = 0;
//    if((abs(adata[0])< FALL_LIMIT) && (abs(adata[1]) < FALL_LIMIT) && (abs(adata[2]) < FALL_LIMIT))
//    {
//        ret = 1;
//    }
//    return ret;
//}

//uint32_t FallDetectionG(int16_t *gdata)
//{
//    uint32_t sum, ret;
//    ret = 0;
//    
//    sum = gdata[0]*gdata[0] + gdata[1]*gdata[1] + gdata[2]*gdata[2];
//    if(sum > 500000)
//    {
//        ret = 1;
//    }

//    return ret;
//}

void HWInit(void)
{

    DelayInit();
    GPIO_Init(HW_GPIOC, PIN3, kGPIO_Mode_OPP);
    GPIO_Init(HW_GPIOA, MPU9250_INT_PIN, kGPIO_Mode_IFT);
    GPIO_SetIntMode(HW_GPIOA, MPU9250_INT_PIN, kGPIO_Int_RE, true);
    
    UART_Init(UART0_RX_PA01_TX_PA02, 115200);
    UART_SetDMAMode(HW_UART0, kUART_DMARx, true);
    DMA_Init_t Init;
    Init.chl = DMA_RX_CH;
    Init.chlTrigSrc = UART0_REV_DMAREQ;
    Init.trigSrcMod = kDMA_TrigSrc_Normal;
    Init.transCnt = 9999;

    Init.sAddr = (uint32_t)&UART0->D;
    Init.sAddrIsInc = false;
    Init.sDataWidth = kDMA_DataWidthBit_8;
    Init.sMod = kDMA_ModuloDisable;

    Init.dAddr = (uint32_t)gRevBuf;
    Init.dAddrIsInc = true;
    Init.dDataWidth = kDMA_DataWidthBit_8;
    Init.dMod = kDMA_ModuloDisable;
    DMA_Init(&Init);

    DMA_EnableReq(DMA_RX_CH);
    
    veep_init();
    mq_init();
    veep_read((uint8_t*)&dcal, sizeof(struct dcal_t));
    sensor_init();
    UART_SetIntMode(HW_UART0, kUART_IntIdleLine, true);
}


void ano_callback(rev_data_t *rd);
uint32_t DataRevDecode(msg_t *pMsg, uint8_t *buf);

int main(void)
{
#if defined(BOOTLOADER)
    SCB->VTOR = 0x5000;
#endif
    int i;

    int16_t adata[3], gdata[3], mdata[3];
    int16_t radata[3], rgdata[3], rmdata[3];
    float fadata[3], fgdata[3], fmdata[3];
    static attitude_t angle;
    uint32_t ret;
    float pressure, dummy, temperature;
    float ares, gres, mres;
    uint32_t len;
    static uint8_t buf[64];
    
    HWInit();

    ares  = mpu9250_get_ares();
    gres  = mpu9250_get_gres();
    mres  = mpu9250_get_mres();
    
    ano_set_callback(ano_callback);

    PIT_Init();
    PIT_SetTime(0, 50*1000);
    PIT_SetIntMode(0, true);

    PIT_SetTime(1, 1000*1000);

    static uint32_t time, load_val, fac_us;
    fac_us = GetClock(kBusClock);
    fac_us /= 1000000;

    RunState = dcal.mode;
    if(RunState == kPTL_REQ_MODE_9AXIS)
    {
        RunState = kPTL_REQ_MODE_9AXIS;
    }
    else
    {
        RunState = kPTL_REQ_MODE_6AXIS;
    }
    ShowInfo();
    
    KalmanSimple1D(&KState[0], 1, 10);
    KalmanSimple1D(&KState[1], 1, 10);
    KalmanSimple1D(&KState[2], 1, 10);
    
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
                    
                    for(i=0; i<3; i++)
                    {
                        adata[i] = radata[i];
                        gdata[i] = rgdata[i];
                        mdata[i] = rmdata[i];
                        
                        adata[i] = adata[i] - dcal.ao[i];
                        gdata[i] = gdata[i] - dcal.go[i];
                        mdata[i] = (mdata[i] - dcal.mo[i])/dcal.mg[i];
                        if(RunState == kPTL_REQ_MODE_6AXIS)
                        {
                            mdata[i] = 0;
                        }
                    }

                    /* set timer */
                    time = PIT_GetCnt(HW_PIT_CH1);
                    PIT_SetCnt(HW_PIT_CH1, 1000*1000);
                    load_val = PIT_GetCnt(HW_PIT_CH1);
                    time = load_val - time;
                    time /= fac_us;

                    /* low pass filter */
                    for(i=0;i<3;i++)
                    {
                        fadata[i] = (float)adata[i]*ares;
                        fgdata[i] = ((float)gdata[i])*gres;
                        fmdata[i] = (float)mdata[i]*mres;
                    }
                    
                    ret = imu_get_euler_angle(fadata, fgdata, fmdata, &angle);
                    halfT = ((float)time)/1000/2000;
                    
                    KalmanRun(&KState[0], angle.P);
                    angle.P = KState[0].State;
    
                    KalmanRun(&KState[1], angle.R);
                    angle.R = KState[1].State;
                        
                    for(i=0;i<3;i++)
                    {
                        adata[i] = (adata[i]*ares*1000);
                    }
                    
                    len = encode_data_packet(buf, &angle, adata, gdata, rmdata, (int32_t)pressure);
                    
                    if(RunState != kPTL_REQ_MODE_CAL)
                    {
                        GPIO_PinToggle(HW_GPIOC, 3);
                        if(UART_DMAGetRemain(HW_UART0) == 0)
                        {
                            UART_DMASend(HW_UART0, DMA_TX_CH, buf, len);
                        }
                    }
                break;
                /* data reviecved from PC */
                case kMSG_CMD_DATA_REV:
                {
                    len = DataRevDecode(pMsg, buf);
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
}

uint32_t DataRevDecode(msg_t *pMsg, uint8_t *buf)
{
    int len, i;
    len = 0;
    switch(pMsg->type)
    {
        case kPTL_REQ_FW:
        {
            fw_info_t fwinfo;
            fwinfo.version = VERSION;
            fwinfo.uid = GetUID();
            fwinfo.id = dcal.id;
            fwinfo.mode = RunState;
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
            dcal.ao[0] = (pMsg->payload[0]<<8) + (pMsg->payload[1]<<0);
            dcal.ao[1] = (pMsg->payload[2]<<8) + (pMsg->payload[3]<<0);
            dcal.ao[2] = (pMsg->payload[4]<<8) + (pMsg->payload[5]<<0);
            dcal.go[0] = (pMsg->payload[6]<<8) + (pMsg->payload[7]<<0);
            dcal.go[1] = (pMsg->payload[8]<<8) + (pMsg->payload[9]<<0);
            dcal.go[2] = (pMsg->payload[10]<<8) + (pMsg->payload[11]<<0);
            dcal.mo[0] = (pMsg->payload[12]<<8) + (pMsg->payload[13]<<0);
            dcal.mo[1] = (pMsg->payload[14]<<8) + (pMsg->payload[15]<<0);
            dcal.mo[2] = (pMsg->payload[16]<<8) + (pMsg->payload[17]<<0);
            break;
        }
        case kPTL_REQ_SAVE_OFS:
            dcal.magic = 0x5ACB;
            veep_write((uint8_t*)&dcal, sizeof(struct dcal_t));
            break;
        case kPTL_REQ_MODE_6AXIS:
        case kPTL_REQ_MODE_9AXIS:
            RunState = pMsg->type;
            dcal.mode = RunState;
            break;
        case kPTL_REQ_MODE_CAL:
            dcal_reset_mag(&dcal);
            RunState = pMsg->type;
            break;
    }
    return len;
}

void ano_callback(rev_data_t *rd)
{
    msg_t msg;
    msg.cmd = kMSG_CMD_DATA_REV;
    msg.type = rd->cmd;
    msg.msg_len = rd->len;
    memcpy(msg.payload, rd->buf, rd->len);
    msg.msg = msg.payload;
    mq_push(msg);
}

void UART0_IRQHandler(void)
{
    static uint8_t buf[64];
    uint32_t len;
    static rev_data_t rd;
    if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART_C2_RIE_MASK))
    {
       // gRevBuf[i++] = UART0->D;
    }
    
    if(UART0->S1 & UART_S1_IDLE_MASK)
    {
        UART0->S1 |=  UART_S1_IDLE_MASK;
        
        len = 9999 - DMA_GetTransCnt(DMA_RX_CH);
        memcpy(buf, gRevBuf, len);
        DMA_SetDestAddr(DMA_RX_CH, (uint32_t)gRevBuf);
        DMA_SetTransCnt(DMA_RX_CH, 9999);
        ano_rec(&rd, buf, len);
    }
    
    /**
        this is very important because in real applications
        OR usually occers, the RDRF will block when OR is asserted 
    */
    if(UART0->S1 & UART_S1_OR_MASK)
    {
      //  ch = UART0->D;
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
    PORTA->ISFR |= (1<<MPU9250_INT_PIN);
    msg_t msg;
    msg.cmd = kMSG_CMD_SENSOR_DATA_READY;
    mq_push(msg);
}
