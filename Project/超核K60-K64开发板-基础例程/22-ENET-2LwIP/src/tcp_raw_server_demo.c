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

#define TCP_SERVER_PORT        (7)

/* code from:  http://www.ultimaserial.com/avr_lwip_tcp.html */

//enum echo_states
//{
//    ES_NONE = 0,
//    ES_ACCEPTED,    /* client has connected us */
//    ES_RECEIVED,    /* a packet has been recevied */
//    ES_CLOSING      
//};

//struct echo_state
//{
//    u8_t state;
//    u8_t retries;
//    struct tcp_pcb *pcb;
//    // pbuf (chain) to recycle
//    struct pbuf *p;
//};

err_t echo_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t echo_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
//void echo_error(void *arg, err_t err);
//err_t echo_poll(void *arg, struct tcp_pcb *tpcb);
//err_t echo_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//void echo_send(struct tcp_pcb *tpcb, struct echo_state *es);
//void echo_close(struct tcp_pcb *tpcb, struct echo_state *es);


static struct tcp_pcb *echo_pcb;
u8_t rx_buf[2048];

void tcp_sever_demo_init(void)
{
    
    echo_pcb = tcp_new();
    if (echo_pcb != NULL)
    {
        tcp_bind(echo_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);  
        echo_pcb = tcp_listen(echo_pcb);
        tcp_accept(echo_pcb, echo_accept);
        printf("TCP server demo: bind to PORT:%d OK!\r\n", TCP_SERVER_PORT);
    }
    else
    {
        // abort? output diagnostic?
    }
}

static err_t echo_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    
    tcp_setprio(pcb, TCP_PRIO_MIN);
    tcp_recv(pcb, echo_recv);
    tcp_err(pcb, NULL); //Don't care about error here
    tcp_poll(pcb, NULL, 4); //No polling here
    
    return ERR_OK;
}

static void close_conn(struct tcp_pcb *pcb)
{
      tcp_arg(pcb, NULL);
      tcp_sent(pcb, NULL);
      tcp_recv(pcb, NULL);
      tcp_close(pcb);
}

err_t echo_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
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




