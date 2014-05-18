#include <rtthread.h>
#include <lwip/netdb.h> /* 为了解析主机名，需要包含netdb.h头文件 */
#include <lwip/sockets.h> /* 使用BSD socket，需要包含sockets.h头文件 */


void net_demo_nfs(const char* url)
{
    if (dfs_mount(RT_NULL, "/NFS", "nfs", 0, url) == 0)
    rt_kprintf("NFSv3 File System initialized!\n");
    else
    rt_kprintf("NFSv3 File System initialzation failed!\n");

    return ;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
/* 输出dns_test函数到finsh shell中 */
FINSH_FUNCTION_EXPORT(net_demo_nfs, dns test eg:nfs("192.168.1.201:/"));
#endif
