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

typedef struct
{
    uint8_t (*trans_getc)(void);
    void    (*trans_putc)(uint8_t ch);
}trans_io_install_t;

uint8_t trans_io_install(trans_io_install_t * IOInstallStruct);
uint32_t trans_send_pactket(trans_user_data_t data);

#endif
