#include <rtthread.h>
#include <lwip/netdb.h> /* 为了解析主机名，需要包含netdb.h头文件 */
#include <lwip/sockets.h> /* 使用BSD socket，需要包含sockets.h头文件 */
#include <dfs_posix.h>

#define BUFSZ	512

static const char http_req_hdr_tmpl[] = "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\n\r\n"; /* 发送用到的数据 */


static int  _httpGetFileLen(char *revbuf)
{
    char *p1 = NULL, *p2 = NULL;
    int HTTP_Body = 0;//内容体长度

    p1 = strstr(revbuf,"Content-Length");
    if(p1 == NULL)
        return -1;
    else
    {
        p2 = p1+strlen("Content-Length")+ 2; 
        HTTP_Body = atoi(p2);
        return HTTP_Body;
    }

}

void http_parse_request_url(char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;

    /* 查找主机的开始位置 */
    begin = strstr(buf, "//");
    begin = (begin ? begin + 2 : buf);
    
    colon = strchr(begin, ':');
    host_end = strchr(begin, '/');

    if (host_end == NULL)
    {
        host_end = buf_end;
    }
    else
    {   /* 得到文件名 */
        file = strrchr(host_end, '/');
        if (file && (file + 1) != buf_end)
            strcpy(file_name, file + 1);
    }

    if (colon) /* 得到端口号 */
    {
        colon++;

        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port = atoi(port_buf);

        host_end = colon - 1;
    }

    /* 得到主机信息 */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}

void wget(const char* url)
{
    char *recv_data;
    struct hostent *host;
    int sock, bytes_received;
    struct sockaddr_in server_addr;

    unsigned short port = 80;
    char shost[128];
    char sname[128];
    http_parse_request_url((char*)url, shost, &port, sname);
    rt_kprintf("%s|%d|%s\r\n", shost, port, sname);
    
    host = gethostbyname(shost);
    
    /* 分配用于存放接收数据的缓冲 */
    recv_data = rt_malloc(BUFSZ);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /* 创建一个socket，类型是SOCKET_STREAM，TCP类型 */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* 创建socket失败 */
        rt_kprintf("Socket error\n");

        /* 释放接收缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 连接到服务端 */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 连接失败 */
        rt_kprintf("Connect fail!\n");
        lwip_close(sock);

        /*释放接收缓冲 */
        rt_free(recv_data);
        return;
    }
  
    /* 发送 HTTP 请求 */
    int send_len;
    send_len = sprintf(recv_data, http_req_hdr_tmpl, sname, host->h_name);
    printf("%s", recv_data);
    send(sock, recv_data, send_len, 0);

    rt_kprintf("connected\r\n");
    
    int file_size, offset;
    char *p;
    bytes_received = recv(sock, recv_data, BUFSZ, 0);
    
    /* get file size */
    file_size = _httpGetFileLen(recv_data);
    if(file_size <=0)
    {
        /* 连接失败 */
        rt_kprintf("cannot get file size!\n");
        lwip_close(sock);

        /*释放接收缓冲 */
        rt_free(recv_data);
        return; 
    }
    
    /* get start */
    p = strstr(recv_data,"\r\n\r\n");
    if(p == NULL)
    {
        /* 连接失败 */
        rt_kprintf("cannot find content!\n");
        lwip_close(sock);

        /*释放接收缓冲 */
        rt_free(recv_data);
        return;
    }
    
    offset = p - recv_data;
    rt_kprintf("size:%d offset:%d\r\n", file_size, offset);
    
    int fd;
    fd = open("/SD/SYS/PIC/TEST.GIF", O_RDWR | O_CREAT | O_TRUNC, 0);

    if (fd < 0)
    {
        rt_kprintf("open file failed\r\n");
        /* 连接失败 */
        rt_kprintf("cannot find content!\n");
        lwip_close(sock);

        /*释放接收缓冲 */
        rt_free(recv_data);
        return;
    }

    write(fd, recv_data+offset, bytes_received - offset);
 
    while(1)
    {
        /* 从sock连接中接收最大BUFSZ - 1字节数据 */
        bytes_received = recv(sock, recv_data, BUFSZ, 0);
        if (bytes_received <= 0)
        {
            /* 接收失败，关闭这个连接 */
            lwip_close(sock);

            /* 释放接收缓冲 */
            rt_free(recv_data);
            close(fd);
            break;
        }
        rt_kprintf("data:%d\r\n", bytes_received);
        write(fd, recv_data, bytes_received);
    }
    return;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
/* 输出tcpclient函数到finsh shell中 */
FINSH_FUNCTION_EXPORT(wget, startup tcp client);
#endif
