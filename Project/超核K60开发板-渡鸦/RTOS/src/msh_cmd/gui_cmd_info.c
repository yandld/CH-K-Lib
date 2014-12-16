#include <rtthread.h>
#include "GUI.H"

#ifdef FINSH_USING_MSH
#include "finsh.h"

int ui_guiinfo(int argc, char** argv)
{
    rt_kprintf("NumFreeBlocks:%d\r\n", GUI_ALLOC_GetNumFreeBlocks());
    rt_kprintf("GUI_ALLOC_GetNumFreeBytes:%d\r\n", GUI_ALLOC_GetNumFreeBytes());
    rt_kprintf("GUI_ALLOC_GetNumUsedBytes:%d\r\n", GUI_ALLOC_GetNumUsedBytes());
    return 0;
}


MSH_CMD_EXPORT(ui_guiinfo, show a picture file.);
#endif





