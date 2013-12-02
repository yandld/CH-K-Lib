#ifndef __PINMUX_H__
#define __PINMUX_H__

#include "stdint.h"

typedef enum
{
    kPINMUX_Alt0,
    kPINMUX_Alt1,
    kPINMUX_Alt2,
    kPINMUX_Alt3,
    kPINMUX_Alt4,
    kPINMUX_Alt5,
    kPINMUX_Alt6,
    kPINMUX_Alt7,
    kPINMUX_MaxNum,
}PINMUX_Alt_Type;


//!< API functions
void PinMuxConfig(uint8_t GPIOIndex, uint8_t pinIndex, PINMUX_Alt_Type muxIndex);

#endif

