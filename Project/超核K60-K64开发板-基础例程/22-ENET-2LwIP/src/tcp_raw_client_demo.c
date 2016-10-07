#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"

#define TCP_SERVER_PORT        (5000)


static struct tcp_pcb *client_pcb;
static u8_t rx_buf[2048];
struct ip_addr ipaddr;
    
static void close_conn(struct tcp_pcb *pcb)
{
      tcp_arg(pcb, NULL);
      tcp_sent(pcb, NULL);
      tcp_recv(pcb, NULL);
      tcp_close(pcb);
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    int i;
    int len;
    char *pc;
    
    if (err == ERR_OK && p != NULL)
    {
        /* Inform TCP that we have taken the data. */
        tcp_recved(pcb, p->tot_len);  

        //pointer to the pay load
        pc = (char *)p->payload;  

        //size of the pay load
        len = p->tot_len; 

        //copy to our own buffer
        for (i=0; i<len; i++)
        {
            rx_buf[i]= pc[i]; 
            printf("%c", rx_buf[i]);
        }
        printf("\r\n");

        //Close TCP when receiving "X"  
        if (rx_buf[0]=='X')
        {
            close_conn(pcb); 
        }

        //Free the packet buffer 
        pbuf_free(p);

        //check output buffer capacity 
        if (len >tcp_sndbuf(pcb)) 
        {
            len = tcp_sndbuf(pcb); 
        }
        
        //Send out the data 
        err = tcp_write(pcb, rx_buf, len, 0); 
        tcp_sent(pcb, NULL); /* No need to call back */
    }
    else if(err != ERR_OK && p == NULL)
    {
        pbuf_free(p);
    }

    if (err == ERR_OK && p == NULL)
    {
        close_conn(pcb);
    }
    return ERR_OK;
}

err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    printf("tcp_client_connected local:port:%d remote port:%d\r\n", pcb->local_port, pcb->remote_port);
    
    if(err == ERR_OK)
    {
        tcp_recv(pcb, tcp_client_recv);
    }
    else
    {

    }
    
    return ERR_OK;
}

static void tcp_errf(void *arg, err_t err)
{  
    printf("tcp_client err: %d\r\n", err);
}

void tcp_client_demo_init(void)
{
    err_t err = ERR_OK; 
    
    client_pcb = tcp_new();
   
    IP4_ADDR(&ipaddr, 192, 168, 1, 102);
  
    u8_t *ip;
    ip = (u8_t*)&ipaddr;
    
    printf("TCP client demo: trying to connect: %u.%u.%u.%u server port:%d ...\r\n",ip[0], ip[1],ip[2], ip[3], TCP_SERVER_PORT);
    
    tcp_connect(client_pcb, &ipaddr, TCP_SERVER_PORT, tcp_client_connected);
    tcp_err(client_pcb, tcp_errf);  
}
