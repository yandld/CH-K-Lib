#include <rtthread.h>
#include "common.h"
 
#ifdef FINSH_USING_MSH
#include <finsh.h>

static int reset(int argc, char** argv)
{
    NVIC_SystemReset();
    return 0;
}
MSH_CMD_EXPORT(reset, reset);

#endif





