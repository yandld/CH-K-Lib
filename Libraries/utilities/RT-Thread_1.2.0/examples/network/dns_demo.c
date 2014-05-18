#include <rtthread.h>
#include <lwip/netdb.h> /* 为了解析主机名，需要包含netdb.h头文件 */
#include <lwip/sockets.h> /* 使用BSD socket，需要包含sockets.h头文件 */


void dns_test(const char* url)
{
    struct hostent *h;

    /*  取得主机信息 */
    h= (struct hostent *) gethostbyname(url);
    if(h == NULL)
    {
        /*  gethostbyname失败 */
        rt_kprintf("Socket error\n");
        return;

    }
    /*  列印程序取得的信息 */
    rt_kprintf("Host name : %s\n", h->h_name);
    rt_kprintf("IP Address : %s\n", inet_ntoa (*((struct in_addr *)h->h_addr)));

    return ;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
/* 输出dns_test函数到finsh shell中 */
FINSH_FUNCTION_EXPORT(dns_test, dns test);
#endif
