#include <rtthread.h>
 
#ifdef FINSH_USING_MSH
#include <finsh.h>

int cmd_nfs(int argc, char** argv)
{
   // NVIC_SystemReset();
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_nfs, __cmd_nfs, nfs.);

#endif





