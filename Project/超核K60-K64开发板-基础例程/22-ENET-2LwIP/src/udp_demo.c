#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"


void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct 
ip_addr *addr, u16_t port)
{
    char buffer[512];
    if (p != NULL) 
    {
        if(pbuf_copy_partial(p, buffer, p->tot_len,0) != p->tot_len) 
        {
            LWIP_DEBUGF(LWIP_DBG_ON, ("pbuf_copy_partial failed\r\n"));
        } 
        else 
        {
            buffer[p->tot_len] = '\0';
            LWIP_DEBUGF(LWIP_DBG_ON, ("got %s\r\n", buffer));
        }
        // send received packet back to sender
        udp_sendto(pcb, p, addr, port);
        // free the pbuf
        pbuf_free(p);
    }
}

void udp_echo_init(void)
{
    struct udp_pcb * pcb;

    // get new pcb
    pcb = udp_new();
    if (pcb == NULL) {
        LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));
        return;
    }

    // bind to any IP address on port 7
    if (udp_bind(pcb, IP_ADDR_ANY, 7) != ERR_OK) {
        LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
        return;
    }
    printf("UDP bind to PORT:7 OK!\r\n");

    // set udp_echo_recv() as callback function
    // for received packets
    udp_recv(pcb, udp_echo_recv, NULL);
}
