#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>

#define CMD_ERROR               0x00

#define CMD_S2H_DATA            0xAF
#define CMD_S2H_READ_FW         0xA0
#define CMD_S2H_DATA_FW         0xA2
#define CMD_S2H_READ_OFFSET     0xA1
#define CMD_S2H_DATA_OFFSET     0xA3

#define CMD_H2S_DATA            0x8F
#define CMD_H2S_READ_FW         0x80
#define CMD_H2S_DATA_FW         0x82
#define CMD_H2S_READ_OFFSET     0x81
#define CMD_H2S_WRITE_OFFSET    0x83

/* protocol:
0x88 + <PROT_FUN> + <LEN> + <DATA> + SUM

<PROT_FUN>: 0xAF: general data(S->H)   0x0F: general data(H->S)
    <DATA>:data specific

<PROT_FUN>: 0xA5: general cmd(S->H)    0x05: general cmd(H->S)
    <DATA>: <CMD> + <ID> + data
        IMU_OffsetAll + <ID> + AccX + AccY + AccZ + GyroX + GyroY + GyroZ + MagX + MagY +MagZ
        IMU_CmdConfig + <ID>
        IMU_CmdRun    + <ID>
*/

#define PROT_FUN_DATA           0xAF /* general data */
#define PROT_FUN_CMD            0xA5 /* general cmd */

/* A5 section */
enum
{
    IMU_OffsetAll,
    IMU_OffsetAcc,
    IMU_OffsetGyro,
    IMU_OffsetMag,
    IMU_FwInfo,
    IMU_CmdRun,
    IMU_CmdConfig,
};


typedef __packed struct
{
    int16_t acc[3];
    int16_t gyo[3];
    int16_t mag[3];
    int16_t R;
    int16_t P;
    int16_t Y;
    int32_t pressure;
}payload_t;

typedef __packed struct
{
    uint8_t version;
    uint32_t uid;
}fw_info_t;

typedef __packed struct
{
    int16_t acc_offset[3];
    int16_t gyro_offset[3];
    int16_t mag_offset[3];
}offset_t;

typedef struct
{
    uint8_t buf[32];
    uint8_t len;
    uint8_t cmd;
}rev_data_t;


//!< API

uint32_t ano_encode_packet(payload_t* data, uint8_t* buf);
uint32_t ano_encode_fwinfo(fw_info_t* fwinfo, uint8_t* buf);
uint32_t ano_encode_offset_packet(offset_t* offset, uint8_t* buf);
int ano_rec(uint8_t ch, rev_data_t *rd);


#endif


