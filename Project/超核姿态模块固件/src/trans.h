#ifndef _TRANS_H_
#define _TRANS_H_

#include <stdint.h>

typedef __packed struct
{
    int16_t trans_accel[3];
    int16_t trans_gyro[3];
    int16_t trans_mag[3];
    int16_t trans_roll;
    int16_t trans_pitch;
    int16_t trans_yaw;
}trans_user_data_t;


#define  TRANS_UART_WITH_DMA        (0x00)
#define  TRANS_WITH_NRF2401              (0x01)

uint32_t trans_send_pactket(trans_user_data_t data, uint8_t mode);
uint8_t trans_init(void);

#endif
