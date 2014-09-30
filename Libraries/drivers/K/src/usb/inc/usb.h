#ifndef __CHLIB_USB_H_
#define	__CHLIB_USB_H_

#include <stdint.h>

//!< API functions
uint8_t USB_Init(void);
void USB_DisConnect(void);
void USB_Connect(void);

#endif

