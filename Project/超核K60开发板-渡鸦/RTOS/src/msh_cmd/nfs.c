#include <rtthread.h>
#include "components.h"
 
#ifdef RT_USING_DFS_NFS
#ifdef FINSH_USING_MSH
#include <finsh.h>

int mountnfs(const char * host)
{
    int r;
	rt_kprintf("mount nfs to %s...", "/NFS");
    r = dfs_mount("e0", "/NFS", "nfs", 0, host);
	if (!r)
	{
		rt_kprintf("[ok]\n");
		return 0;
	}
	else
	{
		rt_kprintf("[failed!]%d\n", r);
		return -1;
	}
}

FINSH_FUNCTION_EXPORT(mountnfs, eg:mountnfs("192.168.1.101:/"))

#endif
#endif



