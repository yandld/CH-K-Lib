#include <rtthread.h>
#include <finsh.h>
#include "GUI.H"

int cmd_guiinfo(int argc, char** argv)
{
    rt_kprintf("NumFreeBlocks:%d\r\n", GUI_ALLOC_GetNumFreeBlocks());
    rt_kprintf("GUI_ALLOC_GetNumFreeBytes:%d\r\n", GUI_ALLOC_GetNumFreeBytes());
    rt_kprintf("GUI_ALLOC_GetNumUsedBytes:%d\r\n", GUI_ALLOC_GetNumUsedBytes());
    return 0;
}


FINSH_FUNCTION_EXPORT_ALIAS(cmd_guiinfo, __cmd_guiinfo, show gui info.);






