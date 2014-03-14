#include "trans.h"
#include <string.h>
#include "common.h"

static trans_io_install_t * gpIOInstallStruct;   /* install struct	*/
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

//安装回调函数
uint8_t trans_io_install(trans_io_install_t * IOInstallStruct)
{
    gpIOInstallStruct = IOInstallStruct;
    return 0;
}

//发送数据包
uint32_t trans_send_pactket(trans_user_data_t data)
{
    trans_packet_t packet;
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
    for(i = 0; i < sizeof(packet); i++)
    {
      gpIOInstallStruct->trans_putc(*p++);
    }
    return 0;
}

