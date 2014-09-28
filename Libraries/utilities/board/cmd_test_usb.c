#include "shell.h"
#include "gpio.h"
#include "board.h"

#include "usb.h"

int DoUSB(int argc, char * const argv[])
{
    USB_Init();
    return 0;
}

SHELL_EXPORT_CMD(DoUSB, usb , usb test)


