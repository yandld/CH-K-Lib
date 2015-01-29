#include "components.h"
#include "board.h"
#include "rtt_ksz8041.h"


int network_startup(int argc, char** argv)
{
    
#ifdef RT_USING_LWIP
    rt_hw_ksz8041_init(BOARD_ENET_PHY_ADDR);
#endif
    
    return 0;
}

#ifdef FINSH_USING_MSH
#include "finsh.h"

MSH_CMD_EXPORT(network_startup, eg:usb sd0);

#endif


