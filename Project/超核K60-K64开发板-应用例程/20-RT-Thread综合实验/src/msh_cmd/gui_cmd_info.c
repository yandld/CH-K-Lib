#include <rtthread.h>
#include "drv_ksz8041.h"
#include "components.h"
#include "common.h"
#include "GUI.H"

#ifdef FINSH_USING_MSH
#include "finsh.h"

int sys_info(int argc, char** argv)
{
    uint32_t clock, tick;
    rt_kprintf("CPU CLOCK:\r\n");
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    rt_kprintf("CoreClock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    rt_kprintf("BusClock:%dHz\r\n", clock);
    
    rt_kprintf("SYSTEM TIME\r\n");
    tick = rt_tick_get();
    rt_kprintf("tick:%d | time:%ds\r\n", tick, tick/RT_TICK_PER_SECOND);
    
    rt_kprintf("MEMORY\r\n");
    extern void list_mem(void);
    list_mem();
    rt_kprintf("UI MEMORY:  ");
    //rt_kprintf("ui_mem_total:%d\r\n", GUI_ALLOC_GetNumUsedBytes()+GUI_ALLOC_GetNumFreeBytes());
    //rt_kprintf("ui_mem_free:%d\r\n", GUI_ALLOC_GetNumFreeBytes());
    
    rt_kprintf("NETWORK\r\n");
    rt_device_t dev;
    dev = rt_device_find("e0");
    if(dev)
    {
        enet_phy_data phy_data;
        extern void list_if(void);
        list_if();
        dev->control(dev, NIOCTL_GET_PHY_DATA, &phy_data);
        rt_kprintf("\r\n");
        rt_kprintf("TX:%05d | %05dKB\r\n", phy_data.tx_fcnt, phy_data.tx_dcnt/1024);
        rt_kprintf("RX:%05d | %05dKB\r\n", phy_data.rx_fcnt, phy_data.rx_dcnt/1024);
    }
    return 0;
}


MSH_CMD_EXPORT(sys_info, show a picture file.);
#endif





