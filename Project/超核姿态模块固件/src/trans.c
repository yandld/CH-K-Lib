#include "trans.h"
#include <string.h>
#include "dma.h"

//数据头
const char trans_header_table[3] = {0x88, 0xAF, 0x1C};
//数据内容
typedef __packed struct
{
    uint8_t trans_header[3];
    trans_user_data_t user_data;
    uint8_t trans_reserved[4];
    uint8_t sum;
}trans_packet_t;

trans_packet_t packet;

//安装回调函数
uint8_t trans_init(void)
{
    
    DMA_InitTypeDef DMA_InitStruct1;
    DMA_InitStruct1.chl = HW_DMA_CH1;
    DMA_InitStruct1.chlTriggerSource = UART1_TRAN_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorByteTransferCount = 1;
    DMA_InitStruct1.majorTransferCount = sizeof(packet);
        
    DMA_InitStruct1.sourceAddress = (uint32_t)&packet;
    DMA_InitStruct1.sourceAddressMajorAdj = -sizeof(packet);
    DMA_InitStruct1.sourceAddressMinorAdj = 1;
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8;
        
    DMA_InitStruct1.destAddress = (uint32_t)&UART1->D;
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 0;
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
    DMA_Init(&DMA_InitStruct1);
    return 0;
}


//发送数据包
uint32_t trans_send_pactket(trans_user_data_t data)
{
    uint8_t i;
    char *p = (char*)&packet;
    uint8_t sum = 0;
    memcpy(packet.trans_header, trans_header_table, sizeof(trans_header_table));
    for(i=0;i<3;i++)
    {
        data.trans_accel[i] = __REV16(data.trans_accel[i]);
        data.trans_gyro[i] = __REV16(data.trans_gyro[i]);
        data.trans_mag[i] = __REV16(data.trans_mag[i]);
    }
    data.trans_yaw = __REV16(data.trans_yaw);
    data.trans_pitch = __REV16(data.trans_pitch);
    data.trans_roll = __REV16(data.trans_roll);
    memcpy(&packet.user_data, &data, sizeof(data));
    memset(packet.trans_reserved,0,sizeof(packet.trans_reserved));
    for(i = 0; i < sizeof(packet) - 1;i++)
    {
      sum += *p++;
    }
    packet.sum = sum;
    p = (char*)&packet;
    DMA_StartTransfer(HW_DMA_CH1);
    return 0;
}

