#include <rtthread.h>
#include "components.h"
 
#ifdef RT_USING_DFS_NFS
#ifdef FINSH_USING_MSH
#include <finsh.h>

int mountnfs(const char * host)
{
    int r;
    if(!(netif_list->flags & NETIF_FLAG_UP)) 
    {
        rt_kprintf("no network\r\n");
        return -1;
    }

    r = dfs_mount("e0", "/NFS", "nfs", 0, host);
    (!r)?(rt_kprintf("mount OK\r\n")):(rt_kprintf("mount failed\r\n"));
    return 0;
}

FINSH_FUNCTION_EXPORT(mountnfs, eg:mountnfs("192.168.1.101:/"))

#endif
#endif



